#ifndef QTOSGWIDGET_H
#define QTOSGWIDGET_H
#include <QOpenGLWidget>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDesktopWidget>
#include <QScreen>
#include <QWindow>

#include <osg/ref_ptr>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Camera>
#include <osg/ShapeDrawable>
#include <osg/Material>
#include <osgGA/EventQueue>
#include <osgGA/TrackballManipulator>

#include <iostream>

class QtOSGWidget : public QOpenGLWidget {
  Q_OBJECT
 public:
  QtOSGWidget(QWidget *parent = 0);


  virtual ~QtOSGWidget() {}
  osg::Geode *root;

 public slots:
  void home() {
    _mViewer->home();
  }
  void setText(QString text) {
      textOne->setText(text.toStdString());
  }
 protected:

  virtual void paintGL();

  virtual void resizeGL(int width, int height);

  virtual void initializeGL();

  virtual void mouseMoveEvent(QMouseEvent *event);

  virtual void mousePressEvent(QMouseEvent *event);

  virtual void mouseReleaseEvent(QMouseEvent *event);

  virtual void wheelEvent(QWheelEvent *event);

  virtual bool event(QEvent *event);

  virtual void mouseDoubleClickEvent(QMouseEvent *event);

  virtual void keyPressEvent(QKeyEvent *event);

  virtual void keyReleaseEvent(QKeyEvent *event);

  void setKeyboardModifiers(QInputEvent *event);
 private:

  osgGA::EventQueue *getEventQueue() const;

  osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> _mGraphicsWindow;
  osg::ref_ptr<osgViewer::Viewer> _mViewer;
  osg::ref_ptr<osgViewer::StatsHandler> _mStatsHandler;
  qreal m_scale;

  osg::Projection* HUDProjectionMatrix;
  osgText::Text* textOne;

};
#endif // QTOSGWIDGET_H
