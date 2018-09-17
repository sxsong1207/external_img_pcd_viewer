#include "imageviewerdaemondlg.h"
#include "ui_imageviewerdaemondlg.h"
#include <QFileInfo>
#include <QDataStream>
ImageViewerDaemonDlg::ImageViewerDaemonDlg(QWidget *parent) :
  QMainWindow(parent),
  scene_(new QGraphicsScene),
  gdalItem_(new QGraphicsGDALItem),
  ui(new Ui::MainWindow) {
  ui->setupUi(this);
  ui->graphicsView->setScene(scene_);
  scene_->addItem(gdalItem_);
  localPort = 53424;
  udp.bind(QHostAddress::Any,localPort);
  connect(&udp,&QUdpSocket::readyRead,this,&ImageViewerDaemonDlg::on_udp_readyRead);
}


ImageViewerDaemonDlg::~ImageViewerDaemonDlg() {
  udp.close();
  delete ui;
}

void ImageViewerDaemonDlg::on_udp_readyRead() {
  static const quint16 IMGPAHT_HEADER=0xAE;
  static const quint16 CLEAR_HEADER=0xCF;
  static const quint16 TEXT_HEADER=0x4E;
  while (udp.hasPendingDatagrams()) {
    QByteArray buffer;
    buffer.resize(udp.pendingDatagramSize());
    QHostAddress sender;
    quint16 senderPort;
    udp.readDatagram(buffer.data(),buffer.size(),&sender,&senderPort);
    QDataStream out(&buffer,QIODevice::ReadWrite);
    quint16 header;
    out>>header;
    if (header == CLEAR_HEADER) {
      clearView();
    } else if (header == IMGPAHT_HEADER) {
      QString path;
      out>>path;
      showImg(path);
    } else if (header == TEXT_HEADER) {
      QString text;
      out>>text;
      showText(text);
    }
  }
}

void ImageViewerDaemonDlg::clearView() {
  scene_->removeItem(gdalItem_);
}

void ImageViewerDaemonDlg::showImg(QString path) {
  QFileInfo info(path);
  if (!info.exists()) {
    qDebug()<<"path not exist";
  }
  if (!scene_->children().contains((QObject *)gdalItem_)) {
    scene_->addItem(gdalItem_);
  }
  gdalItem_->load_image(path);
  scene_->setSceneRect(gdalItem_->boundingRect());
  ui->graphicsView->fitInView(scene_->sceneRect(),Qt::KeepAspectRatio);
}

void ImageViewerDaemonDlg::showText(QString text) {
  ui->label->setText(text);
}
