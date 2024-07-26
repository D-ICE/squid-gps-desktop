#include "backend.h"

#include <QStandardPaths>
#include <QIODevice>
#include <QFile>

#include <spdlog/spdlog.h>
#include <marnav/nmea/nmea.hpp>

BackEnd::BackEnd(QObject *parent) :
 QObject(parent),
 m_squid_connection_state(DISCONNECTED),
 m_settings("Squid", "SquidGPSDesktop"),
 m_controller(m_model),
 m_connect_roadbook(false),
 m_inputs_manager(m_settings, this) {
  connect(m_inputs_manager.serial_reader(), &InputReceiver::sentenceReceived, this, &BackEnd::onSentence);
  connect(m_inputs_manager.udp_listener(), &InputReceiver::sentenceReceived, this, &BackEnd::onSentence);
}

BackEnd::~BackEnd() {
  Disconnect();
}

QString BackEnd::squid_connection_status() const {
  switch (m_squid_connection_state) {
    case DISCONNECTED:
      return "Disconnected";
    case CONNECTING:
      return "Connecting";
    case CONNECTED:
      return "Connected";
  }
  return "INVALID";
}

QString BackEnd::nmea_displayed_frames() const {
  return {m_displayed_nmea_frames.c_str()};
}

void BackEnd::PushNMEAFrame(std::string&& value) {
  m_received_nmea_frames.emplace_front(value);
  while (m_received_nmea_frames.size() > 15) {
    m_received_nmea_frames.pop_back();
  }
  m_displayed_nmea_frames.clear();
  for (const auto& v : m_received_nmea_frames) {
    m_displayed_nmea_frames += v;
    m_displayed_nmea_frames += "\r\n";
  }
  emit nmea_displayed_frames_changed();
  emit current_state_changed();
}

void BackEnd::updateSquidState(bool checked, uint16_t port) {
  std::error_code err;
  if (checked) {
    m_squid_connection_state = CONNECTING;
    Connect(err, port);  // TODO handle err
  } else {
    m_squid_connection_state = DISCONNECTED;
    Disconnect();
  }
  emit squid_connection_status_changed();
}

void BackEnd::onSentence(QString s) {
  try {
    auto sentence = marnav::nmea::make_sentence(s.toStdString(), marnav::nmea::checksum_handling::ignore);
    if (sentence) {
      m_controller.OnSentence(*sentence);
      // Update the visualized frames
      PushNMEAFrame(marnav::nmea::to_string(*sentence));
    }
  }
  catch(const std::exception& e) {
    spdlog::error("[BackEnd] Exception caught on sentence: {}.", e.what());
  }
}

void BackEnd::Connect(std::error_code& err, uint16_t port) {
  spdlog::trace("Building context");
  m_squid_context = std::make_shared<asio::io_context>();

  spdlog::trace("Building squid server");
  m_squid_server = std::make_shared<sgps::SquidGPSServer>(*m_squid_context, m_model, port);
  m_squid_server->Initialize(err);
  if (err) {
    return;
  }

  m_squid_context_thread = std::make_shared<std::thread>([this](){
    m_squid_context->run();
    spdlog::debug("Closing context.");
  });

  m_squid_connection_thread = std::make_shared<std::thread>([this](){
    while (m_squid_connection_state == CONNECTING) {
      spdlog::info("Sending connection request.");
      std::error_code err;
      m_squid_server->Connect([this](){
        m_squid_connection_state = CONNECTED;
        m_squid_connection_condition_variable.notify_all();  // stop waiting for the connection
        emit squid_connection_status_changed();
      }, [this](){
        m_squid_connection_state = DISCONNECTED;
        m_squid_connection_condition_variable.notify_all();  // stop waiting for the connection
        emit squid_connection_status_changed();
      }, err);
      if (err) {
        spdlog::warn("Failed to send connection request: {}", err.message());
      }

      // wait for 2 seconds before calling the connection again
      std::unique_lock lk(m_squid_connection_mutex);
      m_squid_connection_condition_variable.wait_for(lk, std::chrono::seconds(2));
      spdlog::debug("Waking up from connection request loop");
    }
  });
}

void BackEnd::Disconnect() {
  spdlog::trace("Disconnecting");
  if (!m_squid_context) {
    return;
  }
  m_squid_context->stop();
  if (m_squid_context_thread) {
    m_squid_context_thread->join();
    m_squid_context_thread.reset();
  }
  if (m_squid_connection_thread) {
    m_squid_connection_condition_variable.notify_all();
    m_squid_connection_thread->join();
    m_squid_context_thread.reset();
  }
  m_squid_server.reset();
  m_squid_context.reset();
}

QString BackEnd::current_state() const {
  return {m_model.TextFormatted().c_str()};
}

bool BackEnd::connect_roadbook() const {
  return m_connect_roadbook;
}

void BackEnd::set_connect_roadbook(bool value) {
  m_connect_roadbook = value;
  if (!m_connect_roadbook) {
    if (m_roadbook_connection_thread) {
      m_roadbook_connection_thread->join();
      m_roadbook_connection_thread.reset();
    }
    return;
  }
  m_roadbook_connection_thread = std::make_shared<std::thread>([this](){
    uint8_t index = 0;
    while(m_connect_roadbook) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      index += 1;
      // log current position
      {
        auto app_data = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        auto log_file_path = app_data.append("/../log/current.csv");
        QFile log_file(log_file_path);
        if(!log_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
          spdlog::error("Cannot open log file at {}", log_file_path.toStdString());
          continue;
        }
        QTextStream stream(&log_file);
        stream << QString(m_model.CSVFormatted().c_str());
      }
      // Log to trace every 15 seconds
      if (index >= 15) {
        index = 0;

        auto app_data = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        auto log_file_path = app_data.append("/../log/trace.csv");
        QFile log_file(log_file_path);
        if(!log_file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
          spdlog::error("Cannot open log file at {}", log_file_path.toStdString());
          continue;
        }
        QTextStream stream(&log_file);
        stream << QString(m_model.CSVFormatted().c_str());
        stream << "\n";
      }
    }
  });
}

InputsManager* BackEnd::inputs_manager() {
  return &m_inputs_manager;
}
