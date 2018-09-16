#include "test_thread.h"

#include <iostream>
#include <vector>
#include <liblas/liblas.hpp>
#include <QDebug>

Test_Thread::Test_Thread() {
  connect(&incrementalTimer_,&QTimer::timeout,this,&Test_Thread::incremTimerSlot);
}


void Test_Thread::load_test_data() {
  std::ifstream ifs;
  ifs.open("C:/ONR_vis/data/test1_2_lines_group1_densified_point_cloud_part_2.las", std::ios::in | std::ios::binary);
  liblas::ReaderFactory f;
  liblas::Reader reader = f.CreateWithStream(ifs);
  liblas::Header const &header = reader.GetHeader();

  std::cout << "Compressed: " << (header.Compressed() == true) ? "true":"false";
  std::cout << "Signature: " << header.GetFileSignature() << '\n';
  std::cout << "Points count: " << header.GetPointRecordsCount() << '\n';
  double cx=header.GetMinX();
  double cy=header.GetMinY();
  double cz=header.GetMinZ();

  pool_.reserve(header.GetPointRecordsCount());
  while (reader.ReadNextPoint()) {
    liblas::Point const &p = reader.GetPoint();
    float x=p.GetX() - cx ;
    float y=p.GetY() - cy;
    float z=p.GetZ() - cz;
    float r=(float)p.GetColor().GetRed()/65280.0f;
    float g=(float)p.GetColor().GetGreen()/65280.0f;
    float b=(float)p.GetColor().GetBlue()/65280.0f;

    viz::PointXYZRGB pt;
    pt.x = x;
    pt.y = y;
    pt.z = z;
    pt.r = r;
    pt.g = g;
    pt.b = b;
    pool_.push_back(pt);
  }
  qDebug()<<"Read Into pool "<<pool_.size();
  qDebug()<<"back "<<pool_.back().x<<", "<<pool_.back().y<<", "<<pool_.back().z<<", "<<pool_.back().r<<", "<<pool_.back().g<<", "<<pool_.back().b;
  ifs.close();
}

void Test_Thread::incremTimerSlot() {
  static int startIdx = 0;
  const int interval = 500;

  if (startIdx >= pool_.size()) {
    incrementalTimer_.stop();
    qDebug()<< "incremental Test done.";
  }

  int endIdx = pool_.size()<startIdx+interval?pool_.size():startIdx+interval;

  std::vector<viz::PointXYZRGB> partial;
  for (int i=startIdx; i<endIdx; ++i) {
    partial.push_back(pool_[i]);
  }
  qDebug()<<"send partial points";
  emit sendPointCloud(partial);
  startIdx = endIdx;
}

void Test_Thread::batchTest() {
  emit sendPointCloud(pool_);
  emit sendHomeCmd();
  qDebug()<<"Batch Send Done";
}

void Test_Thread::incrementalTest() {
  const int interval = 500;
  bool firstSend = true;
  std::vector<viz::PointXYZRGB> partial;
  for (int i=0; i<pool_.size(); ++i) {
    partial.push_back(pool_[i]);

    if ((i+1)%interval==0) {
      qDebug()<<"send partial points";
      emit sendPointCloud(partial);
      if (firstSend) {
        emit sendHomeCmd();
        firstSend=false;
      }
      msleep(10);
      partial.clear();
    }
  }
  qDebug()<< "incremental Test done.";
}

void Test_Thread::run() {
  load_test_data();
  msleep(100);
  qDebug()<<"Load Done.";
  incrementalTest();
}
