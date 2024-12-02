#pragma once

#include <types.hpp>

#include <memory>
#include <string>
#include <unordered_map>

namespace Game {
  struct Perk;
  class Player;
  enum class Scaling : u8;

  struct Item {
    const std::string name;
    const std::string description;
    u32 perk;
    std::unordered_map<Scaling, f32> scalings;
    u8 maxAmount;

    static void GiveItemToPlayer(Player &player, u32 id);
    static void RemoveItemFromPlayer(Player &player, u32 id);

  private:
    static std::unordered_map<u32, std::shared_ptr<Item>> items;
  };
}