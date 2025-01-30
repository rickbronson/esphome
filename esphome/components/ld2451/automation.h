#pragma once

#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "ld2451.h"

namespace esphome {
namespace ld2451 {

template<typename... Ts> class BluetoothPasswordSetAction : public Action<Ts...> {
 public:
  explicit BluetoothPasswordSetAction(LD2451Component *ld2451_comp) : ld2451_comp_(ld2451_comp) {}
  TEMPLATABLE_VALUE(std::string, password)

  void play(Ts... x) override { this->ld2451_comp_->set_bluetooth_password(this->password_.value(x...)); }

 protected:
  LD2451Component *ld2451_comp_;
};

}  // namespace ld2451
}  // namespace esphome
