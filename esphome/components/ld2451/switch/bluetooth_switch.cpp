#include "bluetooth_switch.h"

namespace esphome {
namespace ld2451 {

void BluetoothSwitch::write_state(bool state) {
  this->publish_state(state);
  this->parent_->set_bluetooth(state);
}

}  // namespace ld2451
}  // namespace esphome
