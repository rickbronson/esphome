#pragma once

#include "esphome/components/button/button.h"
#include "../ld2451.h"

namespace esphome {
namespace ld2451 {

class QueryButton : public button::Button, public Parented<LD2451Component> {
 public:
  QueryButton() = default;

 protected:
  void press_action() override;
};

}  // namespace ld2451
}  // namespace esphome
