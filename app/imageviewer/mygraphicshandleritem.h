#ifndef MYGRAPHICSHANDLERITEM_H
#define MYGRAPHICSHANDLERITEM_H

#include <QGraphicsItem>
#include <QObject>
#include <QPen>
#include <QBrush>

class MyGraphicsHandlerItem :public QObject, public QGraphicsItem {
  Q_OBJECT
 public:
  MyGraphicsHandlerItem();
  void setRect(float x,float y,float w,float h) {
    rect_.setRect(x,y,w,h);
  }
  void setRect(const QRectF &rect) {
    rect_ = rect;
  }
  void setBrush(const QBrush &brush) {
    brush_=brush;
  }
  QRectF rect() {
    return rect_;
  }
  void setPen(QPen pen) {
    pen_ = pen;
  }
 signals:
  void posChanged(QPointF);
 protected:
  // QGraphicItem overrides
  virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;

  // QGraphicsItem interface
 public:
  QRectF boundingRect() const;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

 private:
  QPen pen_;
  QBrush brush_;
  QRectF rect_;
};

#endif // MYGRAPHICSHANDLERITEM_H
