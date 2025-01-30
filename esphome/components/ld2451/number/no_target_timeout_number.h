#pragma once

#include "esphome/components/number/number.h"
#include "../ld2451.h"

namespace esphome {
namespace ld2451 {

class NoTargetTimeoutNumber : public number::Number, public Parented<LD2451Component> {
 public:
  NoTargetTimeoutNumber() = default;

 protected:
  void control(float value) override;
};

}  // namespace ld2451
}  // namespace esphome
