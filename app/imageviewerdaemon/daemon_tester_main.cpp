#include <QCoreApplication>
#include <QThread>
#include <QUdpSocket>
#include <QDataStream>
#include "image_data_sender.h"

image_sender sender;
int main(int argc, char **argv) {
    QCoreApplication app(argc,argv);
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
            sender.Clean_screen();
            sender.Send_text("CLEAR");
        } else {
            sender.Send_image(imageList[idx].toStdString());
            sender.Send_text(imageList[idx].toStdString());
            idx=++idx%imageList.size();
        }
    }
    return 0;
}
