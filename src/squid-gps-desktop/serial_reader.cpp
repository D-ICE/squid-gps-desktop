#include "serial_reader.h"

#include <spdlog/spdlog.h>

#include <QSerialPortInfo>

SerialSettings* SerialReader::settings() {
  return &m_settings;
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

  m_serial_port.setPortName(m_selected_portname);
  m_serial_port.setBaudRate(m_settings.baudrate());
  m_serial_port.setDataBits(m_settings.data_bits());
  m_serial_port.setStopBits(m_settings.stop_bits());
  m_serial_port.setParity(m_settings.parity());
  m_serial_port.setFlowControl(m_settings.flow_control());

  if (m_serial_port.open(QIODevice::OpenModeFlag::ReadOnly)) {
    connect(&m_serial_port, &QSerialPort::readyRead, this, [this]() {
      m_serial_port.readAll();
      //TODO
    });
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
