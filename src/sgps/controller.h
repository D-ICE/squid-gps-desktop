#ifndef SGPS_CONTROLLER_H_
#define SGPS_CONTROLLER_H_

#include <functional>
#include <string>
#include <map>

#include <marnav/nmea/sentence.hpp>
#include "sgps/model.h"

namespace sgps {

  class Controller {
   public:
    Controller(Model& model);
    void OnSentence(const marnav::nmea::sentence& sentence);
    
   private:
    using OnSentenceCallback = std::function<void(const marnav::nmea::sentence&, Model&)>;
    static std::map<marnav::nmea::sentence_id, OnSentenceCallback> kRegistry;
    
    Model& m_model;
  };

}  // namespace sgps

#endif  // SGPS_CONTROLLER_H_
