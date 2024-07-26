#ifndef SQUID_GPS_DESKTOP_SERIAL_READER_H
#define SQUID_GPS_DESKTOP_SERIAL_READER_H

#include <QQmlEngine>
#include <QObject>
#include <QSerialPort>
#include <QString>
#include <QStringList>

#include "input_receiver.h"
#include "serial_settings.h"

class SerialReader : public InputReceiver {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("Has C++ arguments in constructor")

  Q_PROPERTY(SerialSettings* settings READ settings CONSTANT)
  Q_PROPERTY(QStringList portnames_list READ portnames_list NOTIFY portnamesListChanged STORED false)

 public:
  SerialReader(QSettings& settings, QObject* parent = Q_NULLPTR);

  void LoadConfiguration();

  SerialSettings* settings();
  QStringList portnames_list() const;

  Q_INVOKABLE void refreshPortnamesList();
  Q_INVOKABLE void setSelectedPortname(const QString& name);

  virtual void Start() override;
  virtual void Stop() override;

 signals:
  void portnamesListChanged();

 private slots:
  void onError(QSerialPort::SerialPortError err);
  void onRead();

 private:
  const static QString c_baudrate_setting_key;
  const static QString c_data_bits_setting_key;
  const static QString c_stop_bits_setting_key;
  const static QString c_parity_setting_key;
  const static QString c_flow_control_setting_key;

  SerialSettings m_serial_settings;
  QString m_selected_portname { "" };
  QSerialPort m_serial_port;
};

#endif // SQUID_GPS_DESKTOP_SERIAL_READER_H
