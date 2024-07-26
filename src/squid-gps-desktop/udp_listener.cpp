#include "udp_listener.h"

#include <spdlog/spdlog.h>

const uint16_t UdpListener::c_default_port = 7000;
const QString UdpListener::c_port_setting_key = "nmea_udp_port";

quint16 UdpListener::port() const {
  return m_settings_ref.value(c_port_setting_key, c_default_port).toUInt();
}

void UdpListener::set_port(const quint16& port) {
  if (this->port() != port) {
    m_settings_ref.setValue(c_port_setting_key, port);
    emit portChanged();
  }
}

void UdpListener::Start() {
  Stop(); //close previously UDP communication

  set_error_message("");

  quint16 port = this->port();
  spdlog::trace("[UdpListener] Connecting to NMEA publisher on port {}.", port);
  m_listener_context = std::make_shared<asio::io_context>();
  spdlog::trace("[UdpListener] Building listener.");
  m_listener = std::make_shared<sgps::NMEAListener>(*m_listener_context);
  std::error_code err;
  m_listener->Initialize(port, err);
  if (err) {
    set_error_message(QString("Failure to initialize listener: %1.").arg(err.message().c_str()));
    return;
  }
  spdlog::trace("[UdpListener] Listening on port {}.", port);
  m_listener->AsyncListen([this](std::unique_ptr<marnav::nmea::sentence> sentence){
    if (sentence) {
      std::string str = marnav::nmea::to_string(*sentence);
      spdlog::trace("[UdpListener] New sentence {}.", str);
      emit sentenceReceived(str.c_str());
    }
  });

  m_listener_context_thread = std::make_shared<std::thread>([this](){
    m_listener_context->run();
    spdlog::debug("[UdpListener] Closing listener context.");
  });
}

void UdpListener::Stop() {
  if (!m_listener_context) {
    return;
  }
  spdlog::trace("[UdpListener] Disconnecting.");
  m_listener_context->stop();
  if (m_listener_context_thread) {
    m_listener_context_thread->join();
    m_listener_context_thread.reset();
  }
  m_listener.reset();
  m_listener_context.reset();
}
