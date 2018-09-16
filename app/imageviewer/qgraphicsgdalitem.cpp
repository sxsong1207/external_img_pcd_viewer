#include "qgraphicsgdalitem.h"
#include <QFile>
#include <QPainter>
#include <QDebug>
#include <QtMath>
#include <QMessageBox>
#if defined(_OPENMP)
#include <omp.h>
#endif
QGraphicsGDALItem::QGraphicsGDALItem(QGraphicsItem *parent): QGraphicsItem(parent) {
  number_bands=0;
  geoRefInited=false;
  setFlag(QGraphicsItem::ItemIsMovable, false);
  oWGS84_.SetWellKnownGeogCS("WGS84");

  GDALTypeByteTable[GDT_Unknown]=0;
  GDALTypeByteTable[GDT_Byte]=1;
  GDALTypeByteTable[GDT_UInt16]=2;
  GDALTypeByteTable[GDT_Int16]=2;
  GDALTypeByteTable[GDT_CInt16]=2;
  GDALTypeByteTable[GDT_UInt32]=4;
  GDALTypeByteTable[GDT_Int32]=4;
  GDALTypeByteTable[GDT_Float32]=4;
  GDALTypeByteTable[GDT_CInt32]=4;
  GDALTypeByteTable[GDT_Float64]=4;
  GDALTypeByteTable[GDT_CFloat64]=4;
}

void QGraphicsGDALItem::load_image(const QString &name_file) {
  prepareGeometryChange();
  name=name_file;
  size_raster=QSize(0,0);
  number_bands=0;

  if (!name.isEmpty()) {
    GDALDataset  *pdata;
    pdata=(GDALDataset *)GDALOpen(name.toLocal8Bit().data(),GA_ReadOnly);
    if (pdata!=0) {
      int numCount = (pdata->GetRasterBand(1))->GetOverviewCount();
      QFile ovrfile(name+".ovr");
      // build LOD overview if not exist
      if (!ovrfile.exists()&&numCount==0) {
        int anOverviewList[3] = { 4, 8, 16};
        pdata->BuildOverviews("GAUSS", 3, anOverviewList, 0, NULL,
                              NULL, NULL);
      }
      int w,h;

      w=pdata->GetRasterXSize();
      h=pdata->GetRasterYSize();
      pdata->GetGeoTransform(padfTransform_);
      {
        double detRot = padfTransform_[1]*padfTransform_[5]-padfTransform_[2]*padfTransform_[4];
        padfTransformInv_[0] = padfTransform_[5]/detRot;
        padfTransformInv_[1] = -padfTransform_[2]/detRot;
        padfTransformInv_[2] = -padfTransform_[4]/detRot;
        padfTransformInv_[3] = padfTransform_[1]/detRot;
      }
      const char *wtk[]= {pdata->GetProjectionRef(),NULL};
      oTiffRS_.importFromWkt((char **)wtk);

      try {
        poTiff2WGS84 = OGRCreateCoordinateTransformation(&oTiffRS_,&oWGS84_);
        poWGS84ToTiff = OGRCreateCoordinateTransformation(&oWGS84_,&oTiffRS_);
        geoRefInited=true;
      } catch (std::exception e) {
        geoRefInited=false;
      }

      size_raster=QSize(w,h);

      number_bands=pdata->GetRasterCount();
      if (number_bands==1) {
        list_bands[0]= 1;
      };
      if (number_bands>=3) {

        list_bands[0]=1;
        list_bands[1]=2;
        list_bands[2]=3;

        if (number_bands==8) {
          list_bands[0]=2;
          list_bands[1]=3;
          list_bands[2]=5;
        }
      };
      GDALClose(pdata);
    };
  };
  geoRefInited=true;
}

QRectF QGraphicsGDALItem::boundingRect() const {
  return QRectF(QPointF(0,0),size_raster) ;
}

template<typename T>
double getValue(void *pointer) {
  return (double)*reinterpret_cast<T *>(pointer);
}

void QGraphicsGDALItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
  QTransform transform;
  int W,H;
  QPaintDevice *pdevice;
  pdevice=painter->device() ;
  QRectF bound;

  bound=boundingRect();
  if (pdevice!=0) {
    W=pdevice->width();
    H=pdevice->height();

    transform=painter->combinedTransform();

    QRectF window_rect_in_LSC;
    QRectF workig_area_in_LSC;
    QRectF workig_area_in_DSC;
    qreal xw1,xw2,yw1,yw2;
    qreal cf1,cf2,rf1,rf2;

    window_rect_in_LSC=transform.inverted().mapRect(QRectF(0,0,W,H));

    window_rect_in_LSC.getCoords(&cf1, &rf1, &cf2, &rf2);

    workig_area_in_LSC=bound.intersected(QRectF(window_rect_in_LSC));

    workig_area_in_DSC=transform.mapRect(workig_area_in_LSC);

    workig_area_in_LSC.getCoords(&cf1, &rf1, &cf2, &rf2);
    workig_area_in_DSC.getCoords(&xw1, &yw1, &xw2, &yw2);

    painter->save();
    painter->resetTransform();
    if (!name.isEmpty()) {
      GDALDataset  *pdata;

      pdata=(GDALDataset *)GDALOpen(name.toLocal8Bit().data(),GA_ReadOnly);

      if (pdata!=0) {
        int nb_working;
        QImage im;

        if ((!workig_area_in_DSC.isEmpty())&&((number_bands==1)||(number_bands>=3))) {

          int ww,hw;
          int bpp;

          ww=xw2-xw1+1;
          hw=yw2-yw1+1;

          if (number_bands==1) {
            im=QImage(ww,hw,QImage::Format_Indexed8);
            bpp=1;
            nb_working=1;

            int k;
            im.setColorCount(256);
            GDALColorTable *pcolor_table;
            pcolor_table=(pdata->GetRasterBand(1))->GetColorTable();

            for (k=0; k<256; k++) {
              if (pcolor_table!=0) {
                const GDALColorEntry *pcolor;
                pcolor=pcolor_table->GetColorEntry(k);
                im.setColor(k, QColor(pcolor->c1,pcolor->c2,pcolor->c3).rgb());
              } else {
                im.setColor(k, QColor(k,k,k).rgb());
              }
            }
          } else if (number_bands>=3) {
            im=QImage(ww,hw,QImage::Format_RGB32);
            bpp=4;
            nb_working=3;
          }

          GDALDataType valType = pdata->GetRasterBand(1)->GetRasterDataType();
          if (valType==GDT_Unknown || valType>=GDT_CInt16) {
            //unsupport types
            return;
          }
          int bitdepth=GDALTypeByteTable[valType];

          int wf,hf;

          wf=cf2-cf1+1;
          hf=rf2-rf1+1;

          if ((cf1+wf)>size_raster.width()-1) {
            wf=size_raster.width()-1-cf1;
          }
          if ((rf1+hf)>size_raster.height()-1) {
            hf=size_raster.height()-1-rf1;
          }

//          int unitPerLine = qCeil((float)im.width()*bpp/4.0)*4;

          int srcBytePerLine = qCeil((float)im.width()*bpp*bitdepth/4.0)*4;
          quint8 *parr = new quint8[srcBytePerLine*im.height()];
          pdata->RasterIO(GF_Read,cf1,rf1, wf,hf,parr,im.width(),im.height(),valType,
                          nb_working,list_bands,bpp*bitdepth,srcBytePerLine,bitdepth);
          double *minband=new double[nb_working];
          double *maxband=new double[nb_working];
          double *meanband=new double[nb_working];
          double *stdband=new double[nb_working];
          for (int bi=0; bi<nb_working; ++bi) {
            GDALRasterBand *band = pdata->GetRasterBand(list_bands[bi]);
            band->GetStatistics(TRUE,TRUE,minband+bi,maxband+bi,meanband+bi,stdband+bi);
          }

          double (*valueReader)(void *pointer);
          switch (valType) {
          case GDT_Byte:
            valueReader = &getValue<quint8>;
            break;
          case GDT_UInt16:
            valueReader = &getValue<quint16>;
            break;
          case GDT_Int16:
            valueReader = &getValue<qint16>;
            break;
          case GDT_UInt32:
            valueReader = &getValue<quint32>;
            break;
          case GDT_Int32:
            valueReader = &getValue<qint32>;
            break;
          case GDT_Float32:
            valueReader = &getValue<float>;
            break;
          case GDT_Float64:
            valueReader = &getValue<double>;
            break;
          }

          #pragma omp parallel for collapse(3)
          for (int ci=0; ci<im.width(); ++ci)
            for (int ri=0; ri<im.height(); ++ri) {
              for (int bi=0; bi<bpp; ++bi) {
                int srcIdx = srcBytePerLine*ri+(bpp*ci+bi)*bitdepth;
                int dstIdx = im.bytesPerLine()*ri+bpp*ci+bi;

                double tempval = (valueReader(parr+srcIdx)-meanband[bi])*255.0/3/stdband[bi]+128;

                if (tempval>255) {
                  im.bits()[dstIdx] = 255;
                } else if (tempval<0) {
                  im.bits()[dstIdx]=0;
                } else {
                  im.bits()[dstIdx]=tempval;
                }
              }
            }

          delete[] parr;

          painter->drawImage(workig_area_in_DSC,im);
        };
        GDALClose(pdata);
      };
    };

    painter->restore();
  };
}

void QGraphicsGDALItem::mapPixelToProj(double P, double L, double *Xp, double *Yp) {
  if (!geoRefInited) {
    return;
  }
  *Xp = padfTransform_[0] + P*padfTransform_[1]+L*padfTransform_[2];
  *Yp = padfTransform_[3] + P*padfTransform_[4]+L*padfTransform_[5];
}

void QGraphicsGDALItem::mapProjToPixel(double Xp, double Yp, double *P, double *L) {
  if (!geoRefInited) {
    return;
  }
  *P = padfTransformInv_[0]*(Xp - padfTransform_[0]) + padfTransformInv_[1]*(Yp - padfTransform_[3]);
  *L = padfTransformInv_[2]*(Xp - padfTransform_[0]) + padfTransformInv_[3]*(Yp - padfTransform_[3]);
}

void QGraphicsGDALItem::mapWGS84ToProj(double lon, double lat, double *Xp, double *Yp) {
  if (!geoRefInited) {
    return;
  }
  if (poWGS84ToTiff) {
    *Xp = lon;
    *Yp = lat;
    poWGS84ToTiff->Transform(1,Xp,Yp);
  }
}

void QGraphicsGDALItem::mapProjToWGS84(double Xp, double Yp, double *lon, double *lat) {
  if (!geoRefInited) {
    return;
  }
  if (poTiff2WGS84) {
    *lon=Xp;
    *lat=Yp;
    poTiff2WGS84->Transform(1,lon,lat);
  }
}

void QGraphicsGDALItem::mapPixelToWGS84(double P, double L, double *lon, double *lat) {
  if (!geoRefInited) {
    return;
  }
  double Xp, Yp;
  mapPixelToProj(P,L,&Xp,&Yp);
  mapProjToWGS84(Xp,Yp,lon,lat);
}

void QGraphicsGDALItem::mapWGS84ToPixel(double lon, double lat, double *P, double *L) {
  if (!geoRefInited) {
    return;
  }
  double Xp,Yp;
  mapWGS84ToProj(lon,lat,&Xp,&Yp);
  mapProjToPixel(Xp,Yp,P,L);
}
