#include "player.hpp"
#include "../common/logging/log.hpp"

namespace Game {
#define DEFINE_ARMOR(x)                                                                                                \
  { __COUNTER__, x }
#define DEFINE_ITEM(x)                                                                                                 \
  { __COUNTER__, x }

  /*std::unordered_map<u32, std::shared_ptr<Player::Item>> Player::Item::items = {
  DEFINE_ITEM(std::make_shared<Item>("Item name", "Item description", Player::Perk::GetPerkId("Perk name"),
                                     std::unordered_map<Scaling, f32>{}, 53)),
  DEFINE_ARMOR(std::make_shared<Item>("Bucket head", "Are you idiot?", Player::Perk::GetPerkId("Bucket."),
                                      std::unordered_map<Scaling, f32>{}, 1, ArmorType::Head)),
  };*/
#undef DEFINE_PERK
#undef DEFINE_ARMOR

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
  }
} // namespace Game
