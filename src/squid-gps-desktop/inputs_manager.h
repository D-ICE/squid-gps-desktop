#ifndef SQUID_GPS_DESKTOP_INPUTS_MANAGER_H
#define SQUID_GPS_DESKTOP_INPUTS_MANAGER_H

#include <QQmlEngine>
#include <QObject>
#include <QSettings>

#include "serial_reader.h"
#include "udp_listener.h"

class InputsManager : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("Has C++ arguments in constructor")

  Q_PROPERTY(SerialReader* serial_reader READ serial_reader CONSTANT)
  Q_PROPERTY(UdpListener* udp_listener READ udp_listener CONSTANT)

 public:
  InputsManager(QSettings& settings, QObject* parent = Q_NULLPTR);

  SerialReader* serial_reader();
  UdpListener* udp_listener();

  Q_INVOKABLE void readSerial();
  Q_INVOKABLE void listenUDP();

 private:
  SerialReader m_serial_reader;
  UdpListener m_udp_listener;
};

#endif // SQUID_GPS_DESKTOP_INPUTS_MANAGER_H
