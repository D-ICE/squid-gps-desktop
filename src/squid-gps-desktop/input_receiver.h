#ifndef SQUID_GPS_DESKTOP_INPUT_RECEIVER_H
#define SQUID_GPS_DESKTOP_INPUT_RECEIVER_H

#include <QQmlEngine>
#include <QObject>
#include <QString>

class InputReceiver : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString error_message READ error_message WRITE set_error_message NOTIFY errorMessageChanged)

 public:
  using QObject::QObject;

  QString error_message() const;
  void set_error_message(const QString& message);

  virtual void Start() = 0;
  virtual void Stop() = 0;

 signals:
  void errorMessageChanged();

 protected:
  QString m_error_message { "" };
};

#endif // SQUID_GPS_DESKTOP_INPUT_RECEIVER_H
