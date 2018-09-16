#include "cvmainwindow.h"
#include "ui_cvmainwindow.h"
#include <liblas/liblas.hpp>
#include <QTimer>
#include <QDebug>

#include <osg/Point>
#include <osg/Array>
#include <osg/Vec4>
#include <osg/MatrixTransform>

#include <fstream>  // std::ifstream
#include <iostream> // std::cout

CVMainWindow::CVMainWindow(QWidget *parent) :
    QMainWindow(parent)
  , block_cloud_geode_(new osg::Geode)
  , sparse_cloud_geode_(new osg::Geode)
  , ui(new Ui::CVMainWindow) {
    ui->setupUi(this);

    int pointsize=2;
    int index = qApp->arguments().indexOf("-s");
    if(index>0< qApp->arguments().size()-1)
    {
        bool ok;
        int val = qApp->arguments()[index+1].toInt(&ok);
        if(ok) pointsize=val;
    }
    stateset_ = new osg::StateSet;
    stateset_->setMode(GL_LIGHTING, osg::StateAttribute::OFF|osg::StateAttribute::OVERRIDE);
    osg::Point* p = new osg::Point();
    p->setSize(pointsize);
    stateset_->setAttribute(p);

    ui->openGLWidget->root->addChild(block_cloud_geode_);
    ui->openGLWidget->root->addChild(sparse_cloud_geode_);
}

CVMainWindow::~CVMainWindow() {
    delete ui;
}

void CVMainWindow::AddPointCloudRGB(std::vector<viz::PointXYZRGB> data) {
    osg::ref_ptr<osg::Geometry> geom(new osg::Geometry);
    osg::Vec3Array *pts = new osg::Vec3Array;
    pts->reserveArray(data.size());
    osg::Vec4Array *colors = new osg::Vec4Array;
    colors->reserve(data.size());
    for (std::vector<viz::PointXYZRGB>::iterator it=data.begin(); it!=data.end(); ++it) {
        pts->push_back(osg::Vec3(it->x,it->y,it->z));
        colors->push_back(osg::Vec4(it->r,it->g,it->b,1));
    }
    geom->setVertexArray(pts);
    geom->addPrimitiveSet(new osg::DrawArrays(GL_POINTS,0,pts->size()));
    geom->setColorArray(colors);
    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
    geom->setStateSet(stateset_);


    sparse_cloud_geode_->addDrawable(geom);
}

void CVMainWindow::ZoomHome() {
    ui->openGLWidget->home();
}

void CVMainWindow::ShowText(QString text)
{
    ui->openGLWidget->setText(text);
}

void CVMainWindow::ClearPointCloud() {
    sparse_cloud_geode_->removeDrawables(0,sparse_cloud_geode_->getNumDrawables());
    block_cloud_geode_->removeDrawables(0,block_cloud_geode_->getNumDrawables());
}
