#include "mygraphicshandleritem.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDebug>

MyGraphicsHandlerItem::MyGraphicsHandlerItem() {
  setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemSendsGeometryChanges);
  setCursor(Qt::CrossCursor);
  pen_.setColor(QColor(Qt::black));
  pen_.setWidth(2);
}

QVariant MyGraphicsHandlerItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) {
  if (change == QGraphicsItem::ItemPositionChange) {
    emit posChanged(value.toPointF());
  }
  return QGraphicsItem::itemChange(change, value);
}

QRectF MyGraphicsHandlerItem::boundingRect() const {
  return rect_;
}

void MyGraphicsHandlerItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
  painter->setPen(pen_);
  painter->setBrush(brush_);
  painter->drawLine(rect_.left(),0,rect_.right(),0);
  painter->drawLine(0,rect_.top(),0,rect_.bottom());
}
