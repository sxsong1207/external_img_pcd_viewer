#ifndef GDALVIEW_H
#define GDALVIEW_H

#include <QGraphicsView>

class GDALView : public QGraphicsView {
  Q_OBJECT
 public:
  GDALView(QWidget *parent=0);

 protected:
  virtual void mouseMoveEvent(QMouseEvent *event);
  virtual void wheelEvent(QWheelEvent *event);
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent *event);
  virtual void mouseDoubleClickEvent(QMouseEvent *);
 signals:
  void change_scale(double scale);
  void position_on_shot(double x,double y);
  void dbclick(double x,double y);
 private:
  double x0, y0;
  double scaled;
};

#endif // GDALVIEW_H
