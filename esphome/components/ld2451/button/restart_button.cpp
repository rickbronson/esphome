#include "restart_button.h"

namespace esphome {
namespace ld2451 {

void RestartButton::press_action() { this->parent_->restart_and_read_all_info(); }

}  // namespace ld2451
}  // namespace esphome
