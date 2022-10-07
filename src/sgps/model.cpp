#include "sgps/model.h"
#include <nlohmann/json.hpp>

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
    return json.dump();
  }

}  // namespace sgps
