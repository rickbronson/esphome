#include "no_target_timeout_number.h"

namespace esphome {
namespace ld2451 {

void NoTargetTimeoutNumber::control(float value) {
  this->publish_state(value);
  this->parent_->set_detect_config();
}

}  // namespace ld2451
}  // namespace esphome
