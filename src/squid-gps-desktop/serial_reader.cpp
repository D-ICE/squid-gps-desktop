#include "serial_reader.h"

#include <spdlog/spdlog.h>

#include <QByteArray>
#include <QSerialPortInfo>

const QString SerialReader::c_baudrate_setting_key = "serial_baudrate";
const QString SerialReader::c_data_bits_setting_key = "serial_data_bits";
const QString SerialReader::c_stop_bits_setting_key = "serial_stop_bits";
const QString SerialReader::c_parity_setting_key = "serial_parity";
const QString SerialReader::c_flow_control_setting_key = "serial_flow_control";

SerialReader::SerialReader(QSettings& settings, QObject* parent): InputReceiver(settings, parent) {
  connect(&m_serial_settings, &SerialSettings::baudrateChanged,
          this, [this](){ m_settings_ref.setValue(c_baudrate_setting_key, m_serial_settings.baudrate()); });
  connect(&m_serial_settings, &SerialSettings::dataBitsChanged,
          this, [this](){ m_settings_ref.setValue(c_data_bits_setting_key, m_serial_settings.data_bits()); });
  connect(&m_serial_settings, &SerialSettings::stopBitsChanged,
          this, [this](){ m_settings_ref.setValue(c_stop_bits_setting_key, m_serial_settings.stop_bits()); });
  connect(&m_serial_settings, &SerialSettings::parityChanged,
          this, [this](){ m_settings_ref.setValue(c_parity_setting_key, m_serial_settings.parity()); });
  connect(&m_serial_settings, &SerialSettings::flowControlChanged,
          this, [this](){ m_settings_ref.setValue(c_flow_control_setting_key, m_serial_settings.flow_control()); });

  LoadConfiguration();
}

void SerialReader::LoadConfiguration() {
  m_serial_settings.set_baudrate(m_settings_ref.value(c_baudrate_setting_key, QSerialPort::BaudRate::Baud4800).value<QSerialPort::BaudRate>());
  m_serial_settings.set_data_bits(m_settings_ref.value(c_data_bits_setting_key, QSerialPort::DataBits::Data8).value<QSerialPort::DataBits>());
  m_serial_settings.set_stop_bits(m_settings_ref.value(c_stop_bits_setting_key, QSerialPort::StopBits::OneStop).value<QSerialPort::StopBits>());
  m_serial_settings.set_parity(m_settings_ref.value(c_parity_setting_key, QSerialPort::Parity::NoParity).value<QSerialPort::Parity>());
  m_serial_settings.set_flow_control(m_settings_ref.value(c_flow_control_setting_key, QSerialPort::FlowControl::NoFlowControl).value<QSerialPort::FlowControl>());
}

SerialSettings* SerialReader::settings() {
  return &m_serial_settings;
}

QStringList SerialReader::portnames_list() const {
  QStringList list;
  for (const auto& portInfo : QSerialPortInfo::availablePorts()) {
    list.append(portInfo.portName());
  }
  return list;
}

void SerialReader::refreshPortnamesList() {
  emit portnamesListChanged();
}

void SerialReader::setSelectedPortname(const QString& name) {
  m_selected_portname = name;
  spdlog::info("Selected serial port is: '{}'.", m_selected_portname.toStdString());
}

void SerialReader::Start() {
  Stop(); //close previously open serial communication

  set_error_message("");
  m_serial_port.clearError();

  m_serial_port.setPortName(m_selected_portname);
  m_serial_port.setBaudRate(m_serial_settings.baudrate());
  m_serial_port.setDataBits(m_serial_settings.data_bits());
  m_serial_port.setStopBits(m_serial_settings.stop_bits());
  m_serial_port.setParity(m_serial_settings.parity());
  m_serial_port.setFlowControl(m_serial_settings.flow_control());

  if (m_serial_port.open(QIODevice::OpenModeFlag::ReadOnly)) {
    connect(&m_serial_port, &QSerialPort::errorOccurred, this, &SerialReader::onError, Qt::UniqueConnection);
    connect(&m_serial_port, &QSerialPort::readyRead, this, &SerialReader::onRead, Qt::UniqueConnection);
  }
  else {
    set_error_message(QString("Failure to open serial port '%1': %2.").arg(m_serial_port.portName(),
                                                                           m_serial_port.errorString()));
  }
}

void SerialReader::Stop() {
  if (m_serial_port.isOpen()) {
    m_serial_port.close();
    m_serial_port.disconnect();
  }
}

void SerialReader::onError(QSerialPort::SerialPortError err) {
  if (err != QSerialPort::NoError) {
    set_error_message(QString("Error (%1) on serial port.").arg(err));
    Stop();
  }
}

void SerialReader::onRead() {
  while (m_serial_port.canReadLine()) {
    emit sentenceReceived(m_serial_port.readLine().trimmed().constData());
  }
}
