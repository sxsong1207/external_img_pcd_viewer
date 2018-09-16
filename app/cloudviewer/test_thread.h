#ifndef TEST_THREAD_H
#define TEST_THREAD_H

#include <QThread>
#include <QTimer>
#include "point_types.h"

class Test_Thread : public QThread {
  Q_OBJECT
 public:
  Test_Thread();

 private slots:
  void load_test_data();
  void incremTimerSlot();

 signals:
  void sendPointCloud(std::vector<viz::PointXYZRGB> data);
  void sendHomeCmd();
  void sendClearCmd();
 private:
  std::vector<viz::PointXYZRGB> pool_;

  QTimer incrementalTimer_;
  void batchTest();
  void incrementalTest();


  // QThread interface
 protected:
  void run();
};

#endif // TEST_THREAD_H
