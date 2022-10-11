#include "backend.h"
#include <spdlog/spdlog.h>

BackEnd::BackEnd(QObject *parent) :
  QObject(parent),
  m_state(DISCONNECTED),
  m_nmea_port(6001),
  m_controller(m_model) {}

QString BackEnd::status() const {
  switch (m_state) {
    case DISCONNECTED:
      return "Disconnected";
    case CONNECTING:
      return "Connecting";
    case CONNECTED:
      return "Connected";
  }
  return "INVALID";
}

void BackEnd::UpdateState(bool checked) {
  std::error_code err;
  if (checked) {
    m_state = CONNECTING;
    Connect(err);
    // TODO: display error if any
  } else {
    m_state = DISCONNECTED;
    Disconnect(err);
    // TODO: display error if any
  }
  emit status_changed();
}

void BackEnd::Connect(std::error_code& err) {
  spdlog::trace("Building context");
  m_context = std::make_shared<asio::io_context>();

  spdlog::trace("Building nmea listener");
  m_listener = std::make_shared<sgps::NMEAListener>(*m_context);
  m_listener->Initialize(m_nmea_port, err);
  if (err) {
    return;
  }

  spdlog::trace("Building squid server");
  m_squid_server = std::make_shared<sgps::SquidGPSServer>(*m_context, m_model);
  m_squid_server->Initialize(err);
  if (err) {
    return;
  }

  spdlog::trace("Listening on port {}", m_nmea_port);
  m_listener->AsyncListen([this](std::unique_ptr<marnav::nmea::sentence> sentence){
    spdlog::trace("New sentence {}", marnav::nmea::to_string(*sentence));
    m_controller.OnSentence(*sentence);
  });

  m_context_thread = std::make_shared<std::thread>([this](){
    m_context->run();
    spdlog::debug("Closing context.");
  });

  spdlog::trace("Sending connection request.");
  m_squid_server->Connect([this](){
    m_state = CONNECTED;
    emit status_changed();
  }, err);
}

void BackEnd::Disconnect(std::error_code& err) {
  spdlog::trace("Disconnecting");
  if (!m_context) {
    return;
  }
  m_context->stop();
  if (m_context_thread) {
    m_context_thread->join();
    m_context_thread.reset();
  }
  m_context.reset();
  m_listener.reset();
  m_squid_server.reset();
}
