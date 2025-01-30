#pragma once

#include "esphome/components/button/button.h"
#include "../ld2451.h"

namespace esphome {
namespace ld2451 {

class ResetButton : public button::Button, public Parented<LD2451Component> {
 public:
  ResetButton() = default;

 protected:
  void press_action() override;
};

}  // namespace ld2451
}  // namespace esphome
