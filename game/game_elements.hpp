#pragma once

namespace Game {
  enum class Scaling : u8 { // make the base scaling resume later, just made the percentage now.

    // Elements - All applied on pass -1.

    Damage = 0, // 0 - % Of base damage that will be applied to all other elements, based on base damage.
    Physical, // 1 - % Of physical damage that will
    Faith, // 2
    Water, // 3
    Flame, // 4
    Wind, // 5
    Tech, // 6

    // Info - All applied at pass -1.

    Penetration, // 7 - % Of Amount of Defense that will be ignored. [NO BASE SCALE EQUIVALENT]
    Defense, // 8 - % Of damage that will be mitigated. [NO BASE SCALE EQUIVALENT]
    Health, // 9 - Health, self-explanatory. [NO % SCALE EQUIVALENT AND NO BASE SCALE EQUIVALENT]
            // This is only for perks that might cause damage/heal.
    MaxHealth, // 10 - Max health, self-explanatory. [NO % SCALE EQUIVALENT]
    AttackSpeed, // 11 - % Of speed of your attack based on base attack speed [NO BASE SCALE EQUIVALENT]
    MoveSpeed, // 12 - Velocity of the character.
               // % - % Of movement move speed based on base move speed
    JumpPower, // 13 - Height capable for the character. (Gets smaller the lower your health is)
               // % - % Of movement jump power based on base jump power
    Perking, // 14 - multiply/divide (based on the perk) the powers of a perk,
             // starting at 1. each point will give one of this perk again.
             // ex: a 2 extra damage perk with 2 perking will do 2 * 2 extra damage.
    CooldownReduction, // 15 - Base number of cooldown reduction o abilities.
                       // % - % Of cooldown reduction on abilities based on base cooldown from ability.
                       // ex: a 25s cooldown ability with 4s cooldown reduction will have a 13s cooldown.
    Reputation, // 16 - The way people see you. You'll not be able to talk with certain people having low Rep. [NO % SCALE EQUIVALENT]
    Luck, // 17 - % Of Small a buff to RNG, small numbers might be unnoticeable. if luck > 50 then it will be halved upwards.
          // ex. (75 <Luck> / 2) + 25 [NO BASE SCALE EQUIVALENT]
    InventorySize, // 18 - Amount of items you are capable of carry.
                   // % - % Of items you're capable of carrying based on base inventory size.
    ScalingCount
  };
}

