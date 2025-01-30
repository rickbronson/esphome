#pragma once

#include "esphome/components/select/select.h"
#include "../ld2451.h"

namespace esphome {
namespace ld2451 {

class MoveDirectionSelect : public select::Select, public Parented<LD2451Component> {
 public:
  MoveDirectionSelect() = default;

 protected:
  void control(const std::string &value) override;
};

}  // namespace ld2451
}  // namespace esphome
