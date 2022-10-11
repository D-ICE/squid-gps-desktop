#ifndef SQUID_GPS_DESKTOP_BACKEND_H
#define SQUID_GPS_DESKTOP_BACKEND_H

#include <QObject>
#include <QString>
#include <qqml.h>

#include "sgps/nmea_listener.h"
#include "sgps/squid_gps_server.h"
#include "sgps/controller.h"

class BackEnd : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString status READ status NOTIFY status_changed)
  QML_ELEMENT

 public:
  enum State {
    DISCONNECTED,
    CONNECTING,
    CONNECTED
  };

  explicit BackEnd(QObject *parent = nullptr);

  QString status() const;
  Q_INVOKABLE void UpdateState(bool checked);

 signals:
  void status_changed();

 private:
  void Connect(std::error_code& err);
  void Disconnect(std::error_code& err);

 private:
  State m_state;
  uint16_t m_nmea_port;

  std::shared_ptr<asio::io_context> m_context;
  std::shared_ptr<sgps::NMEAListener> m_listener;
  std::shared_ptr<sgps::SquidGPSServer> m_squid_server;
  std::shared_ptr<std::thread> m_context_thread;
  sgps::Model m_model;
  sgps::Controller m_controller;
};

#endif // SQUID_GPS_DESKTOP_BACKEND_H
