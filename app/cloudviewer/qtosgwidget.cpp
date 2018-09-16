#include "qtosgwidget.h"
#include <QApplication>
#include <osgViewer/Viewer>
#include <osgText/Font>
#include <osg/MatrixTransform>
#include <osgViewer/ViewerEventHandlers>
#include <QDebug>

class QtKeyboardMap {

public:
    QtKeyboardMap() {
        mKeyMap[Qt::Key_Escape     ] = osgGA::GUIEventAdapter::KEY_Escape;
        mKeyMap[Qt::Key_Delete   ] = osgGA::GUIEventAdapter::KEY_Delete;
        mKeyMap[Qt::Key_Home       ] = osgGA::GUIEventAdapter::KEY_Home;
        mKeyMap[Qt::Key_Enter      ] = osgGA::GUIEventAdapter::KEY_KP_Enter;
        mKeyMap[Qt::Key_End        ] = osgGA::GUIEventAdapter::KEY_End;
        mKeyMap[Qt::Key_Return     ] = osgGA::GUIEventAdapter::KEY_Return;
        mKeyMap[Qt::Key_PageUp     ] = osgGA::GUIEventAdapter::KEY_Page_Up;
        mKeyMap[Qt::Key_PageDown   ] = osgGA::GUIEventAdapter::KEY_Page_Down;
        mKeyMap[Qt::Key_Left       ] = osgGA::GUIEventAdapter::KEY_Left;
        mKeyMap[Qt::Key_Right      ] = osgGA::GUIEventAdapter::KEY_Right;
        mKeyMap[Qt::Key_Up         ] = osgGA::GUIEventAdapter::KEY_Up;
        mKeyMap[Qt::Key_Down       ] = osgGA::GUIEventAdapter::KEY_Down;
        mKeyMap[Qt::Key_Backspace  ] = osgGA::GUIEventAdapter::KEY_BackSpace;
        mKeyMap[Qt::Key_Tab        ] = osgGA::GUIEventAdapter::KEY_Tab;
        mKeyMap[Qt::Key_Space      ] = osgGA::GUIEventAdapter::KEY_Space;
        mKeyMap[Qt::Key_Delete     ] = osgGA::GUIEventAdapter::KEY_Delete;
        mKeyMap[Qt::Key_Alt      ] = osgGA::GUIEventAdapter::KEY_Alt_L;
        mKeyMap[Qt::Key_Shift    ] = osgGA::GUIEventAdapter::KEY_Shift_L;
        mKeyMap[Qt::Key_Control  ] = osgGA::GUIEventAdapter::KEY_Control_L;
        mKeyMap[Qt::Key_Meta     ] = osgGA::GUIEventAdapter::KEY_Meta_L;

        mKeyMap[Qt::Key_F1             ] = osgGA::GUIEventAdapter::KEY_F1;
        mKeyMap[Qt::Key_F2             ] = osgGA::GUIEventAdapter::KEY_F2;
        mKeyMap[Qt::Key_F3             ] = osgGA::GUIEventAdapter::KEY_F3;
        mKeyMap[Qt::Key_F4             ] = osgGA::GUIEventAdapter::KEY_F4;
        mKeyMap[Qt::Key_F5             ] = osgGA::GUIEventAdapter::KEY_F5;
        mKeyMap[Qt::Key_F6             ] = osgGA::GUIEventAdapter::KEY_F6;
        mKeyMap[Qt::Key_F7             ] = osgGA::GUIEventAdapter::KEY_F7;
        mKeyMap[Qt::Key_F8             ] = osgGA::GUIEventAdapter::KEY_F8;
        mKeyMap[Qt::Key_F9             ] = osgGA::GUIEventAdapter::KEY_F9;
        mKeyMap[Qt::Key_F10            ] = osgGA::GUIEventAdapter::KEY_F10;
        mKeyMap[Qt::Key_F11            ] = osgGA::GUIEventAdapter::KEY_F11;
        mKeyMap[Qt::Key_F12            ] = osgGA::GUIEventAdapter::KEY_F12;
        mKeyMap[Qt::Key_F13            ] = osgGA::GUIEventAdapter::KEY_F13;
        mKeyMap[Qt::Key_F14            ] = osgGA::GUIEventAdapter::KEY_F14;
        mKeyMap[Qt::Key_F15            ] = osgGA::GUIEventAdapter::KEY_F15;
        mKeyMap[Qt::Key_F16            ] = osgGA::GUIEventAdapter::KEY_F16;
        mKeyMap[Qt::Key_F17            ] = osgGA::GUIEventAdapter::KEY_F17;
        mKeyMap[Qt::Key_F18            ] = osgGA::GUIEventAdapter::KEY_F18;
        mKeyMap[Qt::Key_F19            ] = osgGA::GUIEventAdapter::KEY_F19;
        mKeyMap[Qt::Key_F20            ] = osgGA::GUIEventAdapter::KEY_F20;

        mKeyMap[Qt::Key_hyphen         ] = '-';
        mKeyMap[Qt::Key_Equal         ] = '=';

        mKeyMap[Qt::Key_division      ] = osgGA::GUIEventAdapter::KEY_KP_Divide;
        mKeyMap[Qt::Key_multiply      ] = osgGA::GUIEventAdapter::KEY_KP_Multiply;
        mKeyMap[Qt::Key_Minus         ] = '-';
        mKeyMap[Qt::Key_Plus          ] = '+';
        //mKeyMap[Qt::Key_H              ] = osgGA::GUIEventAdapter::KEY_KP_Home;
        //mKeyMap[Qt::Key_                    ] = osgGA::GUIEventAdapter::KEY_KP_Up;
        //mKeyMap[92                    ] = osgGA::GUIEventAdapter::KEY_KP_Page_Up;
        //mKeyMap[86                    ] = osgGA::GUIEventAdapter::KEY_KP_Left;
        //mKeyMap[87                    ] = osgGA::GUIEventAdapter::KEY_KP_Begin;
        //mKeyMap[88                    ] = osgGA::GUIEventAdapter::KEY_KP_Right;
        //mKeyMap[83                    ] = osgGA::GUIEventAdapter::KEY_KP_End;
        //mKeyMap[84                    ] = osgGA::GUIEventAdapter::KEY_KP_Down;
        //mKeyMap[85                    ] = osgGA::GUIEventAdapter::KEY_KP_Page_Down;
        mKeyMap[Qt::Key_Insert        ] = osgGA::GUIEventAdapter::KEY_KP_Insert;
        //mKeyMap[Qt::Key_Delete        ] = osgGA::GUIEventAdapter::KEY_KP_Delete;
    }

    ~QtKeyboardMap() {
    }

    int remapKey(QKeyEvent *event) {
        KeyMap::iterator itr = mKeyMap.find(event->key());
        if (itr == mKeyMap.end()) {
            return int(*(event->text().toLatin1().data()));
        } else {
            return itr->second;
        }
    }

private:
    typedef std::map<unsigned int, int> KeyMap;
    KeyMap mKeyMap;
};

static QtKeyboardMap s_QtKeyboardMap;


QtOSGWidget::QtOSGWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , _mGraphicsWindow(new osgViewer::GraphicsWindowEmbedded(this->x(), this->y(),
                                                             this->width(), this->height()))
    , _mViewer(new osgViewer::Viewer)
    , _mStatsHandler(new osgViewer::StatsHandler)
    , m_scale(QApplication::desktop()->devicePixelRatio()) {

    root = new osg::Geode;

    osg::Camera *camera = new osg::Camera;
    camera->setViewport(0, 0, this->width(), this->height());
    camera->setClearColor(osg::Vec4(0.9f, 0.9f, 1.f, 1.f));
    float aspectRatio = static_cast<float>(this->width()) / static_cast<float>(this->height());
    camera->setProjectionMatrixAsPerspective(30.f, aspectRatio, 1.f, 1000.f);
    camera->setGraphicsContext(_mGraphicsWindow);

    _mViewer->setCamera(camera);
    _mViewer->setSceneData(root);
    _mViewer->addEventHandler(_mStatsHandler);
    osgGA::TrackballManipulator *manipulator = new osgGA::TrackballManipulator;
    manipulator->setAllowThrow(false);
    this->setMouseTracking(true);
    _mViewer->setCameraManipulator(manipulator);
    _mViewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);
    _mViewer->realize();

    // HUD

    // A geometry node for our HUD:
    osg::Geode* HUDGeode = new osg::Geode();
    // Text instance that wil show up in the HUD:
    textOne = new osgText::Text();
    // Projection node for defining view frustrum for HUD:
    HUDProjectionMatrix = new osg::Projection;
    HUDProjectionMatrix->setMatrix(osg::Matrix::ortho2D(0,1024,0,768));
    osg::MatrixTransform* HUDModelViewMatrix = new osg::MatrixTransform;
    HUDModelViewMatrix->setMatrix(osg::Matrix::identity());
    // Make sure the model view matrix is not affected by any transforms
    // above it in the scene graph:
    HUDModelViewMatrix->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    root->addChild(HUDProjectionMatrix);
    HUDProjectionMatrix->addChild(HUDModelViewMatrix);
    HUDModelViewMatrix->addChild( HUDGeode );
    HUDGeode->addDrawable( textOne );
    textOne->setCharacterSize(20);
    textOne->setFont("verdana.ttf");
    textOne->setFontResolution(2,2);
    textOne->setText("");
    textOne->setFont();
    textOne->setAxisAlignment(osgText::Text::SCREEN);
    textOne->setPosition( osg::Vec3(700,0,-1.5) );
    textOne->setColor( osg::Vec4(0, 0, 0, 1) );
}

void QtOSGWidget::paintGL() {
    _mViewer->frame();
}

void QtOSGWidget::resizeGL(int width, int height) {
    this->getEventQueue()->windowResize(this->x()*m_scale, this->y() * m_scale, width*m_scale, height*m_scale);
    _mGraphicsWindow->resized(this->x()*m_scale, this->y() * m_scale, width*m_scale, height*m_scale);
    osg::Camera *camera = _mViewer->getCamera();
    camera->setViewport(0, 0, this->width()*m_scale, this->height()* m_scale);
    HUDProjectionMatrix->setMatrix(osg::Matrix::ortho2D(0, this->width()*m_scale, 0, this->height()* m_scale));
    textOne->setPosition( osg::Vec3(this->width()*m_scale*0.3, this->height()* m_scale *0.95,0) );
}

void QtOSGWidget::initializeGL() {
    osg::Geode *geode = dynamic_cast<osg::Geode *>(_mViewer->getSceneData());
    osg::StateSet *stateSet = geode->getOrCreateStateSet();
    osg::Material *material = new osg::Material;
    material->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);
    stateSet->setAttributeAndModes(material, osg::StateAttribute::ON);
    stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
}

void QtOSGWidget::mouseMoveEvent(QMouseEvent *event) {
    this->getEventQueue()->mouseMotion(event->x()*m_scale, event->y()*m_scale);
}

void QtOSGWidget::mousePressEvent(QMouseEvent *event) {
    unsigned int button = 0;
    switch (event->button()) {
    case Qt::LeftButton:
        button = 1;
        break;
    case Qt::MiddleButton:
        button = 2;
        break;
    case Qt::RightButton:
        button = 3;
        break;
    default:
        break;
    }
    this->getEventQueue()->mouseButtonPress(event->x()*m_scale, event->y()*m_scale, button);
}

void QtOSGWidget::mouseReleaseEvent(QMouseEvent *event) {
    unsigned int button = 0;
    switch (event->button()) {
    case Qt::LeftButton:
        button = 1;
        break;
    case Qt::MiddleButton:
        button = 2;
        break;
    case Qt::RightButton:
        button = 3;
        break;
    default:
        break;
    }
    this->getEventQueue()->mouseButtonRelease(event->x()*m_scale, event->y()*m_scale, button);
}

void QtOSGWidget::wheelEvent(QWheelEvent *event) {
    int delta = event->delta();
    osgGA::GUIEventAdapter::ScrollingMotion motion = delta > 0 ?
                osgGA::GUIEventAdapter::SCROLL_UP : osgGA::GUIEventAdapter::SCROLL_DOWN;
    this->getEventQueue()->mouseScroll(motion);
}

bool QtOSGWidget::event(QEvent *event) {
    bool handled = QOpenGLWidget::event(event);
    this->update();
    return handled;
}

void QtOSGWidget::mouseDoubleClickEvent(QMouseEvent *event) {
    unsigned int button = 0;
    switch (event->button()) {
    case Qt::LeftButton:
        button = 1;
        break;
    case Qt::MiddleButton:
        button = 2;
        break;
    case Qt::RightButton:
        button = 3;
        break;
    default:
        break;
    }
    this->getEventQueue()->mouseDoubleButtonPress(event->x()*m_scale, event->y()*m_scale,button);
}

void QtOSGWidget::keyPressEvent(QKeyEvent *event) {
    setKeyboardModifiers(event);
    int value = s_QtKeyboardMap.remapKey(event);
    this->getEventQueue()->keyPress(value);
    qDebug()<<"keyPress";
}

void QtOSGWidget::keyReleaseEvent(QKeyEvent *event) {
    setKeyboardModifiers(event);
    int value = s_QtKeyboardMap.remapKey(event);
    this->getEventQueue()->keyRelease(value);
}

void QtOSGWidget::setKeyboardModifiers(QInputEvent *event) {
    int modkey = event->modifiers() & (Qt::ShiftModifier | Qt::ControlModifier | Qt::AltModifier);
    unsigned int mask = 0;
    if (modkey & Qt::ShiftModifier) {
        mask |= osgGA::GUIEventAdapter::MODKEY_SHIFT;
    }
    if (modkey & Qt::ControlModifier) {
        mask |= osgGA::GUIEventAdapter::MODKEY_CTRL;
    }
    if (modkey & Qt::AltModifier) {
        mask |= osgGA::GUIEventAdapter::MODKEY_ALT;
    }
    this->getEventQueue()->getCurrentEventState()->setModKeyMask(mask);
}

osgGA::EventQueue *QtOSGWidget::getEventQueue() const {
    osgGA::EventQueue *eventQueue = _mGraphicsWindow->getEventQueue();
    return eventQueue;
}
