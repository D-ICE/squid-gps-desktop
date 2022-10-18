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
    REGISTER(HDG, hdg)
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
