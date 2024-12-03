#include <memory>
#include <logging/log.hpp>
#include <ranges>

#include "ability.hpp"
#include "player.hpp"
#include "math.hpp"

namespace Game {

  void Ability::SetPlayerAbility(Player &player, u32 ability){
    player.ability = ability;
  }

  void Ability::Activate(Player& player, const Enforcer::Window& window) {
    static std::unordered_map<u32, std::chrono::time_point<std::chrono::steady_clock>> playersCooldownList;
    const u32 playerId{player.GetId()};
    if (playersCooldownList.find(playerId) != playersCooldownList.end()){
      if (playersCooldownList.at(playerId) <= std::chrono::steady_clock::now() - abilities.at(player.ability)->cooldown) {
        playersCooldownList.erase(playerId);
      } else {
        return;
      }
    }

    if (glfwGetKey(window.GetGLFWWindow(), GLFW_KEY_T) == GLFW_PRESS) {
      //if (currentCooldown <= cooldown) {
      abilities.at(player.ability)->action(player);
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
                                           []([[maybe_unused]] Player& player){
                                             LOG_INFO(Debug,"Playing ability");
                                           },std::chrono::seconds(2u))),
    DEFINE_ABILITY(std::make_shared<Ability>("Link",
                                           "Link yourself to all players in close range, all heal received by you will also go to them.",
                                           []([[maybe_unused]] Player& player){
                                            for (auto& [id, otherPlayer] : Player::playerList) {
                                              if (*otherPlayer == player) {continue;}
                                              // temporary impl
                                              if (Math::Distance(player.GetViewerObject().transform.translation,
                                                                 otherPlayer->GetViewerObject().transform.translation) > 15.f) {
                                                continue;
                                              }

                                            }
                                           },std::chrono::seconds(15u))),
    DEFINE_ABILITY(std::make_shared<Ability>("Rock it out!",
                                           "does an AOE that buffs Speed, Defense and Damage. Needs a guitar.",
                                           []([[maybe_unused]] Player& player){},std::chrono::seconds(12u)))
  };
  #undef DEFINE_ABILITY
}