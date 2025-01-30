#include "snr_threshold_level_number.h"

namespace esphome {
namespace ld2451 {

void SnrThresholdLevelNumber::control(float value) {
  this->publish_state(value);
  this->parent_->set_radar_sensitivity();
}

}  // namespace ld2451
}  // namespace esphome
