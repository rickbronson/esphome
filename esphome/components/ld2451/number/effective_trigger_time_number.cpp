#include "effective_trigger_time_number.h"

namespace esphome {
namespace ld2451 {

void EffectiveTriggerTimeNumber::control(float value) {
  this->publish_state(value);
  this->parent_->set_radar_sensitivity();
}

}  // namespace ld2451
}  // namespace esphome
