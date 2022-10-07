#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "sgps/nmea_listener.h"
#include "sgps/squid_gps_server.h"

//TEST(sgps, nmea_listener) {
//  spdlog::set_level(spdlog::level::trace);
//  uint16_t port = 9871;
//
//  std::error_code err;
//  asio::io_context context;
//
//  sgps::NMEAListener listener(context);
//  listener.Initialize(port, err);
//  ASSERT_FALSE(err);
//  listener.AsyncListen([](std::unique_ptr<marnav::nmea::sentence> sentence){
//    spdlog::info("Received: {}", marnav::nmea::to_string(*sentence));
//  });
//
//  context.run();
//}

TEST(dgps, squid) {
  spdlog::set_level(spdlog::level::trace);
  
  std::error_code err;
  asio::io_context context;
  sgps::Model model;
  model.set_latitude(49.254484);
  model.set_longitude(-5.749971);
  model.set_heading(-40);
  model.set_speed(12.5);
  
  sgps::SquidGPSServer server(context, model);
  server.Initialize(err);
  ASSERT_FALSE(err);
  
  std::thread context_thread([&context](){
    context.run();
  });
  
  server.Connect(err);
  ASSERT_FALSE(err);
  
  context_thread.join();
}
