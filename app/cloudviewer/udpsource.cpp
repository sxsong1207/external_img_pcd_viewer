#include "udpsource.h"
#include <QDataStream>
#include <QDebug>

const quint16 PTS_HEADER = 0xCE;
const quint16 TEXT_HEADER = 0xE1;
const quint16 CLEAR_HEADER = 0xC1;
const quint16 HOME_HEADER = 0xCE;

UdpSource::UdpSource()
{
    totalNum=0;
    cloudPort = 1234;
    commandPort = 1235;
    minPacketSize = 500;
    ms_interval = 1;    // 1000 hz
    buffer_.reserve(10*minPacketSize);
}

UdpSource::~UdpSource()
{
    cloudUdp_->close();
    commandUdp_->close();
}

void UdpSource::setupSocket()
{
    resetGlobalShift = true;
    resetHomeRequest = true;
    cloudUdp_->bind(QHostAddress::Any,cloudPort);
    commandUdp_->bind(QHostAddress::Any,commandPort);
}

void UdpSource::run()
{
    cloudUdp_ = new QUdpSocket();
    commandUdp_ = new QUdpSocket();
    setupSocket();
    while(true)
    {
        // pull cloud data
        while (cloudUdp_->hasPendingDatagrams())
        {
            QByteArray buffer;
            buffer.resize(cloudUdp_->pendingDatagramSize());
            QHostAddress sender;
            quint16 senderPort;
            cloudUdp_->readDatagram(buffer.data(), buffer.size(),
                                   &sender, &senderPort);
            QDataStream out(&buffer, QIODevice::ReadWrite);
            quint16 header;
            out>>header;
            if(header != PTS_HEADER) continue;
            quint32 num;
            out>>num;
            double x,y,z;
            quint8 r,g,b;
            for(int i=0;i<num;++i) {
                out >> x >> y >> z >> r >> g >> b;
                if(resetGlobalShift)
                {
                    global_shift_x_ = x;
                    global_shift_y_ = y;
                    global_shift_z_ = z;
                    resetGlobalShift=false;
                }
                viz::PointXYZRGB pt;
                pt.x = x-global_shift_x_;
                pt.y = y-global_shift_y_;
                pt.z = z-global_shift_z_;
                pt.r = (float)r/255.0;
                pt.g = (float)g/255.0;
                pt.b = (float)b/255.0;
                buffer_.push_back(pt);
            }
        }

        // pull cmd data
        while (commandUdp_->hasPendingDatagrams())
        {
            QByteArray buffer;
            buffer.resize(commandUdp_->pendingDatagramSize());
            QHostAddress sender;
            quint16 senderPort;
            commandUdp_->readDatagram(buffer.data(), buffer.size(),
                                   &sender, &senderPort);
//            qDebug()<< "udp recv" << sender.toString() <<"Port "<< senderPort<< " Size "<<cloudUdp_->pendingDatagramSize();
            QDataStream out(&buffer, QIODevice::ReadWrite);
            quint16 header;
            out>>header;

            if(header== CLEAR_HEADER)
            {
                emit sendClearCmd();
                buffer_.clear();
                resetGlobalShift=true;
            }
            else if(header == HOME_HEADER)
            {
                resetHomeRequest=true;
            }else if(header == TEXT_HEADER)
            {
                QString text;
                out>>text;
                emit sendTextCmd(text);
                qDebug()<<text<<" vs "<<totalNum;
            }

        }

        if(buffer_.size()>minPacketSize)
        {
            QMutexLocker locker(&bufferMutex_);
            std::vector<viz::PointXYZRGB> payload;
            payload.reserve(buffer_.size());
            for(std::vector<viz::PointXYZRGB>::iterator it=buffer_.begin();it!=buffer_.end();++it)
            {
                payload.push_back(*it);
            }
            buffer_.clear();
            emit sendPointCloud(payload);
            totalNum+=payload.size();
            if(resetHomeRequest)
            {
                emit sendHomeCmd();
                resetHomeRequest=false;
            }
            locker.unlock();
        }

        this->msleep(ms_interval);
    }
}
