#include "game_player.hpp"
#include <ranges>

#include "../src/enf_frame_info.hpp"

namespace Game {
  u32 Player::GetPerkId(const std::string &name) {
    const auto it{std::ranges::find_if(perks, [&](const auto &pair) { return pair.second->name == name; })};
    if (it != perks.end()) {
      return it->first;
    }
    LOG_ERROR(Core, "There's no such perk as {}", name);
    return -1u;
  }

  std::shared_ptr<Player> Player::CreatePlayer(Enforcer::Window &window, Enforcer::Device &device,
                                               Enforcer::Keyboard &keyboard, Enforcer::Camera &camera,
                                               Enforcer::GameObject &viewerObject) {
    static u32 idAmount{0};
    auto player{std::make_shared<Player>(window, device, keyboard, camera, viewerObject, idAmount++)};
    playerList.emplace(player->id, player);
    return player;
  }

  void Player::Update(Enforcer::FrameInfo &frameInfo) {
    // thread-possible, just don't want to do it rn.
    for (const std::shared_ptr<Perk> &perk: std::views::values(perks)) {
      if (!perk->affectsM1 and perk->action) {
        perk->action(*this);
      }
      for (const auto &[scaling, scaleAmount]: perk->scaling) {
        if (perk->isScaleApplied[scaling])
          continue;
        scalings.at(scaling) *= scaleAmount;
        perk->isScaleApplied.at(scaling) = true;
      }
      /*for (const auto &[scaling,scaleAmount]: perk->flatScaling) {
        if (perk->isScaleApplied[scaling]){continue;}
        scalings.at(flatScaling) *= scaleAmount;
        perk->isScaleApplied.at(flatScaling) = true;
      }*/
      // perk->isScaleApplied.clear();

      keyboard.moveSpeed = scalings.at(Scaling::MoveSpeed) * flatScalings.at(Scaling::MoveSpeed);
    }

    Ability::Activate(*this, window, frameInfo);
  }

  std::unordered_map<u32, std::shared_ptr<Player>> Player::playerList = {};
} // namespace Game
