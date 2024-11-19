#pragma once

namespace Game{
enum class Scaling : u8 {
  // Elements
  Damage=0, // 0
  Physical, // 1
  Faith, // 2
  Water, // 3
  Flame, // 4
  Wind, // 5
  Tech, // 6
  //
  Defense, // 7
  Health, // 8
  MaxHealth, // 9
  AttackSpeed, // 10
  MoveSpeed, // 11
  JumpPower, // 12
  Perking, // 13
  CooldownReduction, // 14
  ScalingCount
};
}