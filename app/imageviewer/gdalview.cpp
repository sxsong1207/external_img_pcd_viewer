#include "gdalview.h"
#include <QtMath>
#include <QWheelEvent>
#include <QDebug>

GDALView::GDALView(QWidget *parent): QGraphicsView(parent) {
  setDragMode(QGraphicsView::ScrollHandDrag);
  setMouseTracking(true);
  this->setCursor(QCursor(Qt::CrossCursor));
}

void GDALView::mouseMoveEvent(QMouseEvent *event) {
  double x, y;
  x = event->pos().x();
  y = event->pos().y();
  emit position_on_shot(x, y);
  QGraphicsView::mouseMoveEvent(event);
}

void GDALView::wheelEvent(QWheelEvent *event) {
  QPoint pos;
  double x, y;
  double r, c;

  int delta;

  delta = event->delta();

  pos = event->pos();
  x = pos.x();
  y = pos.y();

  c = (x - x0) / scaled;
  r = (y0 - y) / scaled;

  double numDegrees = event->delta() / 8.0;
  double numSteps = numDegrees / 15.0;
  scaled = std::pow(1.125, numSteps);
  this->scale(scaled, scaled);
  x0 = x - scaled * c;
  y0 = y + scaled * r;
  //  QGraphicsView::wheelEvent(event);
}

void GDALView::mousePressEvent(QMouseEvent *event)
{
  QGraphicsView::mousePressEvent(event);
  this->setCursor(QCursor(Qt::CrossCursor));
}

void GDALView::mouseReleaseEvent(QMouseEvent *event)
{
  QGraphicsView::mouseReleaseEvent(event);
  this->setCursor(QCursor(Qt::CrossCursor));
}

void GDALView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QGraphicsView::mouseDoubleClickEvent(event);
    this->setCursor(QCursor(Qt::CrossCursor));
    emit dbclick(event->pos().x(),event->pos().y());
}
