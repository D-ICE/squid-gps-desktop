#ifndef SQUID_GPS_DESKTOP_UDP_LISTENER_H
#define SQUID_GPS_DESKTOP_UDP_LISTENER_H

#include <memory>
#include <thread>

#include <QQmlEngine>
#include <QObject>
#include <QSettings>
#include <QString>
#include <QtGlobal>

#include "sgps/nmea_listener.h"

class UdpListener : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("Has C++ arguments in constructor")

  Q_PROPERTY(QString error_message READ error_message WRITE set_error_message NOTIFY errorMessageChanged)
  Q_PROPERTY(quint16 port READ port WRITE set_port NOTIFY portChanged STORED false)

 public:
  UdpListener(QSettings& settings, QObject* parent = Q_NULLPTR);
  virtual ~UdpListener() override;

  QString error_message() const;
  void set_error_message(const QString& message);

  quint16 port() const;
  void set_port(const quint16& port);

  void Start();
  void Stop();

 signals:
  void errorMessageChanged();
  void portChanged();
  void sentenceReceived(QString s);

 private:
  const static quint16 c_default_port;
  const static QString c_port_setting_key;
  QSettings& m_settings_ref;

  QString m_error_message { "" };

  std::shared_ptr<asio::io_context> m_listener_context;
  std::shared_ptr<sgps::NMEAListener> m_listener;
  std::shared_ptr<std::thread> m_listener_context_thread;
};

#endif // SQUID_GPS_DESKTOP_UDP_LISTENER_H
