#pragma once

enum class ETileContent {
  Unknown,
  None,
  Coal,
  Ore,
  Water,
};
constexpr std::underlying_type_t<ETileContent> get_tile_content_count() noexcept {
  std::underlying_type_t<ETileContent> count = 0;
  switch (ETileContent{}) {
  case ETileContent::Unknown:
    ++count;
    [[fallthrough]];
  case ETileContent::None:
    ++count;
    [[fallthrough]];
  case ETileContent::Coal:
    ++count;
    [[fallthrough]];
  case ETileContent::Ore:
    ++count;
    [[fallthrough]];
  case ETileContent::Water:
    ++count;
    [[fallthrough]];
  default: {
  };
  }
  return count;
}
