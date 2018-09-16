#ifndef UDPSOURCE_H
#define UDPSOURCE_H

#include <QThread>
#include <QTime>
#include <QtNetwork/QUdpSocket>
#include <point_types.h>

class UdpSource: public QThread
{
    Q_OBJECT
public:
    UdpSource();
    void setupSocket();
    quint16 cloudPort;
    quint16 commandPort;
    quint32 minPacketSize;  // how many point to packet it
    quint32 ms_interval;    // control refresh rate
signals:
 void sendPointCloud(std::vector<viz::PointXYZRGB> data);
 void sendHomeCmd();
 void sendClearCmd();
 void sendTextCmd(QString);
private:
    QUdpSocket* cloudUdp_;
    QUdpSocket* commandUdp_;
    std::vector<viz::PointXYZRGB> buffer_;
    QMutex bufferMutex_;
    bool resetGlobalShift;
    bool resetHomeRequest;
    double global_shift_x_;
    double global_shift_y_;
    double global_shift_z_;

    quint64 totalNum;
    // QThread interface
protected:
    void run();
};

#endif // UDPSOURCE_H
