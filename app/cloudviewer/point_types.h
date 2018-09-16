#ifndef POINT_TYPES_H
#define POINT_TYPES_H
#include <vector>
#include <QObject>
#include <osg/Geometry>

namespace viz {

struct PointXYZ {
  struct {
    float x,y,z;
  };
  float data[3];
};

union PointXYZRGB {
  struct {
    float x,y,z;
    float r,g,b;
  };
  float data[6];
};

}

Q_DECLARE_METATYPE(viz::PointXYZ)
Q_DECLARE_METATYPE(viz::PointXYZRGB)
#endif // POINT_TYPES_H
