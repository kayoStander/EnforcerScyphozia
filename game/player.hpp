#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <boost/mpl/aux_/adl_barrier.hpp>
#include <logging/log.hpp>

#include "../common/types.hpp"
#include "../src/enf_camera.hpp"
#include "../src/enf_game_object.hpp"
#include "../src/keyboard.hpp"
#include "elements.hpp"
#include "perk.hpp"
#include "item.hpp"

namespace Game {
  class Player {
  public:
    Player(Enforcer::Window &window, Enforcer::Keyboard &keyboard, Enforcer::Camera &camera,
           Enforcer::GameObject &viewerObject) noexcept
      : window{window}, keyboard{keyboard}, camera{camera}, viewerObject{viewerObject} {
    };

    ~Player() noexcept = default;

    Player(const Player &) = delete;

    Player &operator=(const Player &) = delete;

    struct {
      /*void TakeDamage(const f32 damage) noexcept {
        if (damage == 0) {
        return;
        }
        scalings.at(Scaling::Health) -= damage - (scalings.at(Scaling::Defense) == 0 ? 1.2f : scalings.at(
          Scaling::Defense)) * 1.2f;
        }*/
    } Humanoid;

    // O(N) possible to refactor
    void attackM1() {
      for ([[maybe_unused]] auto &[id,perk]: perks) {
        if (glfwGetMouseButton(window.GetGLFWWindow(),GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS) {
          return;
        }
        if (!perk->affectsM1) { return; }
        //if (!Hitbox(range,idk))
        perk->action(*this);
      }
    }

    u32 GetPerkId(const std::string &name);

    void Update();

    [[nodiscard]] std::string_view GetName() const noexcept { return name; }
    [[nodiscard]] Enforcer::Keyboard &GetKeyboard() const noexcept { return keyboard; }
    [[nodiscard]] f32 GetScaling(const Scaling scale) const noexcept {return scalings.at(scale);}

    std::unordered_map<u32, std::shared_ptr<Perk> > perks{};
    std::unordered_map<u32, std::shared_ptr<Item> > inventory{127};
    std::unordered_map<std::string_view, s32> keybinds;

    std::unordered_map<Scaling, f32> scalings={
      {Scaling::Damage,10.f},
      {Scaling::Physical,1.f},
      {Scaling::Faith, 1.f},
      {Scaling::Water,1.f},
      {Scaling::Flame,1.f},
      {Scaling::Wind, 1.f},
      {Scaling::Tech,1.f},

      {Scaling::Defense, 1.f},
      {Scaling::Health, 1.f},
      {Scaling::MaxHealth, 1.f},
      {Scaling::AttackSpeed, 1.f}, // %
      {Scaling::MoveSpeed, 1.f},
      {Scaling::JumpPower, 1.f},
      {Scaling::Perking, 1.f},
      {Scaling::CooldownReduction, 1.f},
      {Scaling::Reputation, 0.f}, // shouldn't exist
    }; // % based

    /*struct Set {
      Helmet helmet;
      Chestplate chestplate;
      Leggings leggings;
    };*/

  private:
    Enforcer::Window &window;
    Enforcer::Keyboard &keyboard;
    Enforcer::Camera &camera;
    Enforcer::GameObject &viewerObject;

    std::unordered_map<Scaling, f32> flatScalings={
      {Scaling::Damage, 10.f},
      {Scaling::Physical, 10.f},
      {Scaling::Faith, 10.f},
      {Scaling::Water, 10.f},
      {Scaling::Flame, 10.f},
      {Scaling::Wind, 10.f},
      {Scaling::Tech, 10.f},

      {Scaling::Defense, 10.f},
      {Scaling::Health, 100.f},
      {Scaling::MaxHealth, 100.f},
      {Scaling::AttackSpeed, 1.f},
      {Scaling::MoveSpeed, 5.f},
      {Scaling::JumpPower, 2.f},
      {Scaling::Perking, 0.f},
      {Scaling::CooldownReduction, 0.f}, // seconds based
      {Scaling::Reputation, 0.f},
    }; // flat based

    std::string_view name{"Nameless"};

    void CalculateDamage() const noexcept;

  protected:
  };
} // namespace Game
