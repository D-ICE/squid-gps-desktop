#ifndef SQUID_GPS_DESKTOP_SERIAL_READER_H
#define SQUID_GPS_DESKTOP_SERIAL_READER_H

#include <QQmlEngine>
#include <QObject>
#include <QSerialPort>
#include <QString>
#include <QStringList>

#include "serial_settings.h"

class SerialReader : public QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(QString error_message READ error_message WRITE set_error_message NOTIFY errorMessageChanged)
  Q_PROPERTY(SerialSettings* settings READ settings CONSTANT)
  Q_PROPERTY(QStringList portnames_list READ portnames_list NOTIFY portnamesListChanged STORED false)

 public:
  using QObject::QObject;
  virtual ~SerialReader() override;

  QString error_message() const;
  void set_error_message(const QString& message);

  SerialSettings* settings();
  QStringList portnames_list() const;

  Q_INVOKABLE void refreshPortnamesList();
  Q_INVOKABLE void setSelectedPortname(const QString& name);

  void Start();
  void Stop();

 signals:
  void errorMessageChanged();
  void portnamesListChanged();

 private:
  QString m_error_message { "" };
  SerialSettings m_settings;
  QString m_selected_portname { "" };
  QSerialPort m_serial_port;
};

#endif // SQUID_GPS_DESKTOP_SERIAL_READER_H
