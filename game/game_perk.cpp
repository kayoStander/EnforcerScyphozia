#include "game_perk.hpp"
#include "game_elements.hpp"
#include "game_player.hpp"

namespace Game {
  void Perk::GivePerkToPlayer(Player &player, const u32 id) {
    if (id == -1u) {
      LOG_WARNING(Core, "id sent is from a non-existent perk => {}", id);
      return;
    }
    if (const auto perkIt = std::ranges::find_if(perks, [&](const auto &pair) { return pair.first == id; });
        perkIt != perks.end()) {
      player.perks.emplace(player.perks.size(), perkIt->second);
    }
  }

  void Perk::RemovePerkFromPlayer(Player &player, const u32 id) {
    if (const auto it = std::ranges::find_if(player.perks, [&](const auto &pair) { return pair.first == id; });
        it != player.perks.end()) {
      player.perks.erase(it->first);
    }
  }

  u32 Perk::GetPerkId(const std::string &name) {
    if (const auto it{std::ranges::find_if(perks, [&](const auto &pair) { return pair.second->name == name; })};
        it != perks.end()) {
      return it->first;
    }

    LOG_ERROR(Core, "There's no such perk as {}", name);
    return -1u;
  }

#define START_TIMER()                                                                                                  \
  static auto TIMER_COUNT_ { std::chrono::high_resolution_clock::now() }
#define GET_TIME() (std::chrono::high_resolution_clock::now() - TIMER_COUNT_)
#define CHECK_TIME(y) (std::chrono::duration<float>(GET_TIME()).count() >= (y))

#define DEFINE_PERK(x) {__COUNTER__, x}

  // This is a terrible way of doing it but let's not talk about it
  std::unordered_map<u32, std::shared_ptr<Perk>> Perk::
  perks = {DEFINE_PERK(std::make_shared<Perk>(
           "Perk name", "Perk Description", []([[maybe_unused]] Player &player) {}, std::unordered_map<Scaling, f32>{},
           false)),

           // RACES //

           DEFINE_PERK(std::make_shared<Perk>(
           "Human", "Deal 33.33% more damage when below 50.0% health",
           [](Player &player) {
             if (player.scalings.at(Scaling::Health) <= player.scalings.at(Scaling::MaxHealth) * 0.5f) {
               player.scalings.at(Scaling::Health) *= 1.33f;
             } // look at that later
           },
           std::unordered_map<Scaling, f32>{}, false)),

           DEFINE_PERK(std::make_shared<Perk>(
           "Dwarf", "Better vision at dark, Tanky, Rock, Stone, Mines.", []([[maybe_unused]] Player &player) {},
           std::unordered_map<Scaling, f32>{{Scaling::Defense, 25.f}}, false)),

           DEFINE_PERK(std::make_shared<Perk>(
           "Catty", "Fast living creatures that hide in the shadows.", []([[maybe_unused]] Player &player) {},
           std::unordered_map<Scaling, f32>{{Scaling::MoveSpeed, 33.33f}, {Scaling::JumpPower, 10.f}}, false)),

           DEFINE_PERK(std::make_shared<Perk>(
           "High elf", "Your abilities have lower cooldown.", []([[maybe_unused]] Player &player) {},
           std::unordered_map<Scaling, f32>{{Scaling::CooldownReduction, 11.11f}}, false)),

           // ENCHANTS //

           DEFINE_PERK(std::make_shared<Perk>(
           "Firefly",
           "Every hit received spawns a firefly that heals you and damages the enemy; damage scales with fire.",
           []([[maybe_unused]] Player &player) {}, std::unordered_map<Scaling, f32>{{Scaling::Flame, 10.f}}, true)),

           DEFINE_PERK(std::make_shared<Perk>(
           "Vampiring", "Every hit receives life steal, loses damage at day.", []([[maybe_unused]] Player &player) {},
           std::unordered_map<Scaling, f32>{{Scaling::Flame, 10.f}}, true)),

           DEFINE_PERK(std::make_shared<Perk>(
           "Perking", "Remove all scaling from your armor, but gives an +11.11% buff to all perks.",
           []([[maybe_unused]] Player &player) {}, std::unordered_map<Scaling, f32>{{Scaling::Perking, 11.11f}},
           false)),

           // GUILDS //

           DEFINE_PERK(std::make_shared<Perk>(
           "Scholar", "You can recast your ability once.", []([[maybe_unused]] Player &player) {},
           std::unordered_map<Scaling, f32>{}, false)),

           DEFINE_PERK(std::make_shared<Perk>(
           "Deacon", "Chance to do critical heals", []([[maybe_unused]] Player &player) {},
           std::unordered_map<Scaling, f32>{{Scaling::Faith, 10.f}}, false)),

           DEFINE_PERK(std::make_shared<Perk>(
           "Silenced", "Your perking is halved at the cost of multiplying your damage and defense.",
           []([[maybe_unused]] Player &player) {},
           std::unordered_map<Scaling, f32>{
           {Scaling::Perking, -50.f}, {Scaling::Damage, 44.f}, {Scaling::Defense, 44.f}},
           false)),

           // ADVANTAGES - OBTAINABLE ONLY ON CHARACTER CREATION WITH AN RITUAL ITEM //

           DEFINE_PERK(std::make_shared<Perk>(
           "Looter", "Enemies can drop their rarest item easier.", []([[maybe_unused]] Player &player) {/*Just gives luck*/},
           std::unordered_map<Scaling, f32>{{Scaling::Luck, 1.5f}}, false)),

           // INFORMATION: THE BOON-FLAW FEATURE IS 1:1, SO YOU NEED A FLAW TO HAVE A BOON. LIMIT OF 3 BOONS.
           // BOONS - OBTAINABLE ONLY ON CHARACTER CREATION //

           DEFINE_PERK(std::make_shared<Perk>(
           "Heavy speed", "You are 10.0% faster, but jumps 20.0% low. Also all speed buffs are 5.0% better to you.",
           []([[maybe_unused]] Player &player) {},
           std::unordered_map<Scaling, f32>{{Scaling::MoveSpeed, 10.f}, {Scaling::JumpPower, -20.f}},
           false)), // PRICE: 1 FLAW

           DEFINE_PERK(std::make_shared<Perk>(
           "Charismatic", "Its harder to lose reputation.", []( Player &player) {
             player.achievements[static_cast<u8>(Player::Achievements::Charismatic)] = true;
           },
           std::unordered_map<Scaling, f32>{},
           false)), // PRICE: 1 FLAW

           DEFINE_PERK(std::make_shared<Perk>(
           "Professor", "Gain 1 extra point per level.", []( Player &player) {
             //player.points += 1u;
           },
           std::unordered_map<Scaling, f32>{},
           false)), // PRICE: 1 FLAW

           // FLAWS - OBTAINABLE ONLY ON CHARACTER CREATION //

           DEFINE_PERK(std::make_shared<Perk>(
           "Terrible trader", "Every buy-able item price is doubled.", [](Player &player) {
             player.achievements[static_cast<u8>(Player::Achievements::DoublePrice)] = true;
           },
           std::unordered_map<Scaling, f32>{},
           false)), // RECEIVE: 1 BOON

           DEFINE_PERK(std::make_shared<Perk>(
           "Bad pocket", "Your gold can randomly drop at the ground without warning.",
           []([[maybe_unused]] Player &player) {
             //while RNG(1, 1000 * ( 100 / player.scalings.at(Scaling::Luck))) == 1 {

             //}
           }, std::unordered_map<Scaling, f32>{},
           false)), // RECEIVE: 1 BOON

           DEFINE_PERK(std::make_shared<Perk>(
           "Schizophrenic",
           "False monsters may appear in your vision.", []([[maybe_unused]] Player &player) {},
           std::unordered_map<Scaling, f32>{},
           false)), // RECEIVE: 1 BOON

           // INFORMATION: THE MORE CHALLENGES THE BIGGER YOUR CHANCE OF GETTING AN RITUAL ITEM
           // CHALLENGES - OBTAINABLE ONLY ON CHARACTER CREATION //

           DEFINE_PERK(std::make_shared<Perk>(
           "Corrupt thread", "When attacked you will be dragged and stunned to the hitter.", []([[maybe_unused]] Player &player) {},
           std::unordered_map<Scaling, f32>{}, false)),

           DEFINE_PERK(std::make_shared<Perk>(
           "Unlucky", "Your luck is nullified.", [](Player &player) {
             player.scalings.at(Scaling::Luck) = 0.f;
           },
           std::unordered_map<Scaling, f32>{}, false)),


           // PERKS - OBTAINABLE ON UPGRADING, ARMOR AND (INCLUDE ALL WAYS HERE) //

           DEFINE_PERK(std::make_shared<Perk>(
           "Musician", "Your weapon becomes a lute and every tune that you play gives players a random buff.",
           []([[maybe_unused]] Player &player) {}, std::unordered_map<Scaling, f32>{{Scaling::AttackSpeed, -25.f}},
           true)),
           DEFINE_PERK(std::make_shared<Perk>(
           "Rock and stone",
           "Your tunes now only give players tanky buffs; also change your lute into an awesome guitar, extra buffs to "
           "dwarfs.",
           []([[maybe_unused]] Player &player) {},
           std::unordered_map<Scaling, f32>{
           {Scaling::Defense, 33.f}, {Scaling::MoveSpeed, .25f}, {Scaling::AttackSpeed, -10.f}},
           true)),
           DEFINE_PERK(std::make_shared<Perk>(
           "Marathon runner", "Runs fast at the start and gets slower with time.",
           []([[maybe_unused]] const Player &player) {
             // PRETTY BUGGy
             START_TIMER();
             if (CHECK_TIME(15)) {
               return;
             }
           },
           std::unordered_map<Scaling, f32>{{Scaling::MoveSpeed, 25.f}}, false)),
           DEFINE_PERK(std::make_shared<Perk>(
           "Plunger", "Makes you able to plunge from free falls.", []([[maybe_unused]] Player &player) {},
           std::unordered_map<Scaling, f32>{{Scaling::JumpPower, 15.f}}, true)),
           DEFINE_PERK(std::
                       make_shared<Perk>(
                       "Bucket.", "Spawns a bucket that does something, for sure.",
                       []([[maybe_unused]] Player &player) {
                         // spawns a bucket that does absolutely nothing every 30s
                       },
                       std::unordered_map<Scaling, f32>{{Scaling::JumpPower, 15.f}}, true)),
           DEFINE_PERK(std::make_shared<Perk>(
           "Explosive", "Every attack has a low chance of exploding, giving damage in AOE to enemies and you.",
           []([[maybe_unused]] Player &player) {
             // if (!RNG(33)) { return; }
           },
           std::unordered_map<Scaling, f32>{{Scaling::Flame, 15.f}, {Scaling::Physical, 15.f}}, true)),
           DEFINE_PERK(std::make_shared<Perk>(
           "Toxic", "Leaves a trail of toxin behind you; damage scales with move speed.",
           []([[maybe_unused]] Player &player) {}, std::unordered_map<Scaling, f32>{{Scaling::MoveSpeed, 10.f}},
           false)),
           DEFINE_PERK(std::make_shared<Perk>(
           "High elf technique", "You can recast your abilities once more at the cost of damage output.",
           []([[maybe_unused]] Player &player) {}, std::unordered_map<Scaling, f32>{}, false)),
           DEFINE_PERK(std::make_shared<Perk>(
           "Spell-master", "All your attacks will convert into a random element.",
           []([[maybe_unused]] Player &player) {},
           std::unordered_map<Scaling, f32>{{Scaling::Physical, 10.f},
                                            {Scaling::Faith, 10.f},
                                            {Scaling::Water, 10.f},
                                            {Scaling::Flame, 10.f},
                                            {Scaling::Wind, 10.f},
                                            {Scaling::Tech, 10.f}},
           true)),
           DEFINE_PERK(std::make_shared<Perk>(
           "Flaming", "Put enemies on fire!", []([[maybe_unused]] Player &player) {},
           std::unordered_map<Scaling, f32>{{Scaling::Flame, 10.f}}, true)),
           DEFINE_PERK(std::make_shared<Perk>(
           "Techno-flame", "Your fire now deals flame and tech damage.", []([[maybe_unused]] Player &player) {},
           std::unordered_map<Scaling, f32>{{Scaling::Flame, 5.f}, {Scaling::Tech, 5.f}}, true)),
           DEFINE_PERK(std::make_shared<Perk>(
           "Rhaegal's pet", "Have a pet dragon that sometimes throw fireballs.", []([[maybe_unused]] Player &player) {},
           std::unordered_map<Scaling, f32>{{Scaling::Flame, 6.3f}, {Scaling::Wind, 4.2f}}, true)),
           DEFINE_PERK(std::make_shared<Perk>(
           "Tech cannon", "You weapons is able to turn into a cannon, switching gives move speed for short duration.",
           []([[maybe_unused]] Player &player) {}, std::unordered_map<Scaling, f32>{{Scaling::Tech, 15.f}}, true)),
           DEFINE_PERK(std::make_shared<Perk>(
           "Backpack++", "Increases your inventory capacity.", []([[maybe_unused]] Player &player) {},
           std::unordered_map<Scaling, f32>{{Scaling::Tech, 15.f}}, true)),
           DEFINE_PERK(std::make_shared<Perk>(
           "Regalia", "Enemies wont aggro if you dont attack them.", []([[maybe_unused]] Player &player) {},
           std::unordered_map<Scaling, f32>{{Scaling::Tech, 15.f}}, true)),
           DEFINE_PERK(std::make_shared<Perk>(
           "Strengthened", "You lose all your armor in exchange to have 35.0% Armor Penetration.",
           []([[maybe_unused]] Player &player) {
             // player.flatScalings.at(Scaling::Defense) = 0.f;
           },
           std::unordered_map<Scaling, f32>{{Scaling::Physical, 15.f}, {Scaling::Penetration, 35.f}},
           // std::unordered_map<Scaling, f32>{{Scaling::Defense = 0.f}}; FLAT SCALING HERE WIP
           false))

  };
} // namespace Game
