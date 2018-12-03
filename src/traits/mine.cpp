#include "traits/mine.hpp"

operation_t trait_mine_t::operation_mine(unit_carrier_t& miner, World::position_t area) noexcept {
  return [&miner, area]() -> bool {
    // TODO should get tile info without discovery
    miner.type = [tile = World::get_tile_info(area)]() {
      if (ETileContent::Coal == tile) return EResourceType::Coal;
      if (ETileContent::Ore == tile) return EResourceType::Ore;
      if (ETileContent::Water == tile) return EResourceType::Water;
      throw ex_trait_mine_no_resource{};
    }();
    constexpr auto KCapacity{5u};
    if (miner.volume < KCapacity) {
      World::Chronicles().Log(miner) << "mines 1 " << miner.type << '\n';
      miner.volume++;
    }

    return miner.volume == KCapacity;
  };
}
