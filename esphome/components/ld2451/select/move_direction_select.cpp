#include "move_direction_select.h"

namespace esphome {
namespace ld2451 {

void MoveDirectionSelect::control(const std::string &value) {
  this->publish_state(value);
  this->parent_->set_detect_config();
}

}  // namespace ld2451
}  // namespace esphome
