/*
 * Copyright (c) 2018 Smart Mapping Tech Co., Ltd. All rights reserved.
 *
 * Created by Shuang Song on 4/19/18.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <QtCore/QCoreApplication>
#include <QtNetwork/QUdpSocket>
#include <QtCore/QDataStream>
#include <QtCore/QByteArray>
#include <vector>
#include <cstdlib>	
#include <QDebug>
#include <QThread>

#include <Windows.h>

int NUM_POINT_OF_PACKET;
std::string ADDR;
std::string posTopic;
std::string laserTopic;
QHostAddress hostAddr;
int CLOUD_PORT;
int CMD_PORT;

QUdpSocket *udpSocket;

const quint16 PTS_HEADER = 0xCE;
const quint16 TEXT_HEADER = 0xE1;
const quint16 CLEAR_HEADER = 0xC1;
const quint16 HOME_HEADER = 0xCE;

typedef struct pt3D
{
    double x;
    double y;
    double z;
    quint8 R;
    quint8 G;
    quint8 B;
};

void SendPacket(std::vector<pt3D> &ptvec,int port_number)
{

    if (ptvec.empty()) return;
    QByteArray buffer;

    QDataStream out(&buffer, QIODevice::ReadWrite);
    out << PTS_HEADER;
    out << (quint32)ptvec.size();
    for (int i = 0; i < ptvec.size(); ++i) {
        out << ptvec[i].x << ptvec[i].y << ptvec[i].z<<ptvec[i].R<<ptvec[i].G<<ptvec[i].B;
    }

    udpSocket->writeDatagram(buffer, hostAddr, port_number);

} // send packages

void SendClear(int port_number)
{
    QByteArray buffer;
    QDataStream out(&buffer, QIODevice::ReadWrite);
    out << CLEAR_HEADER;
    udpSocket->writeDatagram(buffer,hostAddr,port_number);
}

void SendHome(int port_number)
{
    QByteArray buffer;
    QDataStream out(&buffer, QIODevice::ReadWrite);
    out << HOME_HEADER;
    udpSocket->writeDatagram(buffer,hostAddr,port_number);
}

void SendText(QString text, int port_number)
{
    QByteArray buffer;
    QDataStream out(&buffer, QIODevice::ReadWrite);
    out << TEXT_HEADER;
    out<<text;
    udpSocket->writeDatagram(buffer,hostAddr,port_number);
}

quint64 totalNum=0;

void exit_handler(int s){
    printf("Caught signal %d\n",s);
    if(udpSocket)
    {
        udpSocket->close();
    }
    exit(1);
}

int main(int argc,char** argv)
{
    QCoreApplication app(argc, argv);

    std::vector<pt3D> package;
    int byte_limit = 1200;
    int max_npts = byte_limit/sizeof(pt3D);

    udpSocket = new QUdpSocket();
    udpSocket->bind(QHostAddress::Any, 50000);
    hostAddr = QHostAddress::LocalHost;

    CLOUD_PORT=1234;
    CMD_PORT=1235;

    // 10000 packages
    for (int loop_pack = 0; true;loop_pack++)
    {
        package.resize(0);

        for (int perpoint = 0; perpoint < max_npts; perpoint++)
        {
            pt3D curpt;
            curpt.x = 10000+((float)rand()/(float)RAND_MAX);
            curpt.y = 100000+((float)rand()/(float)RAND_MAX);
            curpt.z = ((float)rand()/(float)RAND_MAX);
            curpt.R = (char)(((float)rand()/(float)RAND_MAX)*255);
            curpt.G = (char)(((float)rand()/(float)RAND_MAX)*255);
            curpt.B = (char)(((float)rand()/(float)RAND_MAX)*255);
            package.push_back(curpt);
        }
        SendPacket(package,CLOUD_PORT);
        totalNum+=package.size();
        qApp->thread()->msleep(10);
//        if(loop_pack%3000000==0)
//        {
//            SendClear(CMD_PORT);

//        }
//        if(loop_pack%3000000==1)
//        {
//            SendHome(CMD_PORT);
//        }

        if(loop_pack%300==0)
        {
            SendText("Total Num "+QString::number(totalNum),CMD_PORT);
        }
    }// end for.

    return 0;
}
