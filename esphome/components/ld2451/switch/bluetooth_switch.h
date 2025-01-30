#pragma once

#include "esphome/components/switch/switch.h"
#include "../ld2451.h"

namespace esphome {
namespace ld2451 {

class BluetoothSwitch : public switch_::Switch, public Parented<LD2451Component> {
 public:
  BluetoothSwitch() = default;

 protected:
  void write_state(bool state) override;
};

}  // namespace ld2451
}  // namespace esphome
