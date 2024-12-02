#pragma once

#include <functional>
#include <memory>
#include <string>
#include <types.hpp>
#include <unordered_map>

namespace Game {
  class Player;
  enum class Scaling : u8;

  struct Perk {
  public:
    const std::string name;
    const std::string description;
    const std::function<void(Player &player)> action;
    std::unordered_map<Scaling, f32> scaling; // perk isn't supposed to have scaling, so I'll look that later
    const bool affectsM1;
    //int perking;
    std::unordered_map<Scaling, bool> isScaleApplied{}; // REDO THAT LATER, WILL ONLY WORK ONE PER PLAYER FOR NOW

    static void GivePerkToPlayer(Player &player, u32 id);

    static void RemovePerkFromPlayer(Player &player, u32 id);

    /*[[deprecated("You can use, but it's really recommended to just hardcode the id.")]]*/
    static u32 GetPerkId(const std::string &name);
  private:
    static std::unordered_map<u32, std::shared_ptr<Perk>> perks;
};
};