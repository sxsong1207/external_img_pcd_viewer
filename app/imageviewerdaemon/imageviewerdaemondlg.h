#ifndef IMAGEVIEWERDAEMONDLG_H
#define IMAGEVIEWERDAEMONDLG_H

#include <QMainWindow>
#include <QUdpSocket>

#include <QGraphicsScene>
#include "qgraphicsgdalitem.h"

namespace Ui {
class MainWindow;
}

class ImageViewerDaemonDlg : public QMainWindow {
  Q_OBJECT

 public:
  explicit ImageViewerDaemonDlg(QWidget *parent = 0);
  ~ImageViewerDaemonDlg();
 private slots:
  void on_udp_readyRead();
  void clearView();
  void showImg(QString path);
  void showText(QString text);
 private:
  int localPort;
  Ui::MainWindow *ui;
  QGraphicsScene *scene_;
  QGraphicsGDALItem *gdalItem_;
  QUdpSocket udp;
};

#endif // IMAGEVIEWERDAEMONDLG_H
