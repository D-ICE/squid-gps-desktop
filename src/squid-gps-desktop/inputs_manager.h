#ifndef SQUID_GPS_DESKTOP_INPUTS_MANAGER_H
#define SQUID_GPS_DESKTOP_INPUTS_MANAGER_H

#include <QQmlEngine>
#include <QObject>

#include "serial_reader.h"
#include "udp_listener.h"

class InputsManager : public QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(SerialReader* serial_reader READ serial_reader CONSTANT)
  Q_PROPERTY(UdpListener* udp_listener READ udp_listener CONSTANT)

 public:
  using QObject::QObject;

  SerialReader* serial_reader();
  UdpListener* udp_listener();

  Q_INVOKABLE void readSerial();
  Q_INVOKABLE void listenUDP();

 private:
  SerialReader m_serial_reader;
  UdpListener m_udp_listener;
};

#endif // SQUID_GPS_DESKTOP_INPUTS_MANAGER_H
