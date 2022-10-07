#include "sgps/squid_gps_server.h"
#include <spdlog/spdlog.h>

namespace sgps {

class Connection : public std::enable_shared_from_this<Connection> {
 public:
  Connection(asio::io_context& context, const Model& model) :
    m_socket(context),
    m_model(model) {}
  
  asio::ip::tcp::socket& socket() { return m_socket; }
  
  void AsyncListen() {
    auto shared_this = shared_from_this();  // keep the object alive
    m_socket.async_read_some(asio::buffer(m_datagram), [shared_this](const asio::error_code& ec, std::size_t size){
      if (ec) {
        spdlog::info("[sgps] Reading from tcp socket failed: {}. Closing the connection", ec.message());
        return;  // shared_this will be de-referenced leading to the connection getting closed
      }
      
      std::string datagram(shared_this->m_datagram.data(), size);
      spdlog::trace("[sgps] Received data: {}", datagram);
      if (datagram.rfind("AUTHORIZE", 0) == 0) {  // if starts with AUTHORIZE
        spdlog::trace("[sgps] Allowing connection");
        shared_this->m_socket.send(asio::buffer("ALLOW"));
      } else if (datagram.rfind("DATA", 0) == 0) {
        // send representation of model
        auto model = shared_this->m_model.SquidXFormatted();
        spdlog::trace("[sgps] Sending {}", model);
        shared_this->m_socket.send(asio::buffer(model));
      }
      shared_this->AsyncListen();  // keep the connection open because the shared_this will be recreated
    });
  }

 private:
  asio::ip::tcp::socket m_socket;
  const Model& m_model;
  
  std::array<char, 4096> m_datagram;
};

uint16_t SquidGPSServer::kSquidXConnectionPort = 8000;

SquidGPSServer::SquidGPSServer(asio::io_context& context, const Model& model) :
  m_context(context),
  m_acceptor(context),
  m_model(model) {}

void SquidGPSServer::Initialize(std::error_code &err) {
  m_acceptor.open(asio::ip::tcp::v4(), err);
  if (err) {
    return;
  }
  m_acceptor.bind(asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 0), err);
  if (err) {
    return;
  }
  m_acceptor.listen(asio::ip::tcp::socket::max_listen_connections, err);
  if (err) {
    return;
  }
  Listen();
}

void SquidGPSServer::Listen() {
  spdlog::trace("[sgps] Listening to TCP connections");
  auto connection = std::make_shared<Connection>(m_context, m_model);
  m_acceptor.async_accept(connection->socket(), [connection, this](const asio::error_code& ec) {
    if (ec) {
      spdlog::error("[sgps] Could not accept connection: {}", ec.message());
      Listen();
      return;
    }
    spdlog::debug("[sgps] New connection setup");
    connection->AsyncListen();
    Listen();
  });
}

void SquidGPSServer::Connect(std::error_code& err) {
  Connect(asio::ip::address::from_string("127.0.0.1"), err);
}

void SquidGPSServer::Connect(const asio::ip::address& squidx_addr, std::error_code &err) {
  asio::ip::udp::socket socket(m_context);
  socket.open(asio::ip::udp::v4(), err);
  if (err) {
    return;
  }
  spdlog::debug("[sgps] Sending a connection request to SquidX");
  auto payload = std::to_string(m_acceptor.local_endpoint().port());
  socket.send_to(asio::buffer(payload), asio::ip::udp::endpoint(squidx_addr, kSquidXConnectionPort), 0, err);
}

}  // namespace sgps
