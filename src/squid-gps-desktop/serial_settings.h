#ifndef SQUID_GPS_DESKTOP_SERIAL_SETTINGS_H
#define SQUID_GPS_DESKTOP_SERIAL_SETTINGS_H

#include <string>

#include <QMetaObject>
#include <QObject>
#include <QQmlEngine>
#include <QSerialPort>

class SerialSettings : public QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(QVariantList baudrate_choices READ baudrate_choices CONSTANT)
  Q_PROPERTY(QVariantList data_bits_choices READ data_bits_choices CONSTANT)
  Q_PROPERTY(QVariantList stop_bits_choices READ stop_bits_choices CONSTANT)
  Q_PROPERTY(QVariantList parity_choices READ parity_choices CONSTANT)
  Q_PROPERTY(QVariantList flow_control_choices READ flow_control_choices CONSTANT)

  Q_PROPERTY(SerialSettings* self READ self NOTIFY changed)

  Q_PROPERTY(QSerialPort::BaudRate baudrate READ baudrate WRITE set_baudrate NOTIFY baudrateChanged)
  Q_PROPERTY(QSerialPort::DataBits data_bits READ data_bits WRITE set_data_bits NOTIFY dataBitsChanged)
  Q_PROPERTY(QSerialPort::StopBits stop_bits READ stop_bits WRITE set_stop_bits NOTIFY stopBitsChanged)
  Q_PROPERTY(QSerialPort::Parity parity READ parity WRITE set_parity NOTIFY parityChanged)
  Q_PROPERTY(QSerialPort::FlowControl flow_control READ flow_control WRITE set_flow_control NOTIFY flowControlChanged)

 public:
  explicit SerialSettings(QObject* parent = Q_NULLPTR);

  static QVariantList baudrate_choices();
  static QVariantList data_bits_choices();
  static QVariantList stop_bits_choices();
  static QVariantList parity_choices();
  static QVariantList flow_control_choices();

  SerialSettings* self();

  QSerialPort::BaudRate baudrate() const;
  void set_baudrate(const QSerialPort::BaudRate& baudrate);

  QSerialPort::DataBits data_bits() const;
  void set_data_bits(const QSerialPort::DataBits& data_bits);

  QSerialPort::StopBits stop_bits() const;
  void set_stop_bits(const QSerialPort::StopBits& stop_bits);

  QSerialPort::Parity parity() const;
  void set_parity(const QSerialPort::Parity& parity);

  QSerialPort::FlowControl flow_control() const;
  void set_flow_control(const QSerialPort::FlowControl& flow_control);

  Q_INVOKABLE void from(SerialSettings* other);
  Q_INVOKABLE bool isSame(SerialSettings* other);

 signals:
  void changed();

  void baudrateChanged();
  void dataBitsChanged();
  void stopBitsChanged();
  void parityChanged();
  void flowControlChanged();

 private:
  static QVariantList qtEnumToList(const QMetaObject& meta_object, const std::string& enum_name);

 private:
  QSerialPort::BaudRate m_baudrate { QSerialPort::BaudRate::Baud115200 };
  QSerialPort::DataBits m_data_bits { QSerialPort::DataBits::Data8 };
  QSerialPort::StopBits m_stop_bits { QSerialPort::StopBits::OneStop };
  QSerialPort::Parity m_parity { QSerialPort::Parity::NoParity };
  QSerialPort::FlowControl m_flow_control { QSerialPort::FlowControl::NoFlowControl };
};

#endif // SQUID_GPS_DESKTOP_SERIAL_SETTINGS_H
