#include "nmea_listener.h"
#include <spdlog/spdlog.h>
#include <marnav/nmea/nmea.hpp>

namespace sgps {

  const std::string NMEAListener::kSentenceDelimiter = "\r\n";

  NMEAListener::NMEAListener(asio::io_context& context) : m_socket(context) {}

  void NMEAListener::Initialize(uint16_t port, std::error_code& err) {
    m_socket.open(asio::ip::udp::v4(), err);
    if (err) {
      return;
    }
    m_socket.set_option(asio::socket_base::reuse_address(true), err);
    if (err) {
      return;
    }
    m_socket.bind(asio::ip::udp::endpoint(asio::ip::address_v4::any(), port), err);
    if (err) {
      spdlog::error("[sgps] Could not bind: {}", err.message());
      return;
    }
  }

  void NMEAListener::AsyncListen(std::function<void (std::unique_ptr<marnav::nmea::sentence>)> on_sentence) {
    m_socket.async_receive(asio::buffer(m_datagram), [this, on_sentence](const asio::error_code& ec, std::size_t size){
      if (ec) {
        spdlog::error("[sgps] Listener failed: {}", ec.message());
        return;
      }
      m_buffer += std::string(m_datagram.data(), size);

      std::size_t pos = 0;
      while ((pos = m_buffer.find(kSentenceDelimiter)) != std::string::npos) {
        auto sentence_str = m_buffer.substr(0, pos);
        m_buffer.erase(0, pos + kSentenceDelimiter.length());

        try {
          auto sentence = marnav::nmea::make_sentence(sentence_str);
          on_sentence(std::move(sentence));
        } catch (std::invalid_argument e) {
          spdlog::debug("[sgps] Could not decode sentence {} - {}", sentence_str, e.what());
          continue;
        }
      }
      AsyncListen(on_sentence);
    });
  }

}  // namespace sgps
