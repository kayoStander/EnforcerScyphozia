#include <memory>
#include <logging/log.hpp>
#include <ranges>

#include "game_ability.hpp"
#include "game_player.hpp"
#include "game_math.hpp"

#include "../src/enf_frame_info.hpp"

namespace Game {

  void Ability::SetPlayerAbility(Player &player, u32 ability){
    player.ability = ability;
  }

  void Ability::Activate(Player& player, const Enforcer::Window& window, Enforcer::FrameInfo& frameInfo) {
    static std::unordered_map<u32, std::chrono::time_point<std::chrono::steady_clock>> playersCooldownList;
    const u32 playerId{player.GetId()};
    if (playersCooldownList.contains(playerId)) {
      if (playersCooldownList.at(playerId) <= std::chrono::steady_clock::now() - abilities.at(player.ability)->cooldown) {
        playersCooldownList.erase(playerId);
      } else {
        return;
      }
    }

    if (glfwGetKey(window.GetGLFWWindow(), GLFW_KEY_T) == GLFW_PRESS) {
      //if (currentCooldown <= cooldown) {
      abilities.at(player.ability)->action(player, frameInfo);
      playersCooldownList.emplace(playerId, std::chrono::steady_clock::now());
      //} else {
      // play sound
      //
    }
  }

  u32 Ability::GetAbilityId(const std::string& name){
    if (const auto it{std::ranges::find_if(abilities, [&](const auto &pair) { return pair.second->name == name; })};
    it != abilities.end()) {
      return it->first;
    }

    LOG_ERROR(Core, "There's no such perk as {}", name);
    return -1u;
  }

  #define DEFINE_ABILITY(x) {__COUNTER__, x}
  std::unordered_map<u32, std::shared_ptr<Ability>> Ability::abilities = {
    DEFINE_ABILITY(std::make_shared<Ability>("Ability name",
                                           "Ability description",
                                           []([[maybe_unused]] Player& player, [[maybe_unused]] Enforcer::FrameInfo& frameInfo){
                                             LOG_INFO(Debug,"Playing ability");
                                           },std::chrono::seconds(2u))),
    DEFINE_ABILITY(std::make_shared<Ability>("Link",
                                           "Link yourself to all players in close range, all heal received by you will also go to them.",
                                           [](Player& player, [[maybe_unused]] Enforcer::FrameInfo& frameInfo){
                                            constexpr float RANGE{15.f};
                                             //constexpr Scaling BASE_SCALING_PERCENT{};

                                            LOG_CRITICAL(Debug,"{} Is linking!", player.GetName());
                                            for (auto& [id, otherPlayer] : Player::playerList) {
                                              //if (*otherPlayer == player) {continue;}
                                              LOG_CRITICAL(Debug,"Linking player => {}!", otherPlayer->GetName());

                                              // temporary impl
                                              if (Math::Distance(player.GetPosition(),
                                                                 otherPlayer->GetPosition()) >= RANGE) {
                                                continue;
                                              }

                                              const std::shared_ptr<Enforcer::Model> cube
                                              {Enforcer::Model::CreateModelFromFile(player.GetDevice(), "model/cube.obj")};

                                              //std::thread{[&](){
                                                //[[maybe_unused]] std::chrono::time_point<std::chrono::steady_clock> startClock
                                                //{std::chrono::steady_clock::now()};

                                                //while (std::chrono::steady_clock::now() - startClock < std::chrono::seconds(5u)) {

                                                  Enforcer::GameObject gameObject{Enforcer::GameObject::CreateGameObject()};
                                                  gameObject.transform.scale = {5.f,5.f,5.f}; //Math::Lerp(glm::vec3{1.f,1.f,1.f},
                                                                                 //         glm::vec3{5.f,5.f,5.f}, 5);
                                                  gameObject.transform.translation = {0.f,-5.f,0.f}; // player.GetPosition();
                                                  gameObject.transform.rotation = {0.f,0.f,0.f}; // player.GetRotation();
                                                  gameObject.model = cube;
                                                  gameObject.imageBind = 0;
                                                  gameObject.physics.isStatic = true;

                                                  LOG_WARNING(Debug,"{}",gameObject.GetId());
                                                  //frameInfo.gameObjects.emplace(gameObject.GetId(), std::move(gameObject));
                                                  //std::this_thread::sleep_for(std::chrono::milliseconds(100));
                                              //}}.detach();
                                            }
                                           },std::chrono::seconds(15u))),
    DEFINE_ABILITY(std::make_shared<Ability>("Rock it out!",
                                           "does an AOE that buffs Speed, Defense and Damage. Needs a guitar.",
                                           [](Player& player, [[maybe_unused]] Enforcer::FrameInfo& frameInfo){
                                             constexpr float RANGE{10.f};
                                             //constexpr Scaling BASE_SCALING_PERCENT{};

                                             for (auto &otherPlayer: Player::playerList | std::views::values) {
                                               if (*otherPlayer == player) {continue;}
                                                 // temporary impl
                                               if (Math::Distance(player.GetPosition(),
                                                                otherPlayer->GetPosition()) > RANGE) {
                                                 continue;
                                               }
                                             }
                                           },std::chrono::seconds(12u))),
    DEFINE_ABILITY(std::make_shared<Ability>("Spinsword",
                                           "Sends your swords forward like a boomerang!",
                                           []([[maybe_unused]] Player& player, [[maybe_unused]] Enforcer::FrameInfo& frameInfo){
                                             [[maybe_unused]] constexpr float RANGE{25.f};
                                             //constexpr Scaling BASE_SCALING_PERCENT{};

                                           },std::chrono::seconds(18u)))
  };
  #undef DEFINE_ABILITY
}