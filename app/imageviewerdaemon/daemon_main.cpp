#include <QApplication>
#include <imageviewerdaemondlg.h>
#include <gdal_priv.h>

#include <QCommandLineParser>
#include <QCommandLineOption>

void setWindowSize(QWidget *win) {

  QCommandLineParser parser;
  QCommandLineOption op_x("x")
  ,op_y("y")
  ,op_w("w")
  ,op_h("h");
  op_x.setValueName("x");
  op_y.setValueName("y");
  op_w.setValueName("w");
  op_h.setValueName("h");
  parser.addOption(op_x);
  parser.addOption(op_y);
  parser.addOption(op_w);
  parser.addOption(op_h);
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

  GDALAllRegister();
  QApplication app(argc,argv);

  ImageViewerDaemonDlg win;
  setWindowSize(&win);
  win.show();
  return app.exec();
}
