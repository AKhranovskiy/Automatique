#pragma once
#include "types/resourcetype.h"
#include "units/basic.h"

struct unit_carrier_t : public unit_basic_t {
  size_t volume{0u};
  EResourceType type{EResourceType::None};

  using unit_basic_t::unit_basic_t;
  ~unit_carrier_t() override = default;
};
