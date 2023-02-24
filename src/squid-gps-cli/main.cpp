#include <cstdint>

#include <asio.hpp>
#include <spdlog/spdlog.h>
#include <cxxopts.hpp>

#include "sgps/nmea_listener.h"
#include "sgps/squid_gps_server.h"
#include "sgps/controller.h"

void AsyncConnect(std::shared_ptr<asio::system_timer> timer,
                  std::shared_ptr<sgps::SquidGPSServer> squid_server) {

  timer->async_wait([timer, squid_server](const asio::error_code& aerr){
    if (aerr) {
      spdlog::debug("Timer failed: {}", aerr.message());
      return;
    }

    std::error_code err;

    spdlog::debug("Trying to connect to Squid");
    squid_server->Connect(
      [timer](){ 
        spdlog::warn("Connected !"); 
        std::error_code err;
        timer->cancel(err);
        if (err) {
          spdlog::debug("Could not cancel the timer: {}", err.message());
        }
      },
      [timer, squid_server](){ 
        spdlog::warn("Connection closed by remote host."); 
        AsyncConnect(timer, squid_server);
      },
      err
    );

    timer->expires_from_now(std::chrono::seconds(1));
    AsyncConnect(timer, squid_server);
  });
}

int main(int argc, char* argv[]) {
  cxxopts::Options options(argv[0], "Squid GPS from command line");
  options.add_options()
    ("p,port", "NMEA data UDP port", cxxopts::value<uint16_t>()->default_value("7000"))
    ("s,squidx_port", "SquidX UDP port", cxxopts::value<uint16_t>()->default_value("8000"))
    ("v,verbose", "Verbose output", cxxopts::value<bool>()->default_value("false"))
    ;
  auto result = options.parse(argc, argv);
  if (result.count("help")) {
    std::cout << options.help() << std::endl;
    return 0;
  }

  auto port = result["port"].as<uint16_t>();
  auto squidx_port = result["squidx_port"].as<uint16_t>();
  spdlog::set_level(spdlog::level::trace);
  if (!result["verbose"].as<bool>()) {
    spdlog::set_level(spdlog::level::warn);
  }

  std::error_code err;
  asio::io_context context;

  sgps::Model model;
  sgps::Controller controller(model);

  // Listen on given port for NMEA frames
  sgps::NMEAListener listener(context);
  listener.Initialize(port, err);
  if (err) {
    spdlog::error("Could not listen on port {}: {}", port, err.message());
    return -1;
  }
  listener.AsyncListen([&controller](std::unique_ptr<marnav::nmea::sentence> sentence){
    controller.OnSentence(*sentence);
  });

  // Connect to squid
  auto squid_server = std::make_shared<sgps::SquidGPSServer>(context, model, squidx_port);
  squid_server->Initialize(err);
  if (err) {
    spdlog::error("Could not initialize squid server: {}", port, err.message());
    return -1;
  }

  auto timer = std::make_shared<asio::system_timer>(context);
  AsyncConnect(timer, squid_server);
  
  spdlog::info("Application started !");
  context.run();

  return 0;
}
