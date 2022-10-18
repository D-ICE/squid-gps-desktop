#ifndef SGPS_SQUID_GPS_SERVER_H_
#define SGPS_SQUID_GPS_SERVER_H_

#include <system_error>
#include <asio.hpp>

#include "sgps/model.h"

namespace sgps {

  class SquidGPSServer {
   public:
    SquidGPSServer(asio::io_context& context, const Model& model);
    void Initialize(std::error_code& err);
    void Connect(std::function<void()> on_connected, std::function<void()> on_disconnected, std::error_code& err);

   private:
    void Listen();

   private:
    static uint16_t kSquidXConnectionPort;
    asio::io_context& m_context;
    asio::ip::tcp::acceptor m_acceptor;
    const Model& m_model;
    std::function<void()> m_on_connected;
    std::function<void()> m_on_disconnected;
  };

}  // namespace sgps

#endif  // SGPS_SQUID_GPS_SERVER_H_
