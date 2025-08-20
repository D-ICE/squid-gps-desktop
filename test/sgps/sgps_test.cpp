#include <fstream>

#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <marnav/nmea/nmea.hpp>

#include "sgps/controller.h"
#include "sgps/nmea_listener.h"
#include "sgps/squid_gps_server.h"

TEST(sgps, basic) {
  spdlog::set_level(spdlog::level::info);
  uint16_t port = 5522;

  std::error_code err;
  asio::io_context context;

  sgps::Model model;
  sgps::Controller controller(model);

  sgps::NMEAListener listener(context);
  listener.Initialize(port, err);
  ASSERT_FALSE(err);
  listener.AsyncListen([&controller](std::unique_ptr<marnav::nmea::sentence> sentence){
    controller.OnSentence(*sentence);
  });

  sgps::SquidGPSServer server(context, model, 8000);
  server.Initialize(err);
  ASSERT_FALSE(err);

  std::thread context_thread([&context](){
    context.run();
  });

  server.Connect([](){},[](){}, err);
  ASSERT_FALSE(err);

  context_thread.join();
}

TEST(sgps, frames) {
  sgps::Model model;
  sgps::Controller controller(model);

  std::ifstream infile(FRAMES_TXT);
  std::string line;
  while (std::getline(infile, line)) {
    try {
      auto value = marnav::nmea::make_sentence(line);
    } catch (const std::invalid_argument& e) {
      spdlog::info("Error on {}: {}", line, e.what());
    } catch (const std::exception& e) {
      spdlog::info("Error on {}: {}", line, e.what());
    }
  }
}
