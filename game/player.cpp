#include "player.hpp"

namespace Game {
  u32 Player::GetPerkId(const std::string &name) {
    const auto it{std::ranges::find_if(perks,[&](const auto& pair) {
      return pair.second->name == name;
    })};
    if (it != perks.end()) {
      return it->first;
    }
    LOG_ERROR(Core, "There's no such perk as {}", name);
    return -1u;
  }

  std::shared_ptr<Player> Player::CreatePlayer(Enforcer::Window &window, Enforcer::Keyboard &keyboard,
                                           Enforcer::Camera &camera, Enforcer::GameObject &viewerObject) {
      auto player{std::make_shared<Player>(window, keyboard, camera, viewerObject)};
      playerList.emplace(player->id, player);
      return player;
    }

  void Player::Update() {
    // thread-possible, just don't want to do it rn.
    for (const auto &[id,perk]: perks) {
      if (!perk->affectsM1 and perk->action) {
        perk->action(*this);
      }
      for (const auto &[scaling,scaleAmount]: perk->scaling) {
        if (perk->isScaleApplied[scaling]){continue;}
        scalings.at(scaling) *= scaleAmount;
        perk->isScaleApplied.at(scaling) = true;
      }
      /*for (const auto &[scaling,scaleAmount]: perk->flatScaling) {
        if (perk->isScaleApplied[scaling]){continue;}
        scalings.at(flatScaling) *= scaleAmount;
        perk->isScaleApplied.at(flatScaling) = true;
      }*/
      //perk->isScaleApplied.clear();

      keyboard.moveSpeed = scalings.at(Scaling::MoveSpeed) * flatScalings.at(Scaling::MoveSpeed);
    }

    Ability::Activate(*this, window);
  }

  std::unordered_map<u32, std::shared_ptr<Player>> Player::playerList = {};
} // namespace Game
