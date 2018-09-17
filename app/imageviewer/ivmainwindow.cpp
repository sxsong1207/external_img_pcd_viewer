#include "ivmainwindow.h"
#include "ui_ivmainwindow.h"
#include "qgraphicsgdalitem.h"
#include <QTimer>
#include <QGraphicsRectItem>
#include <QFileDialog>
#include <QDebug>
#include <QTextBrowser>
#include <QMessageBox>
#include <QScrollBar>

IVMainWindow::IVMainWindow(QWidget *parent) :
  QMainWindow(parent),
  regionSet(false),
  ui(new Ui::IVMainWindow),
  scene_(new QGraphicsScene),
  rectItem_(new QGraphicsRectItem),
  handler_1(new MyGraphicsHandlerItem),
  handler_2(new MyGraphicsHandlerItem),
  gdalItem_(new QGraphicsGDALItem),
  stateLabel_(new QLabel),
  proc_(new QProcess) {
  ui->setupUi(this);
  ui->mainView->setScene(scene_);
  scene_->addItem(gdalItem_);
  scene_->addItem(rectItem_);
  scene_->addItem(handler_1);
  scene_->addItem(handler_2);

  handler_1->setZValue(10);
  handler_2->setZValue(12);

  rectItem_->setPen(QPen(QBrush(Qt::red),2,Qt::DashLine));
  rectItem_->setBrush(QBrush(QColor(30,30,200,50)));

  handler_1->setRect(-6,-6,12,12);
  handler_1->setPen(QPen(QBrush(Qt::red),3,Qt::DashLine));
  handler_1->setBrush(QBrush(QColor(30,30,200,100)));
  handler_1->setFlag(QGraphicsItem::ItemIsSelectable, true);
  handler_1->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
  handler_1->setFlag(QGraphicsItem::ItemIsFocusable, true);
  handler_1->setFlag(QGraphicsItem::ItemIgnoresTransformations,true);

  handler_2->setRect(-6,-6,12,12);
  handler_2->setPen(QPen(QBrush(Qt::red),3,Qt::DashLine));
  handler_2->setBrush(QBrush(QColor(30,200,30,100)));
  handler_2->setFlag(QGraphicsItem::ItemIsMovable, true);
  handler_2->setFlag(QGraphicsItem::ItemIsSelectable, true);
  handler_2->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
  handler_2->setFlag(QGraphicsItem::ItemIsFocusable, true);
  handler_2->setFlag(QGraphicsItem::ItemIgnoresTransformations,true);

  statusBar()->addPermanentWidget(stateLabel_);
  stateLabel_->setText("Idle");
  connect(handler_1,&MyGraphicsHandlerItem::posChanged,this,&IVMainWindow::handlerMove);
  connect(handler_2,&MyGraphicsHandlerItem::posChanged,this,&IVMainWindow::handlerMove);
  connect(ui->mainView,&GDALView::position_on_shot,this,&IVMainWindow::mouseMoveInView);
  connect(proc_,&QProcess::readyRead,this,&IVMainWindow::on_proc_readyRead);
  connect(proc_,&QProcess::stateChanged,this,&IVMainWindow::onProcStateChanged);
  QTimer::singleShot(1000,this,&IVMainWindow::on_actionOpenFolder_triggered);

  QString appDir = qApp->applicationDirPath();
  QFileInfo appDirInfo(appDir);
  mvs3dprogram_path = appDirInfo.dir().filePath("MVS3D_Bin/MSV3D.exe");
  qDebug()<<mvs3dprogram_path;
}

IVMainWindow::~IVMainWindow() {
  delete ui;
}

void IVMainWindow::mouseMoveInView(double x, double y) {
  QPointF p_scene = ui->mainView->mapToScene(QPoint(x,y));

  double Xp,Yp;
  double lon,lat;
  gdalItem_->mapPixelToProj(p_scene.x(),p_scene.y(),&Xp,&Yp);
  gdalItem_->mapProjToWGS84(Xp,Yp,&lon,&lat);
  statusBar()->showMessage(tr("px:%1 py:%2 ux:%3 uy:%4 lon:%5 lat:%6").arg(p_scene.x())
                           .arg(p_scene.y()).arg(Xp,3,'f').arg(Yp,3,'f').arg(lon,6,'f').arg(lat,6,'f'));
}

void IVMainWindow::handlerMove() {
  if(!gdalItem_->geoRefInited){ return;}
  float minx,maxx,miny,maxy;
  minx=handler_1->x()<handler_2->x()?handler_1->x():handler_2->x();
  maxx=handler_1->x()>handler_2->x()?handler_1->x():handler_2->x();
  miny=handler_1->y()<handler_2->y()?handler_1->y():handler_2->y();
  maxy=handler_1->y()>handler_2->y()?handler_1->y():handler_2->y();

  rectItem_->setRect(minx,miny,maxx-minx,maxy-miny);
  regionSet=true;
}

void IVMainWindow::on_actionFitView_triggered() {
  scene_->setSceneRect(gdalItem_->boundingRect());
  ui->mainView->fitInView(scene_->sceneRect(),Qt::KeepAspectRatio);
}

void IVMainWindow::on_actionExportKML_triggered() {
    if(kml_path.isEmpty())
    {
        outputKML(kml_path);
    }
}

void IVMainWindow::outputXML(QString path)
{
    QFile file(":/MVS3D_xml_template.txt");
    file.open(QIODevice::ReadOnly);
    QString content = file.readAll();
    file.close();

    content.replace("${RESULT_FOLDER}",Result_path);
    content.replace("${INPUT_PAN_FOLDER}",PAN_path);
    content.replace("${INPUT_MSI_FOLDER}",MSI_path);
    content.replace("${KML_PATH}",kml_path);
    content.replace("${PRIORITY_PATH}",pairlist_path);

    QFile output(path);
    output.open(QIODevice::WriteOnly);
    QTextStream stream(&output);
    stream<<content;
    output.close();
}

void IVMainWindow::outputKML(QString path)
{
    QFile file(":/kml_template.txt");
    file.open(QIODevice::ReadOnly);
    QString content = file.readAll();
    file.close();

    double lat[4];
    double lon[4];
    gdalItem_->mapPixelToWGS84(rectItem_->rect().left(),rectItem_->rect().top(),&lon[0],&lat[0]);
    gdalItem_->mapPixelToWGS84(rectItem_->rect().right(),rectItem_->rect().top(),&lon[1],&lat[1]);
    gdalItem_->mapPixelToWGS84(rectItem_->rect().right(),rectItem_->rect().bottom(),&lon[2],&lat[2]);
    gdalItem_->mapPixelToWGS84(rectItem_->rect().left(),rectItem_->rect().bottom(),&lon[3],&lat[3]);

    QString coords=
      tr("\t%1,%2,0\n\t%3,%4,0\n\t%5,%6,0\n\t%7,%8,0\n\t%1,%2,0")
      .arg(lon[0],0,'f',9).arg(lat[0],0,'f',9).arg(lon[1],0,'f',9).arg(lat[1],0,'f',9)
      .arg(lon[2],0,'f',9).arg(lat[2],0,'f',9).arg(lon[3],0,'f',9).arg(lat[3],0,'f',9);

    content.replace("${RECT_COORDS}",coords);

    QFile output(path);
    output.open(QIODevice::WriteOnly);
    QTextStream stream(&output);
    stream<<content;
    output.close();
}

void IVMainWindow::on_actionRun_triggered() {
  ui->outputBox->clear();
  outputKML(kml_path);
  outputXML(runfile_path);
  proc_->start(mvs3dprogram_path,QStringList()<<runfile_path);
//  proc_->start("ping",QStringList()<<"www.google.com");
}

void IVMainWindow::on_imgListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous) {
  if(current==NULL) {
      gdalItem_->close_img();
      return;
  }
  QString path = current->data(Qt::UserRole).toString();
  qDebug()<<path;

  if(regionSet && gdalItem_->geoRefInited)
  {
      double lat,lon;
      gdalItem_->mapPixelToWGS84(rectItem_->rect().right(),rectItem_->rect().top(),&lon,&lat);
      maxLat = lat;
      maxLon = lon;
      gdalItem_->mapPixelToWGS84(rectItem_->rect().left(),rectItem_->rect().bottom(),&lon,&lat);
      minLat=lat;
      minLon=lon;
  }

  if (!path.isEmpty()) {
    gdalItem_->load_image(path);
  } else {
    return;
  }

  if(gdalItem_->geoRefInited)
  {
      handler_1->setVisible(true);
      handler_2->setVisible(true);
      rectItem_->setVisible(true);
      if(!regionSet )
      {
          handler_1->setPos(gdalItem_->boundingRect().topLeft());
          handler_2->setPos(gdalItem_->boundingRect().bottomRight());
          rectItem_->setRect(gdalItem_->boundingRect());
      }else{
          double right,top,left,bottom;
          gdalItem_->mapWGS84ToPixel(minLon,minLat,&left,&bottom);
          gdalItem_->mapWGS84ToPixel(maxLon,maxLat,&right,&top);
          handler_1->setPos(QPointF(left,top));
          handler_2->setPos(QPointF(right,bottom));
          rectItem_->setRect(QRectF(QPointF(left,top),QPointF(right,bottom)));
      }
  }else{
      handler_1->setVisible(false);
      handler_2->setVisible(false);
      rectItem_->setVisible(false);
  }

  scene_->setSceneRect(gdalItem_->boundingRect());
  ui->mainView->fitInView(scene_->sceneRect(),Qt::KeepAspectRatio);
}

void IVMainWindow::on_actionOpenFolder_triggered() {

  QString folderPath = QFileDialog::getExistingDirectory(this,tr("Open Work Folder"),"",QFileDialog::ReadOnly);
  if (folderPath.isEmpty()) {
    return;
    folderPath="D:/gda/ONR.VIS/data/NTF_TO_TIF_SS";
  }

  QFileInfo info(folderPath);
  workfolder_=info.absoluteDir().path();
  QDir workdir(workfolder_);
  MSI_path = workdir.filePath("MSI_Folder");
  PAN_path = workdir.filePath("PAN_folder_N");
  Result_path = workdir.filePath("Result_N");
  kml_path = workdir.filePath("region.kml");
  runfile_path = workdir.filePath("run.xml");
  pairlist_path = workdir.filePath("Priority_pair_list_tif_Selected.txt");

  qDebug()<<"Workfolder"<< info.absoluteDir().path();
  qDebug()<<MSI_path;
  qDebug()<<PAN_path;
  qDebug()<<Result_path;
  qDebug()<<kml_path;
  qDebug()<<runfile_path;
  qDebug()<<pairlist_path;
  qDebug()<<"#1";
  ui->imgListWidget->clear();
  qDebug()<<"#2";
  QDir dir(folderPath);
  QFileInfoList tiffList = dir.entryInfoList(QStringList()<<"*.tif" << "*.Tif" << "*.TIF");
  for (int i=0; i<tiffList.size(); ++i) {
    QListWidgetItem *item = new QListWidgetItem(tiffList[i].baseName());
    item->setData(Qt::UserRole,tiffList[i].absoluteFilePath());
    ui->imgListWidget->addItem(item);
  }
  qDebug()<<"#3";
  regionSet=false;
}

void IVMainWindow::on_proc_readyRead() {
  bool toBottom;
  if (ui->outputBox->verticalScrollBar()->value()==ui->outputBox->verticalScrollBar()->maximum()) {
    toBottom=true;
  } else {
    toBottom=false;
  }
  QString str = QString::fromLocal8Bit(proc_->readAll());
  ui->outputBox->setPlainText(ui->outputBox->toPlainText()+str);
  if (toBottom) {
    ui->outputBox->verticalScrollBar()->setValue(ui->outputBox->verticalScrollBar()->maximum());
  }
}

void IVMainWindow::on_actionForceStop_triggered()
{
    proc_->kill();
}

void IVMainWindow::on_actionResetRegion_triggered()
{
    handler_1->setPos(gdalItem_->boundingRect().topLeft());
    handler_2->setPos(gdalItem_->boundingRect().bottomRight());
    rectItem_->setRect(gdalItem_->boundingRect());
}

void IVMainWindow::onProcStateChanged(QProcess::ProcessState state)
{
    enum ProcessState {
        NotRunning,
        Starting,
        Running
    };
    switch(state)
    {
    case QProcess::NotRunning:
        stateLabel_->setText("Idle");
        break;
    case QProcess::Starting:
        stateLabel_->setText("Starting");
        break;
    case QProcess::Running:
        stateLabel_->setText("Running");
        break;
    }
}
