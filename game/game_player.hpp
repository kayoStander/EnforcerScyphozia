#pragma once

#include <boost/mpl/aux_/adl_barrier.hpp>
#include <chrono>
#include <functional>
#include <logging/log.hpp>
#include <memory>
#include <ranges>
#include <string_view>
#include <thread>
#include <unordered_map>

#include "../common/types.hpp"
#include "../src/enf_camera.hpp"
#include "../src/enf_game_object.hpp"
#include "../src/keyboard.hpp"
#include "game_ability.hpp"
#include "game_elements.hpp"
#include "game_item.hpp"
#include "game_perk.hpp"

namespace Enforcer {
  struct FrameInfo;
}
namespace Game {
  class Player : public std::enable_shared_from_this<Player> {
  public:
    static std::shared_ptr<Player> CreatePlayer(Enforcer::Window &window, Enforcer::Device &device,
                                                Enforcer::Keyboard &keyboard, Enforcer::Camera &camera,
                                                Enforcer::GameObject &viewerObject);

    ~Player() noexcept = default;

    Player(const Player &) = delete;
    Player &operator=(const Player &) = delete;
    Player(Player &&) = delete;
    Player &operator=(const Player &&) = delete;

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
      for (const std::shared_ptr<Perk> &perk: perks | std::views::values) {
        if (glfwGetMouseButton(window.GetGLFWWindow(), GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS) {
          return;
        }
        if (!perk->affectsM1) {
          return;
        }
        // if (!Hitbox(range,idk))
        perk->action(*this);
      }
    }

    [[nodiscard]] u32 GetPerkId(const std::string &name);

    void Update(Enforcer::FrameInfo &frameInfo);

    [[nodiscard]] std::string_view GetName() const noexcept { return name; }
    [[nodiscard]] Enforcer::Device &GetDevice() const noexcept { return device; }
    [[nodiscard]] Enforcer::Keyboard &GetKeyboard() const noexcept { return keyboard; }
    [[nodiscard]] Enforcer::GameObject &GetViewerObject() const noexcept { return viewerObject; };
    [[nodiscard]] Enforcer::Camera &GetCamera() const noexcept { return camera; }

    [[nodiscard]] u32 GetId() const noexcept { return id; }
    [[nodiscard]] f32 GetScaling(const Scaling scale) const noexcept { return scalings.at(scale); }

    [[nodiscard]] glm::vec3 &GetPosition() const noexcept { return viewerObject.transform.translation; }
    [[nodiscard]] glm::vec3 &GetRotation() const noexcept { return viewerObject.transform.rotation; }
    [[nodiscard]] glm::vec3 &GetScale() const noexcept { return viewerObject.transform.scale; }

    std::unordered_map<u32, std::shared_ptr<Perk>> perks{};
    std::unordered_map<u32, std::shared_ptr<Item>> inventory{127};
    std::unordered_map<std::string_view, s32> keybinds;
    u32 ability{0};

    std::unordered_map<Scaling, f32> scalings = {
    {Scaling::Damage, 10.f},     {Scaling::Physical, 1.f},  {Scaling::Faith, 1.f},   {Scaling::Water, 1.f},
    {Scaling::Flame, 1.f},       {Scaling::Wind, 1.f},      {Scaling::Tech, 1.f},

    {Scaling::Penetration, 1.f}, {Scaling::Defense, 1.f},   {Scaling::Health, 1.f},  {Scaling::MaxHealth, 1.f},
    {Scaling::AttackSpeed, 1.f}, // %
    {Scaling::MoveSpeed, 1.f},   {Scaling::JumpPower, 1.f}, {Scaling::Perking, 1.f}, {Scaling::CooldownReduction, 1.f},
    {Scaling::Reputation, 0.f}, {Scaling::Luck, 1.f}, // shouldn't exist
    }; // % based

    /*struct Set {
      Helmet helmet;
      Chestplate chestplate;
      Leggings leggings;
    };*/

    static std::unordered_map<u32, std::shared_ptr<Player>> playerList;

    constexpr bool operator==(const Player &other) const { return this->GetId() == other.GetId(); }
    // constexpr bool operator==(const Player& player1, const Player& player2) const {return player1.GetId() ==
    // player2.GetId();}

    [[deprecated("just dont use that.")]] Player(Enforcer::Window &window, Enforcer::Device &device,
                                                 Enforcer::Keyboard &keyboard, Enforcer::Camera &camera,
                                                 Enforcer::GameObject &viewerObject, const u32 id) noexcept :
        window{window}, device{device}, keyboard{keyboard}, camera{camera}, viewerObject{viewerObject},
        id{id} {}; // should be private but lets not talk about it

    enum class Achievements : u8 {
      Charismatic,
      DoublePrice,
      AchievementsCount
    }; // temporary impl later make into .cpp
    std::array<bool, static_cast<u8>(Achievements::AchievementsCount)> achievements{};

  private:
    Enforcer::Window &window;
    Enforcer::Device &device;
    Enforcer::Keyboard &keyboard;
    Enforcer::Camera &camera;
    Enforcer::GameObject &viewerObject;
    const u32 id;

    std::unordered_map<Scaling, f32> flatScalings = {
    {Scaling::Damage, 10.f},     {Scaling::Physical, 10.f},   {Scaling::Faith, 10.f},
    {Scaling::Water, 10.f},      {Scaling::Flame, 10.f},      {Scaling::Wind, 10.f},
    {Scaling::Tech, 10.f},

    {Scaling::Penetration, 0.f}, {Scaling::Defense, 10.f},    {Scaling::Health, 100.f},
    {Scaling::MaxHealth, 100.f}, {Scaling::AttackSpeed, 1.f}, {Scaling::MoveSpeed, 5.f},
    {Scaling::JumpPower, 2.f},   {Scaling::Perking, 0.f},     {Scaling::CooldownReduction, 0.f}, // seconds based
    {Scaling::Reputation, 0.f}, {Scaling::Luck, 1.f}
    }; // flat based

    std::string_view name{"Nameless"};

    void CalculateDamage() const noexcept;

  protected:
  };
} // namespace Game
