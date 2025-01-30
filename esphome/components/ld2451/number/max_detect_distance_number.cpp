#include "max_detect_distance_number.h"

namespace esphome {
namespace ld2451 {

void MaxDetectDistanceNumber::control(float value) {
  this->publish_state(value);
  this->parent_->set_detect_config();
}

}  // namespace ld2451
}  // namespace esphome
