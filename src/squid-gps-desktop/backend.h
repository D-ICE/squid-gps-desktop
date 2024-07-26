#ifndef SQUID_GPS_DESKTOP_BACKEND_H
#define SQUID_GPS_DESKTOP_BACKEND_H

#include <list>
#include <memory>
#include <thread>

#include <QObject>
#include <QString>
#include <QSettings>
#include <qqml.h>

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
  Q_PROPERTY(QString current_state READ current_state NOTIFY current_state_changed)
  Q_PROPERTY(bool connect_roadbook READ connect_roadbook WRITE set_connect_roadbook NOTIFY connect_roadbook_changed)
  Q_PROPERTY(InputsManager* inputs_manager READ inputs_manager CONSTANT)

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
  QString current_state() const;
  bool connect_roadbook() const;

  Q_INVOKABLE void updateSquidState(bool checked, uint16_t port);
  void set_connect_roadbook(bool value);

  Q_INVOKABLE void transation(QString portName);
  void ConnectUSB(QString portName);

  InputsManager* inputs_manager();

 signals:
  void squid_connection_status_changed();
  void nmea_displayed_frames_changed();
  void current_state_changed();
  void connect_roadbook_changed();

 private slots:
  void onSentence(QString s);

 private:
  void Connect(std::error_code& err, uint16_t port);
  void Disconnect(std::error_code& err);

  void PushNMEAFrame(std::string&& value);

 private:
  QSettings m_settings;

  sgps::Model m_model;
  sgps::Controller m_controller;

  std::list<std::string> m_received_nmea_frames;
  std::string m_displayed_nmea_frames;
  std::error_code m_listener_err;

  State m_squid_connection_state;
  std::shared_ptr<asio::io_context> m_squid_context;
  std::shared_ptr<sgps::SquidGPSServer> m_squid_server;
  std::shared_ptr<std::thread> m_squid_context_thread;
  std::mutex m_squid_connection_mutex;
  std::condition_variable m_squid_connection_condition_variable;
  std::shared_ptr<std::thread> m_squid_connection_thread;

  bool m_connect_roadbook;
  std::shared_ptr<std::thread> m_roadbook_connection_thread;

  MasterThread m_thread;

  QThread* m_nmea_usb_open_thread { nullptr };
  QThread* m_nmea_usb_read_thread { nullptr };
  QString m_port_name;
  QSerialPort m_serial;

  InputsManager m_inputs_manager;
};

#endif // SQUID_GPS_DESKTOP_BACKEND_H
