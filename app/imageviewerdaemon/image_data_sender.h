#ifndef IMAGE_DATA_SENDER_H
#define IMAGE_DATA_SENDER_H

#define DEMO_RUN

#ifdef DEMO_RUN

#include <QtNetwork/QUdpSocket>
#include <QtCore/QDataStream>
#include <QtCore/QByteArray>
#include <vector>
#include <Windows.h>
#include "string.h"

extern const quint16 IV_IMGPAHT_HEADER;//=0xAE;
extern const quint16 IV_CLEAR_HEADER;//=0xCF;
extern const quint16 IV_TEXT_HEADER;//=0x4E;

class image_sender {
 public:
  image_sender();
  ~image_sender();
  bool initialize();
  bool Clean_screen();
  bool Send_text(std::string textmsg);
  bool Send_image(std::string path);
 private:
  int localPort;
  int remotePort;
  QUdpSocket socket;
};
#endif
#endif // IMAGE_DATA_SENDER_H
