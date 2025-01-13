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

    //  TODO: const u8 ScalingPass
    //  INFORMATION: when the effect will be applied, can be 0 to 5, when the effect will happens,
    //  ex: 10 base (pass -1) is multiplyed 10% (pass 1) is multiplyed by 20% (pass 2) and divided by 50% (pass 3).
    //  resulting in 26.4 (pass 6) damage.
    //  all passes will happen after the base scaling passes from armors.

    std::unordered_map<Scaling, bool> isScaleApplied{};
    //  WARN: only works for a player, so this will not let Multiplayer be real for now.

    // Perk(const Perk &) = delete;
    Perk &operator=(const Perk &) = delete;
    // Perk(Perk &&) = delete;
    Perk &operator=(const Perk &&) = delete;

    static void GivePerkToPlayer(Player &player, u32 id);

    static void RemovePerkFromPlayer(Player &player, u32 id);

    /*[[deprecated("You can use, but it's really recommended to just hardcode the id.")]]*/
    static u32 GetPerkId(const std::string &name);

  private:
    static std::unordered_map<u32, std::shared_ptr<Perk>> perks;
  };
}; // namespace Game

