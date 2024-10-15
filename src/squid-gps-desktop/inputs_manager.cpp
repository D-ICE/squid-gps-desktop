#include "inputs_manager.h"

#include <spdlog/spdlog.h>

InputsManager::InputsManager(QSettings& settings, QObject* parent):
 QObject(parent),
 m_serial_reader(settings, this),
 m_udp_listener(settings, this) {

}

InputsManager::~InputsManager() {
  m_serial_reader.Stop();
  m_udp_listener.Stop();
}

SerialReader* InputsManager::serial_reader() {
  return &m_serial_reader;
}

UdpListener* InputsManager::udp_listener() {
  return &m_udp_listener;
}

void InputsManager::readSerial() {
  spdlog::info("{}", __func__);
  m_udp_listener.Stop();
  m_serial_reader.Start();
}

void InputsManager::listenUDP() {
  spdlog::info("{}", __func__);
  m_serial_reader.Stop();
  m_udp_listener.Start();
}
