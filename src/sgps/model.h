#ifndef SGPS_MODEL_H_
#define SGPS_MODEL_H_

#include <optional>

namespace sgps {

  class Model {
   public:
    void set_latitude(double value) { m_latitude = value; }
    void set_longitude(double value) { m_longitude = value; }
    void set_heading(double value) { m_heading = value; }
    void set_speed(double value) { m_speed = value; }
    
    std::string SquidXFormatted() const;
    std::string TextFormatted() const;
    
   private:
    std::optional<double> m_latitude;
    std::optional<double> m_longitude;
    std::optional<double> m_heading;
    std::optional<double> m_speed;
  };

}  // namespace sgps

#endif  // SGPS_MODEL_H_
