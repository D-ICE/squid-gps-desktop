#include "inputs_manager.h"

#include <spdlog/spdlog.h>

SerialReader* InputsManager::serial_reader() {
  return &m_serial_reader;
}

UdpListener* InputsManager::udp_listener() {
  return &m_udp_listener;
}

void InputsManager::readSerial() {
  spdlog::info("{}", __func__);
  m_serial_reader.Start();
}

void InputsManager::listenUDP() {
  spdlog::info("{}", __func__);
  m_serial_reader.Stop();
}
