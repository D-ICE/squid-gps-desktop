#include <cstdint>

#include <spdlog/spdlog.h>
#include <cxxopts.hpp>

#include "sgps/nmea_listener.h"
#include "sgps/squid_gps_server.h"
#include "sgps/controller.h"

int main(int argc, char* argv[]) {
  cxxopts::Options options(argv[0], "Squid GPS from command line");
  options.add_options()
    ("p,port", "NMEA data UDP port", cxxopts::value<uint16_t>()->default_value("7000"))
    ("v,verbose", "Verbose output", cxxopts::value<bool>()->default_value("false"))
    ;
  auto result = options.parse(argc, argv);
  if (result.count("help")) {
    std::cout << options.help() << std::endl;
    return 0;
  }

  auto port = result["port"].as<uint16_t>();
  if (result["verbose"].as<bool>()) {
    spdlog::set_level(spdlog::level::trace);
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
  sgps::SquidGPSServer squid_server(context, model);
  squid_server.Initialize(err);
  if (err) {
    spdlog::error("Could not initialize squid server: {}", port, err.message());
    return -1;
  }
  squid_server.Connect(
    [](){ spdlog::info("Connected !"); },
    [&context](){
      spdlog::info("Connection closed by remote host. Closing.");
      context.stop();
    },
    err
  );

  spdlog::debug("Application started !");
  context.run();

  return 0;
}
