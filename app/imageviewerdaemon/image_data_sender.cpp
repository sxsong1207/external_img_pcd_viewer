#include "image_data_sender.h"
#include <iostream>
#ifdef DEMO_RUN
const quint16 IV_IMGPAHT_HEADER=0xAE;
const quint16 IV_CLEAR_HEADER=0xCF;
const quint16 IV_TEXT_HEADER=0x4E;

image_sender::image_sender()
{
    localPort=6548;
    remotePort=53424;
}
image_sender::~image_sender(){
    socket.close();
}

bool image_sender::initialize(){
    socket.bind(QHostAddress::Any,localPort);
    return true;
}

bool image_sender::Clean_screen()
{
	QByteArray buffer;
    QDataStream out(&buffer,QIODevice::ReadWrite);
    out<<IV_CLEAR_HEADER;
    socket.writeDatagram(buffer,QHostAddress::LocalHost,remotePort);
    return true;
}
bool image_sender::Send_image(std::string path)
{
	QByteArray buffer;
    QDataStream out(&buffer,QIODevice::ReadWrite);
    out<<IV_IMGPAHT_HEADER<<QString(path.c_str());
    socket.writeDatagram(buffer,QHostAddress::LocalHost,remotePort);
    return true;
}
bool image_sender::Send_text(std::string textmsg)
{
	QByteArray buffer;
    QDataStream out(&buffer,QIODevice::ReadWrite);
    out<<IV_TEXT_HEADER<<QString(textmsg.c_str());
    socket.writeDatagram(buffer,QHostAddress::LocalHost,remotePort);
    return true;
}
  
#endif
