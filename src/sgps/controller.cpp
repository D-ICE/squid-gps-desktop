#include "sgps/controller.h"
#include <spdlog/spdlog.h>

#include <marnav/nmea/gga.hpp>
#include <marnav/nmea/gns.hpp>
#include <marnav/nmea/gll.hpp>
#include <marnav/nmea/rma.hpp>
#include <marnav/nmea/rmb.hpp>
#include <marnav/nmea/rmc.hpp>
#include <marnav/nmea/vtg.hpp>
#include <marnav/nmea/vhw.hpp>
#include <marnav/nmea/hdg.hpp>
#include <marnav/nmea/hdt.hpp>
#include <marnav/nmea/mwv.hpp>
#include <marnav/nmea/mwd.hpp>
#include <marnav/nmea/mmb.hpp>
#include <marnav/nmea/xdr.hpp>

namespace sgps {
  namespace internal {

    void OnSentence(const marnav::nmea::gga& s, Model& model) {
      if (s.get_lat()) { model.set_latitude(s.get_lat().value()); }
      if (s.get_lon()) { model.set_longitude(s.get_lon().value()); }
    }
    void OnSentence(const marnav::nmea::gns& s, Model& model) {
      if (s.get_lat()) { model.set_latitude(s.get_lat().value()); }
      if (s.get_lon()) { model.set_longitude(s.get_lon().value()); }
    }
    void OnSentence(const marnav::nmea::gll& s, Model& model) {
      if (s.get_lat()) { model.set_latitude(s.get_lat().value()); }
      if (s.get_lon()) { model.set_longitude(s.get_lon().value()); }
    }
    void OnSentence(const marnav::nmea::rma& s, Model& model) {
      if (s.get_lat()) { model.set_latitude(s.get_lat().value()); }
      if (s.get_lon()) { model.set_longitude(s.get_lon().value()); }
    }
    void OnSentence(const marnav::nmea::rmb& s, Model& model) {
      if (s.get_lat()) { model.set_latitude(s.get_lat().value()); }
      if (s.get_lon()) { model.set_longitude(s.get_lon().value()); }
      if (s.get_bearing()) { model.set_heading(s.get_bearing().value()); }
    }
    void OnSentence(const marnav::nmea::rmc& s, Model& model) {
      if (s.get_lat()) { model.set_latitude(s.get_lat().value()); }
      if (s.get_lon()) { model.set_longitude(s.get_lon().value()); }
      if (s.get_heading()) { model.set_heading(s.get_heading().value()); }
      if (s.get_sog()) { model.set_speed(s.get_sog().value().get<marnav::units::knots>().value()); }
    }
    void OnSentence(const marnav::nmea::vtg& s, Model& model) {
      if (s.get_speed_kn()) { model.set_speed(s.get_speed_kn().value().value());
      } else if (s.get_speed_kmh()) {
        marnav::units::velocity speed = s.get_speed_kmh().value();
        model.set_speed(speed.get<marnav::units::knots>().value());
      }
      if (s.get_track_magn()) { model.set_heading(s.get_track_magn().value()); }
      else if (s.get_track_true()) { model.set_heading(s.get_track_true().value()); }
    }
    void OnSentence(const marnav::nmea::vhw& s, Model& model) {
      if (s.get_speed_knots()) { model.set_speed(s.get_speed_knots().value().value());
      } else if (s.get_speed_kmh()) {
        marnav::units::velocity speed = s.get_speed_kmh().value();
        model.set_speed(speed.get<marnav::units::knots>().value());
      }
      if (s.get_heading_magn()) {
        model.set_heading(s.get_heading_magn().value());
      } else if (s.get_heading_true()) {
        model.set_heading(s.get_heading_true().value());
      }
    }
    void OnSentence(const marnav::nmea::hdt& s, Model& model) {
      if (s.get_heading()) { model.set_heading(s.get_heading().value()); }
    }
    void OnSentence(const marnav::nmea::hdg& s, Model& model) {
      if (s.get_heading()) { model.set_heading(s.get_heading().value()); }
    }
    void OnSentence(const marnav::nmea::mwv& s, Model& model) {
      if (!(s.get_data_valid() && s.get_data_valid().value() == marnav::nmea::status::ok)) {
        return;
      }
      if (s.get_angle() && s.get_angle_ref() && s.get_angle_ref().value() == marnav::nmea::reference::TRUE_REF) {
        model.set_wind_angle(s.get_angle().value());
      }
      if (s.get_speed()) {
        model.set_wind_speed(s.get_speed().value().get<marnav::units::knots>().value());
      }
    }
    void OnSentence(const marnav::nmea::mwd& s, Model& model) {
      if (s.get_direction_true().has_value()) {
        model.set_wind_direction(s.get_direction_true().value());
      } else if (s.get_direction_mag().has_value()) {
        model.set_wind_direction(s.get_direction_mag().value());
      }
      if (s.get_speed_kn()) {
        model.set_wind_speed(s.get_speed_kn().value().get<marnav::units::knots>().value());
      } else if (s.get_speed_ms()) {
        model.set_wind_speed(s.get_speed_ms().value().get<marnav::units::knots>().value());
      }
    }
    void OnSentence(const marnav::nmea::mmb& s, Model& model) {
      if (s.get_pressure_bars()) {
        model.set_pressure(s.get_pressure_bars().value() * 1000);  // 1 bar = 1000hPa
      } else if (s.get_pressure_inches_of_mercury()) {
        model.set_pressure(s.get_pressure_bars().value() * 33.86);  // 1 inch of mercury = 33.86hPa
      }
    }
    void OnSentence(const marnav::nmea::xdr& s, Model& model) {
      for (auto i = 0; i < marnav::nmea::xdr::max_transducer_info; i++) {
        try {
          auto info = s.get_info(i);
          if (!info) {
            continue;
          }
          if (info.value().transducer_type != 'P') {
            continue;
          }
          if (info.value().units_of_measurement == 'P') {  // Pascal
            model.set_pressure(info.value().measurement_data * 0.01);
          } else if (info.value().units_of_measurement == 'B') {  // Bar
            model.set_pressure(info.value().measurement_data * 1000);
          }
        } catch (const std::out_of_range&) {
          return;
        }
      }
    }

    template <typename Sentence>
    void OnSentence(const marnav::nmea::sentence& s, Model& model) {
      auto casted_sentence = marnav::nmea::sentence_cast<Sentence>(&s);
      if (casted_sentence == nullptr) {
        spdlog::error("[sgps] Could not case sentence: {}", marnav::nmea::to_string(s));
        return;
      }
      OnSentence(*casted_sentence, model);
    }
  
  }  // namespace internal

#define REGISTER(UPPER_NAME, LOWER_NAME) {marnav::nmea::sentence_id::UPPER_NAME, [](const marnav::nmea::sentence& s, Model& m){ internal::OnSentence<marnav::nmea::LOWER_NAME>(s, m); } }

  std::map<marnav::nmea::sentence_id, Controller::OnSentenceCallback> Controller::kRegistry = {
    REGISTER(GGA, gga),
    REGISTER(GNS, gns),
    REGISTER(GLL, gll),
    REGISTER(RMA, rma),
    REGISTER(RMB, rmb),
    REGISTER(RMC, rmc),
    REGISTER(VTG, vtg),
    REGISTER(VHW, vhw),
    REGISTER(HDT, hdt),
    REGISTER(HDG, hdg),
    REGISTER(MWV, mwv),
    REGISTER(MWD, mwd),
    REGISTER(MMB, mmb),
    REGISTER(XDR, xdr)
  };

  Controller::Controller(Model& model) : m_model(model) {}
  
  void Controller::OnSentence(const marnav::nmea::sentence &sentence) {
    if (kRegistry.find(sentence.id()) == std::end(kRegistry)) {
      spdlog::trace("[sgps] Unhandled sentence received: {}", marnav::nmea::to_string(sentence));
      return;
    }
    kRegistry[sentence.id()](sentence, m_model);
  }

}  // namespace sgps
