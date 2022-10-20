#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "sgps/controller.h"
#include "sgps/nmea_listener.h"
#include "sgps/squid_gps_server.h"

TEST(sgps, basic) {
  spdlog::set_level(spdlog::level::info);
  uint16_t port = 6001;

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

  sgps::SquidGPSServer server(context, model);
  server.Initialize(err);
  ASSERT_FALSE(err);

  std::thread context_thread([&context](){
    context.run();
  });

  server.Connect([](){},[](){}, err);
  ASSERT_FALSE(err);

  context_thread.join();
}
