#include "sgps/squid_gps_server.h"
#include <spdlog/spdlog.h>

namespace sgps {

class Connection : public std::enable_shared_from_this<Connection> {
 public:
  Connection(asio::io_context& context, const Model& model) :
    m_socket(context),
    m_deadline(context),
    m_model(model),
    m_on_disconnect([](){}) {}

  ~Connection() {
    m_on_disconnect();
  }

  asio::ip::tcp::socket& socket() { return m_socket; }

  void AsyncListen() {
    auto shared_this = shared_from_this();  // keep the object alive
    std::error_code err;
    m_deadline.cancel(err);
    if (err) {
      spdlog::debug("Could not cancel deadline: {}", err.message());
      err.clear();
    }
    m_deadline.expires_after(std::chrono::seconds(5));
    m_deadline.async_wait([shared_this](const asio::error_code& ec){
      if (ec) {
        return;
      }
      shared_this->m_socket.close();
    });

    m_socket.async_read_some(asio::buffer(m_datagram), [shared_this](const asio::error_code& ec, std::size_t size){
      if (ec) {
        spdlog::info("[sgps] Reading from tcp socket failed: {}. Closing the connection", ec.message());
        return;  // shared_this will be de-referenced leading to the connection getting closed
      }
      std::string datagram(shared_this->m_datagram.data(), size);
      spdlog::trace("[sgps] Received data: {}", datagram);
      if (datagram.rfind("AUTHORIZE", 0) == 0) {  // if starts with AUTHORIZE
        spdlog::info("[sgps] Allowing connection");
        shared_this->m_socket.send(asio::buffer("ALLOW"));
      } /*else if (datagram.rfind("DATA", 0) == 0) {
        // send representation of model
      }*/
      auto model = shared_this->m_model.SquidXFormatted();
      spdlog::debug("[sgps] Sending {}", model);
      shared_this->m_socket.send(asio::buffer(model));
      shared_this->AsyncListen();  // keep the connection open because the shared_this will be recreated
    });
  }

  void set_on_disconnect(std::function<void()> on_disc) {
    m_on_disconnect = std::move(on_disc);
  }

 private:
  asio::ip::tcp::socket m_socket;
  asio::steady_timer m_deadline;
  const Model& m_model;
  std::function<void()> m_on_disconnect;

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
    m_on_connected();
    connection->set_on_disconnect(m_on_disconnected);
    connection->AsyncListen();
    Listen();
  });
}

void SquidGPSServer::Connect(std::function<void()> on_connected,
                             std::function<void()> on_disconnect,
                             std::error_code &err) {
  m_on_connected = std::move(on_connected);
  m_on_disconnected = std::move(on_disconnect);

  asio::ip::udp::socket socket(m_context);
  socket.open(asio::ip::udp::v4(), err);
  if (err) {
    return;
  }
  socket.set_option(asio::socket_base::broadcast(true), err);
  if (err) {
    return;
  }
  socket.set_option(asio::socket_base::reuse_address(true), err);
  if (err) {
    return;
  }
  spdlog::info("[sgps] Sending a connection request to SquidX");
  auto payload = std::to_string(m_acceptor.local_endpoint().port());
  socket.send_to(asio::buffer(payload),
                 asio::ip::udp::endpoint(asio::ip::address_v4::broadcast(), kSquidXConnectionPort),
                 0, err);
}

}  // namespace sgps
