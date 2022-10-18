#ifndef SQUID_GPS_DESKTOP_BACKEND_H
#define SQUID_GPS_DESKTOP_BACKEND_H

#include <list>

#include <QObject>
#include <QString>
#include <QSettings>
#include <qqml.h>

#include "sgps/nmea_listener.h"
#include "sgps/squid_gps_server.h"
#include "sgps/controller.h"

class BackEnd : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString squid_connection_status READ squid_connection_status NOTIFY squid_connection_status_changed)
  Q_PROPERTY(QString nmea_displayed_frames READ nmea_displayed_frames NOTIFY nmea_displayed_frames_changed)
  Q_PROPERTY(uint16_t nmea_udp_port READ nmea_udp_port WRITE set_nmea_udp_port NOTIFY nmea_udp_port_changed)
  Q_PROPERTY(QString current_state READ current_state NOTIFY current_state_changed)
  QML_ELEMENT

  static const uint16_t kDefaultNMEAPort;

 public:
  enum State {
    DISCONNECTED,
    CONNECTING,
    CONNECTED
  };

  explicit BackEnd(QObject *parent = nullptr);

  QString squid_connection_status() const;
  QString nmea_displayed_frames() const;
  uint16_t nmea_udp_port() const;
  QString current_state() const;

  Q_INVOKABLE void UpdateSquidState(bool checked);
  void set_nmea_udp_port(uint16_t value);

 signals:
  void squid_connection_status_changed();
  void nmea_displayed_frames_changed();
  void nmea_udp_port_changed();
  void current_state_changed();

 private:
  void Connect(std::error_code& err);
  void Disconnect(std::error_code& err);

  void ConnectNMEA(std::error_code& err);
  void DisconnectNMEA(std::error_code& err);
  void PushNMEAFrame(std::string&& value);

  uint16_t nmea_udp_port_setting() const;
  void set_nmea_udp_port_setting(uint16_t value);

 private:
  QSettings m_settings;

  sgps::Model m_model;
  sgps::Controller m_controller;

  std::list<std::string> m_received_nmea_frames;
  std::string m_displayed_nmea_frames;
  std::error_code m_listener_err;
  std::shared_ptr<asio::io_context> m_listener_context;
  std::shared_ptr<sgps::NMEAListener> m_listener;
  std::shared_ptr<std::thread> m_listener_context_thread;

  State m_squid_connection_state;
  std::shared_ptr<asio::io_context> m_squid_context;
  std::shared_ptr<sgps::SquidGPSServer> m_squid_server;
  std::shared_ptr<std::thread> m_squid_context_thread;
  std::mutex m_squid_connection_mutex;
  std::condition_variable m_squid_connection_condition_variable;
  std::shared_ptr<std::thread> m_squid_connection_thread;
};

#endif // SQUID_GPS_DESKTOP_BACKEND_H
