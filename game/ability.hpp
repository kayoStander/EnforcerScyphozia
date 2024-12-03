#pragma once

#include "../src/enf_window.hpp"

#include <functional>
#include <memory>
#include <unordered_map>
#include <chrono>

namespace Game {
  class Player;
  struct Ability {
    const std::string name;
    const std::string description;
    const std::function<void(Player& player)> action;
    const std::chrono::steady_clock::duration cooldown;

    //Ability(Ability &) = delete;
    Ability &operator=(const Ability &) = delete;
    //Ability(Ability &&) = delete;
    Ability &operator=(const Ability &&) = delete;

    static void SetPlayerAbility(Player &player, u32 ability);

    static void Activate(Player& player, const Enforcer::Window& window);

    /*[[deprecated("REALLY not recommended for performance reasons, just hardcode the id.")]]*/
    [[nodiscard]] static u32 GetAbilityId(const std::string& name);

  private:
    static std::unordered_map<u32, std::shared_ptr<Ability>> abilities;
  };
}