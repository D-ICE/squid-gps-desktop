#include "backend.h"

#include <thread>

#include <QStandardPaths>
#include <QIODevice>
#include <QFile>
#include <QSerialPort>

#include <spdlog/spdlog.h>
#include <marnav/nmea/nmea.hpp>

const uint16_t BackEnd::kDefaultNMEAPort = 7000;

BackEnd::BackEnd(QObject *parent) :
    QObject(parent),
    m_squid_connection_state(DISCONNECTED),
    m_settings("Squid", "SquidGPSDesktop"),
    m_controller(m_model),
    m_connect_roadbook(false),
    m_nmea_udp_active(false) {
  ConnectNMEA(m_listener_err);
}

BackEnd::~BackEnd() {
  DisconnectNMEA(m_listener_err);
  Disconnect(m_listener_err);
  m_thread.terminate();
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

uint16_t BackEnd::nmea_udp_port() const {
  return nmea_udp_port_setting();
}

void BackEnd::set_nmea_udp_port(uint16_t value) {
  if (value == nmea_udp_port()) {
    return;
  }
  set_nmea_udp_port_setting(value);
  DisconnectNMEA(m_listener_err);
  if (m_listener_err) {
    spdlog::warn("Disconnect error: {}", m_listener_err.message());
  }
  m_listener_err.clear();
  ConnectNMEA(m_listener_err);
}

uint16_t BackEnd::nmea_udp_port_setting() const {
  return m_settings.value("nmea_udp_port", kDefaultNMEAPort).toUInt();
}

void BackEnd::set_nmea_udp_port_setting(uint16_t value) {
  m_settings.setValue("nmea_udp_port", value);
}

void BackEnd::ConnectNMEA(std::error_code& err) {
    if (m_nmea_udp_active) {
        spdlog::trace("Connecting to NMEA publisher on port {}", nmea_udp_port());
        m_listener_context = std::make_shared<asio::io_context>();
        spdlog::trace("Building nmea listener");
        m_listener = std::make_shared<sgps::NMEAListener>(*m_listener_context);
        m_listener->Initialize(nmea_udp_port(), err);
        if (err) {
            return;
        }
        spdlog::trace("Listening on port {}", nmea_udp_port());
        m_listener->AsyncListen([this](std::unique_ptr<marnav::nmea::sentence> sentence){
            spdlog::trace("New sentence {}", marnav::nmea::to_string(*sentence));
            m_controller.OnSentence(*sentence);

            // Update the visualized frames
            PushNMEAFrame(marnav::nmea::to_string(*sentence));
        });

        m_listener_context_thread = std::make_shared<std::thread>([this](){
            m_listener_context->run();
            spdlog::debug("Closing listener context.");
        });
    }
}

void BackEnd::PushNMEAFrame(std::string&& value) {
  m_received_nmea_frames.emplace_front(value);
  while (m_received_nmea_frames.size() > 10) {
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

void BackEnd::DisconnectNMEA(std::error_code& err) {
  spdlog::trace("Disconnecting NMEA Listener");
  if (!m_listener_context) {
    return;
  }
  m_listener_context->stop();
  if (m_listener_context_thread) {
    m_listener_context_thread->join();
    m_listener_context_thread.reset();
  }
  m_listener.reset();
  m_listener_context.reset();
}


void BackEnd::UpdateSquidState(bool checked, uint16_t port) {
  std::error_code err;
  if (checked) {
    m_squid_connection_state = CONNECTING;
    Connect(err, port);  // TODO handle err
  } else {
    m_squid_connection_state = DISCONNECTED;
    Disconnect(err);
  }
  emit squid_connection_status_changed();
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

void BackEnd::Disconnect(std::error_code& err) {
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

bool BackEnd::nmea_udp_active() const {
    return m_nmea_udp_active;
}

void BackEnd::set_nmea_udp_active(bool value) {
    if (m_nmea_udp_active == value)
        return;
    m_nmea_udp_active = value;
    if (m_nmea_udp_active) {
        if (m_nmea_usb_read_thread != nullptr) {
          m_nmea_usb_read_thread->terminate();
        }
        ConnectNMEA(m_listener_err);
    } else {
        DisconnectNMEA(m_listener_err);
        if (m_listener_err) {
            spdlog::warn("Disconnect error: {}", m_listener_err.message());
        }
        m_listener_err.clear();
    }
    emit nmea_udp_active_changed();
}

SerialSettings* BackEnd::serial_settings() {
  return &m_serial_settings;
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

void BackEnd::transation(QString portName) {
    // m_thread.transaction(portName, 1000);
    ConnectUSB(portName);
}


void BackEnd::ConnectUSB(QString portName) {
    m_port_name = portName;
    qDebug() << "Failed to open port: " ;
    m_nmea_usb_open_thread = QThread::create([this]{
        qDebug() << "Failed to open port: -" ;
        while (!m_nmea_udp_active) {
            m_serial.setPortName(m_port_name);
            m_serial.setBaudRate(4800);
            if (!m_serial.open(QIODevice::ReadOnly)) {
                spdlog::warn("Failed to open USB port: {}",m_port_name.toStdString());
                // qDebug() << "Failed to open port: " << m_port_name;
                return;
            }
        }
    });
    m_nmea_usb_open_thread->start();
    m_nmea_usb_read_thread = QThread::create([this]{
        // to improve!!
        while (true) {
            QByteArray responseData = m_serial.readAll();
            while (m_serial.waitForReadyRead(10)) {
                responseData += m_serial.readAll();
            }
            QString serialString = responseData.mid(responseData.indexOf("$"),responseData.indexOf("\r\n") + 2);
            if (!serialString.isEmpty()) {
                qDebug() << "SerialString" <<serialString;
                std::string sentence_str = serialString.toStdString();
                try {
                    auto sentence = marnav::nmea::make_sentence(sentence_str, marnav::nmea::checksum_handling::ignore);
                    qDebug() << "SerialString2" <<serialString;
                    m_controller.OnSentence(*sentence);
                    PushNMEAFrame(marnav::nmea::to_string(*sentence));
                } catch (std::invalid_argument e) {
                    spdlog::debug("[sgps] Could not decode sentence {} - {}", sentence_str, e.what());
                    continue;
                }
            }
        }
    });
    m_nmea_usb_read_thread->start();
}
