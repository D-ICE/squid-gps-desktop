#include "serial_settings.h"

SerialSettings::SerialSettings(QObject* parent): QObject(parent) {
  connect(this, &SerialSettings::baudrateChanged, this, &SerialSettings::changed);
  connect(this, &SerialSettings::dataBitsChanged, this, &SerialSettings::changed);
  connect(this, &SerialSettings::stopBitsChanged, this, &SerialSettings::changed);
  connect(this, &SerialSettings::parityChanged, this, &SerialSettings::changed);
  connect(this, &SerialSettings::flowControlChanged, this, &SerialSettings::changed);
}

QVariantList SerialSettings::baudrate_choices() {
  return qtEnumToList(QSerialPort::staticMetaObject, "BaudRate");
}

QVariantList SerialSettings::data_bits_choices() {
  return qtEnumToList(QSerialPort::staticMetaObject, "DataBits");
}

QVariantList SerialSettings::stop_bits_choices() {
  return qtEnumToList(QSerialPort::staticMetaObject, "StopBits");
}

QVariantList SerialSettings::parity_choices() {
  return qtEnumToList(QSerialPort::staticMetaObject, "Parity");
}

QVariantList SerialSettings::flow_control_choices() {
  return qtEnumToList(QSerialPort::staticMetaObject, "FlowControl");
}

SerialSettings* SerialSettings::self() {
  return this;
}

QSerialPort::BaudRate SerialSettings::baudrate() const {
  return m_baudrate;
}

void SerialSettings::set_baudrate(const QSerialPort::BaudRate& baudrate) {
  if (m_baudrate != baudrate) {
    m_baudrate = baudrate;
    emit baudrateChanged();
  }
}

QSerialPort::DataBits SerialSettings::data_bits() const {
  return m_data_bits;
}

void SerialSettings::set_data_bits(const QSerialPort::DataBits& data_bits) {
  if (m_data_bits != data_bits) {
    m_data_bits = data_bits;
    emit dataBitsChanged();
  }
}

QSerialPort::StopBits SerialSettings::stop_bits() const {
  return m_stop_bits;
}

void SerialSettings::set_stop_bits(const QSerialPort::StopBits& stop_bits) {
  if (m_stop_bits != stop_bits) {
    m_stop_bits = stop_bits;
    emit stopBitsChanged();
  }
}

QSerialPort::Parity SerialSettings::parity() const {
  return m_parity;
}

void SerialSettings::set_parity(const QSerialPort::Parity& parity) {
  if (m_parity != parity) {
    m_parity = parity;
    emit parityChanged();
  }
}

QSerialPort::FlowControl SerialSettings::flow_control() const {
  return m_flow_control;
}

void SerialSettings::set_flow_control(const QSerialPort::FlowControl& flow_control) {
  if (m_flow_control != flow_control) {
    m_flow_control = flow_control;
    emit flowControlChanged();
  }
}

void SerialSettings::from(SerialSettings* other) {
  if (other != nullptr) {
    set_baudrate(other->m_baudrate);
    set_data_bits(other->m_data_bits);
    set_stop_bits(other->m_stop_bits);
    set_parity(other->m_parity);
    set_flow_control(other->m_flow_control);
  }
}

bool SerialSettings::isSame(SerialSettings* other) {
  if (other != nullptr) {
    return (m_baudrate == other->m_baudrate &&
            m_data_bits == other->m_data_bits &&
            m_stop_bits == other->m_stop_bits &&
            m_parity == other->m_parity &&
            m_flow_control == other->m_flow_control);
  }
  return false;
}

QVariantList SerialSettings::qtEnumToList(const QMetaObject& meta_object, const std::string& enum_name) {
  QVariantList result;
  QMetaEnum meta_enum = meta_object.enumerator(meta_object.indexOfEnumerator(enum_name.c_str()));
  for (int i = 0; i < meta_enum.keyCount(); ++i) {
    int value = meta_enum.value(i);
    QString label = meta_enum.valueToKey(value);
    result.append(QVariantMap{{"label", label}, {"value", value}});
  }
  return result;
}
