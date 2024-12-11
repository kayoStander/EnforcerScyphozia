#include "game_player.hpp"
#include "game_item.hpp"
#include "game_perk.hpp"

namespace Game{
  void Item::GiveItemToPlayer(Player &player, const u32 id) {
    if (id == -1u) { return; }
    if (const auto itemIt{items.find(id)}; itemIt != items.end()) {
      player.inventory.emplace(player.inventory.size() + 1u, itemIt->second);
    }
  }

  void Item::RemoveItemFromPlayer(Player &player, const u32 id) {
    if (const auto itemIt{player.inventory.find(id)}; itemIt != player.inventory.end()) {
      player.inventory.erase(itemIt);
    }
  }

  #define DEFINE_ITEM(x) {__COUNTER__, x}
  std::unordered_map<u32, std::shared_ptr<Item>> Item::items = {
  DEFINE_ITEM(std::make_shared<Item>("Item name", "Item description", 0u,
                                     std::unordered_map<Scaling, f32>{}, 53u)),
  };
  #undef DEFINE_ITEM
}