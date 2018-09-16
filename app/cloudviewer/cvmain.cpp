#include <osgViewer/Viewer>
#include <QApplication>
#include <QMainWindow>
#include "qtosgwidget.h"
#include "cvmainwindow.h"

#include "test_thread.h"
#include "udpsource.h"

#include <QCommandLineParser>
#include <QCommandLineOption>

void setWindowSize(QWidget *win) {

  QCommandLineParser parser;
  QCommandLineOption op_x("x")
  ,op_y("y")
  ,op_w("w")
  ,op_h("h")
  ,op_size("s");
  op_x.setValueName("x");
  op_y.setValueName("y");
  op_w.setValueName("w");
  op_h.setValueName("h");
  op_size.setValueName("size");
  parser.addOption(op_x);
  parser.addOption(op_y);
  parser.addOption(op_w);
  parser.addOption(op_h);
  parser.addOption(op_size);
  parser.process(*qApp);

  bool ok_x,ok_y,ok_w,ok_h;
  int x=parser.value(op_x).toInt(&ok_x);
  int y=parser.value(op_y).toInt(&ok_y);
  int w=parser.value(op_w).toInt(&ok_w);
  int h=parser.value(op_h).toInt(&ok_h);

  if (ok_x&&ok_y&&ok_w&&ok_h) {

    win->move(x,y);
    win->setFixedSize(w,h);
  }
}

int main(int argc, char **argv) {

  QApplication qapp(argc, argv);

  // !!! They are important line !!
  qRegisterMetaType<std::vector<viz::PointXYZRGB> >("std:vector<viz::PointXYZRGB>");
  qRegisterMetaType<std::vector<viz::PointXYZRGB> >("std:vector<viz::PointXYZRGB>&");

  CVMainWindow w;
  setWindowSize(&w);
  w.show();

#if 0
  Test_Thread tth;
  QObject::connect(&tth,&Test_Thread::sendPointCloud,&w,&CVMainWindow::AddPointCloudRGB);
  QObject::connect(&tth,&Test_Thread::sendHomeCmd,&w,&CVMainWindow::ZoomHome);
  QObject::connect(&tth,&Test_Thread::sendClearCmd,&w,&CVMainWindow::ClearPointCloud);
  tth.start();
#else
  UdpSource udpsrc;
  QObject::connect(&udpsrc,&UdpSource::sendPointCloud,&w,&CVMainWindow::AddPointCloudRGB);
  QObject::connect(&udpsrc,&UdpSource::sendHomeCmd,&w,&CVMainWindow::ZoomHome);
  QObject::connect(&udpsrc,&UdpSource::sendClearCmd,&w,&CVMainWindow::ClearPointCloud);
  QObject::connect(&udpsrc,&UdpSource::sendTextCmd,&w,&CVMainWindow::ShowText);
  udpsrc.start();
#endif
  return qapp.exec();
}
