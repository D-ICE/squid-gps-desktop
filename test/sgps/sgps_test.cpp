#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "sgps/nmea_listener.h"

TEST(sgps, basic) {
  spdlog::set_level(spdlog::level::trace);
  uint16_t port = 9871;
  
  std::error_code err;
  asio::io_context context;
  
  sgps::NMEAListener listener(context);
  listener.Initialize(port, err);
  ASSERT_FALSE(err);
  listener.AsyncListen([](std::unique_ptr<marnav::nmea::sentence> sentence){
    spdlog::info("Received: {}", marnav::nmea::to_string(*sentence));
  });
  
  context.run();
}

TEST(dgps, squidx) {
  
}
