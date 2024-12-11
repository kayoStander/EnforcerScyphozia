#pragma once

#include "../src/enf_window.hpp"

#include <chrono>
#include <functional>
#include <memory>
#include <unordered_map>

namespace Enforcer {
  struct FrameInfo;
}

namespace Game {
  class Player;
  struct Ability {
    const std::string name;
    const std::string description;
    const std::function<void(Player& player, Enforcer::FrameInfo& frameInfo)> action;
    const std::chrono::steady_clock::duration cooldown;

    //Ability(Ability &) = delete;
    Ability &operator=(const Ability &) = delete;
    //Ability(Ability &&) = delete;
    Ability &operator=(const Ability &&) = delete;

    static void SetPlayerAbility(Player &player, u32 ability);
    static void Activate(Player& player, const Enforcer::Window& window, Enforcer::FrameInfo& frameInfo);

    /*[[deprecated("REALLY not recommended for performance reasons, just hardcode the id.")]]*/
    [[nodiscard]] static u32 GetAbilityId(const std::string& name);

  private:
    static std::unordered_map<u32, std::shared_ptr<Ability>> abilities;
  };
}