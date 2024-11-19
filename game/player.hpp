#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <logging/log.hpp>

#include "elements.hpp"
#include "../common/types.hpp"
#include "../src/enf_camera.hpp"
#include "../src/enf_game_object.hpp"
#include "../src/keyboard.hpp"

namespace Game {
  class Player {

  public:
    struct Perk {
    public:
      const std::string name;
      const std::string description;
      const std::function<void(Player &player)> action;
      std::unordered_map<Scaling, f32> scaling;
      const bool affectsM1;
      //int perking;
      std::unordered_map<Scaling, bool> isScaleApplied{}; // REDO THAT LATER, WILL ONLY WORK ONE PER PLAYER FOR NOW

      static void GivePerkToPlayer(Player &player, const u32 id) {
        if (id == -1u) { return; }
        if (const auto perkIt{perks.find(id)}; perkIt != perks.end()) {
          player.perks[player.perks.size()] = perkIt->second;
        }
      };

      static void RemovePerkFromPlayer(Player &player, const u32 id) {
        if (const auto it{player.perks.find(id)}; it != player.perks.end()) {
          player.perks.erase(it);
        }
      };

      // O(N) possible to refactor
      static u32 GetPerkId(const std::string &name) {
        for (const auto &[id,perk]: perks) {
          if (perk->name == name) {
            return id;
          }
        }
        LOG_ERROR(Core, "There's no such perk as {}", name);
        return -1u;
      }

    private:
      static std::unordered_map<u32, std::shared_ptr<Perk> > perks;
    };

    Player(Enforcer::Window &window, Enforcer::Keyboard &keyboard, Enforcer::Camera &camera,
           Enforcer::GameObject &viewerObject) noexcept
      : window{window}, keyboard{keyboard}, camera{camera}, viewerObject{viewerObject} {
    };

    ~Player() noexcept = default;

    Player(const Player &) = delete;

    Player &operator=(const Player &) = delete;

    void TakeDamage(const f32 damage) noexcept {
      if (damage == 0) {
        return;
      }
      scalings.at(Scaling::Health) -= damage - (scalings.at(Scaling::Defense) == 0 ? 1.2f : scalings.at(
        Scaling::Defense)) * 1.2f;
    }

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

    u32 GetPerkId(const std::string &name) {
      for (const auto &[id,perk]: perks) {
        if (perk->name == name) {
          return id;
        }
      }
      LOG_ERROR(Core, "There's no such perk as {}", name);
      return -1u;
    }

    void Update() {
      // thread-possible, just don't want to do it rn.
      for (const auto &[id,perk]: perks) {
        if (!perk->affectsM1 and perk->action) {
          perk->action(*this);
        }

        keyboard.moveSpeed = scalings.at(Scaling::MoveSpeed);

        for (const auto &[scaling,scaleAmount]: perk->scaling) {
          if (perk->isScaleApplied[scaling]){continue;}
          scalings.at(scaling) *= scaleAmount;
          perk->isScaleApplied.at(scaling) = true;
        }
        //perk->isScaleApplied.clear();
      }
    }

    [[nodiscard]] std::string_view GetName() const noexcept { return name; }
    [[nodiscard]] Enforcer::Keyboard &GetKeyboard() const noexcept { return keyboard; }
    [[nodiscard]] f32 GetScaling(const Scaling scale) const noexcept {return scalings.at(scale);}

    std::unordered_map<u32, std::shared_ptr<Perk> > perks{};

  private:
    Enforcer::Window &window;
    Enforcer::Keyboard &keyboard;
    Enforcer::Camera &camera;
    Enforcer::GameObject &viewerObject;

    std::unordered_map<Scaling, f32> scalings={
      {Scaling::Damage,10.f},
      {Scaling::Physical,1.f},
      {Scaling::Faith, 1.f},
      {Scaling::Water,1.f},
      {Scaling::Flame,1.f},
      {Scaling::Wind, 1.f},
      {Scaling::Tech,1.f},

      {Scaling::Defense,10.f},
      {Scaling::Health,100.f},
      {Scaling::MaxHealth,100.f},
      {Scaling::AttackSpeed,1.f}, // %
      {Scaling::MoveSpeed, 4.5f},
      {Scaling::JumpPower,2.f},
      {Scaling::Perking,1.f},
      {Scaling::CooldownReduction, 100.f} // %
    };

    std::string_view name{"Nameless"};

    void CalculateDamage() const noexcept;

  protected:
  };
} // namespace Game
