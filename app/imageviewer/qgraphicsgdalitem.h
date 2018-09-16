#ifndef QGRAPHICSGDALITEM_H
#define QGRAPHICSGDALITEM_H
#include <gdal_priv.h>
#include <ogr_spatialref.h>
#include <QGraphicsItem>

class QGraphicsGDALItem : public QGraphicsItem {
 private:
  QString name; //!< Path of image
  QSize  size_raster;//!< Size of raster
  int number_bands;//!< number of bands
  int list_bands[3];//!< display bands, in r g b order
  bool geoRefInited;
  double padfTransform_[6];  //!< transformation in GeoTiff
  double padfTransformInv_[4]; //!< inversed transformation from Geo To Pixel
  OGRSpatialReference oTiffRS_;
  OGRSpatialReference oWGS84_;
  OGRCoordinateTransformation *poTiff2WGS84;
  OGRCoordinateTransformation *poWGS84ToTiff;
  std::map<int,int> GDALTypeByteTable;
 public:
  QGraphicsGDALItem(QGraphicsItem *parent = 0);
  void load_image(const QString &name_file);
  virtual QRectF boundingRect() const;
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
  void mapPixelToProj(double P, double L, double *Xp, double *Yp);
  void mapProjToPixel(double Xp, double Yp, double *P, double *L);
  void mapWGS84ToProj(double lon, double lat, double *Xp, double *Yp);
  void mapProjToWGS84(double Xp, double Yp, double *lon, double *lat);
  void mapPixelToWGS84(double P, double L, double *lon,double *lat);
  void mapWGS84ToPixel(double lon, double lat, double *P, double *L);
};

#endif // QGRAPHICSGDALITEM_H
