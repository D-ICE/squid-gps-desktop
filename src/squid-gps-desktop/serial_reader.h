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

  Q_PROPERTY(SerialSettings* settings READ settings CONSTANT)
  Q_PROPERTY(QStringList portnames_list READ portnames_list NOTIFY portnamesListChanged STORED false)

 public:
  using InputReceiver::InputReceiver;

  SerialSettings* settings();
  QStringList portnames_list() const;

  Q_INVOKABLE void refreshPortnamesList();
  Q_INVOKABLE void setSelectedPortname(const QString& name);

  virtual void Start() override;
  virtual void Stop() override;

 signals:
  void portnamesListChanged();

 private:
  SerialSettings m_settings;
  QString m_selected_portname { "" };
  QSerialPort m_serial_port;
};

#endif // SQUID_GPS_DESKTOP_SERIAL_READER_H
