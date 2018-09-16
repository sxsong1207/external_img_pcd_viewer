#include <QCoreApplication>
#include <QThread>
#include <QUdpSocket>
#include <QDataStream>

const quint16 IMGPAHT_HEADER=0xAE;
const quint16 CLEAR_HEADER=0xCF;
const quint16 TEXT_HEADER=0x4E;
class ImageDaemonClient {
 public:
  ImageDaemonClient() {
    localPort=6548;
    remotePort=53424;
    socket.bind(QHostAddress::Any,localPort);
  }
  void SendClearCmd() {
    QByteArray buffer;
    QDataStream out(&buffer,QIODevice::ReadWrite);
    out<<CLEAR_HEADER;
    socket.writeDatagram(buffer,QHostAddress::LocalHost,remotePort);
  }
  void SendImagePath(QString str) {
    QByteArray buffer;
    QDataStream out(&buffer,QIODevice::ReadWrite);
    out<<IMGPAHT_HEADER<<str;
    socket.writeDatagram(buffer,QHostAddress::LocalHost,remotePort);
  }
  void SendText(QString text) {
    QByteArray buffer;
    QDataStream out(&buffer,QIODevice::ReadWrite);
    out<<TEXT_HEADER<<text;
    socket.writeDatagram(buffer,QHostAddress::LocalHost,remotePort);
  }
 private:
  int localPort;
  int remotePort;
  QUdpSocket socket;
};



int main(int argc, char **argv) {
  QCoreApplication app(argc,argv);
  ImageDaemonClient client;
  QStringList imageList;
  imageList<<"D:/gda/ONR.VIS/data/11OCT22113415-P2AS-057772981010_01_P001.TIF"
           <<"D:/gda/ONR.VIS/data/11OCT22113425-M2AS-057772981050_01_P001.TIF"
           <<"D:/gda/ONR.VIS/data/15JUL05110249-P2AS-054784425010_01_P001.TIF"
           <<"D:/gda/ONR.VIS/data/15JUL05110357-P2AS-054784425010_01_P001.TIF"
           <<"D:/gda/ONR.VIS/data/32float.tif"
           <<"D:/gda/ONR.VIS/data/2007__11_clsmap_res.tif"
           <<"D:/gda/ONR.VIS/data/BareEarth2015.tif"
           <<"D:/gda/ONR.VIS/data/int16.tif";

  int idx=0;
  while (true) {
    qApp->thread()->msleep(1000);

    if (rand()>0.9*RAND_MAX) {
      client.SendClearCmd();
      client.SendText("CLEAR");
    } else {
      client.SendImagePath(imageList[idx]);
      client.SendText(imageList[idx]);
      idx=++idx%imageList.size();
    }
  }
  return 0;
}
