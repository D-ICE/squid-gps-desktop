#include "sgps/model.h"

#include <spdlog/spdlog.h>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <marnav/geo/angle.hpp>

namespace sgps {

  std::string Model::SquidXFormatted() const {
    nlohmann::json json {};
    if (m_latitude.has_value()) {
      json["Lat"] = m_latitude.value();
    }
    if (m_longitude.has_value()) {
      json["Long"] = m_longitude.value();
    }
    if (m_heading.has_value()) {
      json["Heading"] = m_heading.value();
    }
    if (m_speed.has_value()) {
      // Note: knots to SquidX unit conversion required
      json["Speed"] = m_speed.value() / 1.85;
    }
    if (json.is_null()) {
      return "{}";
    }
    return json.dump();
  }

  std::string Model::TextFormatted() const {
    std::string lat_str("-");
    if (m_latitude.has_value()) {
      marnav::geo::latitude lat_angle(m_latitude.value());
      lat_str = fmt::format("{} {:02}°{:02}.{:0.3f}", marnav::geo::to_string(lat_angle.hem()), lat_angle.degrees(), lat_angle.minutes(), lat_angle.seconds());
    }
    std::string lon_str("-");
    if (m_longitude.has_value()) {
      marnav::geo::longitude lon_angle(m_longitude.value());
      lon_str = fmt::format("{} {:03}°{:02}.{:0.3f}", marnav::geo::to_string(lon_angle.hem()), lon_angle.degrees(), lon_angle.minutes(), lon_angle.seconds());
    }
    auto position = fmt::format("{} - {}", lat_str, lon_str);

    std::string hdg_str(" - ");
    if (m_heading.has_value()) {
      hdg_str = fmt::format("{:03.0f}", m_heading.value());
    }

    std::string sog_str(" - ");
    if (m_speed.has_value()) {
      sog_str = fmt::format("{:02.1f}", m_speed.value());
    }
    return fmt::format(R"(
  POS: {}
  HDG: {} °
  SOG: {} kn
    )", position, hdg_str, sog_str);
  }
}  // namespace sgps
