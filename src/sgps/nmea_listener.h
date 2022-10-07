#ifndef SGPS_NMEA_LISTENER_H_
#define SGPS_NMEA_LISTENER_H_

#include <functional>
#include <system_error>
#include <array>

#include <asio.hpp>
#include <marnav/nmea/sentence.hpp>

namespace sgps {

  class NMEAListener {
   public:
    explicit NMEAListener(asio::io_context& context);
    void Initialize(uint16_t port, std::error_code& err);
    
    using OnSentenceCallback = std::function<void(std::unique_ptr<marnav::nmea::sentence>)>;
    void AsyncListen(OnSentenceCallback on_sentence);
    
   private:
    static const std::string kSentenceDelimiter;
    
    asio::ip::udp::socket m_socket;
    std::array<char, 1024> m_datagram;
    std::string m_buffer;
    OnSentenceCallback m_callback;
  };

}  // namespace sgps

#endif  // SGPS_NMEA_LISTENER_H_
