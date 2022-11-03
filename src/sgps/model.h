#ifndef SGPS_MODEL_H_
#define SGPS_MODEL_H_

#include <optional>
#include <string>

namespace sgps {

  class Model {
   public:
    void set_latitude(double value) { m_latitude = value; }
    void set_longitude(double value) { m_longitude = value; }
    void set_heading(double value) { m_heading = value; }
    void set_speed(double value) { m_speed = value; }
    void set_wind_speed(double value) { m_wind_speed = value; }
    void set_wind_direction(double value) { m_wind_direction = value; }
    void set_wind_angle(double value) { m_wind_speed = value; }
    
    std::string SquidXFormatted() const;
    std::string TextFormatted() const;
    std::string CSVFormatted() const;
    
   private:
    std::optional<double> m_latitude;
    std::optional<double> m_longitude;
    std::optional<double> m_heading;
    std::optional<double> m_speed;
    std::optional<double> m_wind_speed;
    std::optional<double> m_wind_direction;
    std::optional<double> m_wind_angle;
  };

}  // namespace sgps

#endif  // SGPS_MODEL_H_
