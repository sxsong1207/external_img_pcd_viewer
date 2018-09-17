#ifndef IVMAINWINDOW_H
#define IVMAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QProcess>
#include "mygraphicshandleritem.h"

class QGraphicsGDALItem;
namespace Ui {
class IVMainWindow;
}

class IVMainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit IVMainWindow(QWidget *parent = 0);
  ~IVMainWindow();

 private slots:
  void mouseMoveInView(double x, double y);
  void handlerMove();

  void on_actionFitView_triggered();

  void on_actionExportKML_triggered();
  void outputXML(QString path);
  void outputKML(QString path);
  void on_actionRun_triggered();

  void on_imgListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

  void on_actionOpenFolder_triggered();

  void on_proc_readyRead();
  void on_actionForceStop_triggered();

  void on_actionResetRegion_triggered();

  void onProcStateChanged(QProcess::ProcessState state);
private:
  Ui::IVMainWindow *ui;
  QGraphicsScene *scene_;
  QGraphicsRectItem *rectItem_;
  QGraphicsGDALItem *gdalItem_;
  MyGraphicsHandlerItem *handler_1;
  MyGraphicsHandlerItem *handler_2;
  QLabel  *stateLabel_;
  QProcess *proc_;

  QString workfolder_;
  QString mvs3dprogram_path;
  QString MSI_path;
  QString PAN_path;
  QString Result_path;
  QString kml_path;
  QString runfile_path;
  QString pairlist_path;
  bool regionSet;
  double minLat,minLon,maxLat,maxLon;
};

#endif // IVMAINWINDOW_H
