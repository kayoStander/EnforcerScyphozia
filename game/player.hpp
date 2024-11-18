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

enum Guild {
  None = 0,
  Scholar,
  Deacon,
  GuildCount
};

enum Race {
  Human = 0,
  Elf,
  HighElf,
  Dwarf,
  RaceCount
};

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
        return -1u;
      }

    private:
      static std::unordered_map<u32, std::shared_ptr<Perk> > perks;
    };

    Player(/*Enforcer::Window &window,*/ Enforcer::Keyboard &keyboard, Enforcer::Camera &camera,
           Enforcer::GameObject &viewerObject) noexcept
      : /*window{window},*/ keyboard{keyboard}, camera{camera}, viewerObject{viewerObject} {
    };

    ~Player() noexcept = default;

    Player(const Player &) = delete;

    Player &operator=(const Player &) = delete;

    void TakeDamage(const f32 damage) noexcept {
      if (damage == 0) {
        return;
      }
      Health -= damage - (Defense == 0 ? 1.2f : Defense) * 1.2f;
    }

    // O(N) possible to refactor
    void attackM1() {
      /*for ([[maybe_unused]] auto &[id,perk]: perks) {
        if (glfwGetMouseButton(window.GetGLFWWindow(),GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS) {
          return;
        }
        if (!perk->affectsM1) { return; }
        //if (!Hitbox(range,idk))
        perk->action(*this);
      }*/
    }

    u32 GetPerkId(const std::string &name) {
      for (const auto &[id,perk]: perks) {
        if (perk->name == name) {
          return id;
        }
      }
      return -1u;
    }

    void Update() {
      // thread-possible, just dont wanna do it rn.
      for (const auto &[id,perk]: perks) {
        if (perk && perk->action) {
            perk->action(*this);
        }


        /*#define SET_SCALING(x,y) (x *= y / 100.f)
                for (const auto& [scaling,scaleAmount]:perk->scaling) {
                  //if (flatScaling){}
                  SET_SCALING(scaling,scaleAmount);
                }
        #undef SET_SCALING NOT WORKING AS EXPECTED*/
      }
    }

    [[nodiscard]] std::string_view GetName() const noexcept { return name; }
    [[nodiscard]] Enforcer::Keyboard &GetKeyboard() const noexcept { return keyboard; }

    f32 Damage{10.f};

    f32 Physical{10.f};
    f32 Faith{1.f};
    f32 Water{1.f};
    f32 Flame{1.f};
    f32 Wind{1.f};
    f32 Tech{1.f};

    f32 Defense{1.f};
    f32 Health{100.f};
    f32 MaxHealth{100.f};
    f32 AttackSpeed{1.f}; // %
    f32 MoveSpeed{4.5f};
    f32 JumpPower{2.f};
    f32 Perking{1.f};
    f32 CooldownReduction{100.f}; // %

    std::unordered_map<u32, std::shared_ptr<Perk> > perks{};

  private:
    //Enforcer::Window &window;
    Enforcer::Keyboard &keyboard;
    Enforcer::Camera &camera;
    Enforcer::GameObject &viewerObject;

    std::string_view name{"Nameless"};

    void CalculateDamage() const noexcept;

  protected:
  };
} // namespace Game
