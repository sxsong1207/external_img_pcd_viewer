#ifndef CVMAINWINDOW_H
#define CVMAINWINDOW_H

#include <QMainWindow>
#include "point_types.h"

#include <QUdpSocket>
namespace Ui {
class CVMainWindow;
}

class CVMainWindow : public QMainWindow {
  Q_OBJECT
 public:
  explicit CVMainWindow(QWidget *parent = 0);
  ~CVMainWindow();

 public slots:
  void AddPointCloudRGB(std::vector<viz::PointXYZRGB> data);
  void ClearPointCloud();
  void ZoomHome();
  void ShowText(QString text);
 private:
  Ui::CVMainWindow *ui;
  osg::ref_ptr<osg::StateSet> stateset_;
  osg::ref_ptr<osg::Geode>  block_cloud_geode_;
  osg::ref_ptr<osg::Geode>  sparse_cloud_geode_;

  QUdpSocket socket;

};

#endif // CVMAINWINDOW_H
