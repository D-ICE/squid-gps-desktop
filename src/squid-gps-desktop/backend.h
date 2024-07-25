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
#include "inputs_manager.h"
#include "usb.h"

class BackEnd : public QObject {
  Q_OBJECT
  QML_SINGLETON
  QML_ELEMENT
  Q_PROPERTY(QString squid_connection_status READ squid_connection_status NOTIFY squid_connection_status_changed)
  Q_PROPERTY(QString nmea_displayed_frames READ nmea_displayed_frames NOTIFY nmea_displayed_frames_changed)
  Q_PROPERTY(uint16_t nmea_udp_port READ nmea_udp_port WRITE set_nmea_udp_port NOTIFY nmea_udp_port_changed)
  Q_PROPERTY(QString current_state READ current_state NOTIFY current_state_changed)
  Q_PROPERTY(bool connect_roadbook READ connect_roadbook WRITE set_connect_roadbook NOTIFY connect_roadbook_changed)
  Q_PROPERTY(bool nmea_udp_active READ nmea_udp_active WRITE set_nmea_udp_active NOTIFY nmea_udp_active_changed)
  Q_PROPERTY(InputsManager* inputs_manager READ inputs_manager CONSTANT)

  static const uint16_t kDefaultNMEAPort;

 public:
  enum State {
    DISCONNECTED,
    CONNECTING,
    CONNECTED
  };

  explicit BackEnd(QObject *parent = nullptr);
  ~BackEnd();

  QString squid_connection_status() const;
  QString nmea_displayed_frames() const;
  uint16_t nmea_udp_port() const;
  QString current_state() const;
  bool connect_roadbook() const;

  Q_INVOKABLE void UpdateSquidState(bool checked, uint16_t port);
  void set_nmea_udp_port(uint16_t value);
  void set_connect_roadbook(bool value);

  bool nmea_udp_active() const;
  void set_nmea_udp_active(bool value);

  Q_INVOKABLE void transation(QString portName);
  void ConnectUSB(QString portName);

  InputsManager* inputs_manager();

 signals:
  void squid_connection_status_changed();
  void nmea_displayed_frames_changed();
  void nmea_udp_port_changed();
  void current_state_changed();
  void connect_roadbook_changed();
  void nmea_udp_active_changed();

 private:
  void Connect(std::error_code& err, uint16_t port);
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

  bool m_connect_roadbook;
  std::shared_ptr<std::thread> m_roadbook_connection_thread;

  bool m_nmea_udp_active;
  MasterThread m_thread;

  QThread* m_nmea_usb_open_thread { nullptr };
  QThread* m_nmea_usb_read_thread { nullptr };
  QString m_port_name;
  QSerialPort m_serial;

  InputsManager m_inputs_manager;
};

#endif // SQUID_GPS_DESKTOP_BACKEND_H
