#pragma once

#include <types.hpp>

namespace Game {
  class Player;
  class Ai {
  public:
    enum class State : u8 {
      Passive = 0,
      Searching,
      Aggro,
      Combat,
      Flee,
      Patrol,
      Idle,
      Alert,
      Chasing,
      Defensive,
      Disabled,
      Count
    };
    Ai() = default;
    virtual ~Ai() = default;

    virtual void Update(float deltaTime) = 0; // anyone who inherits this have to implement that
  protected:
    void SetState(const State _state) const noexcept { state = _state; };
    void Search(float range, float fovAngleDegrees);
    void Found(Player &player);
    mutable State state{State::Passive};
  };
} // namespace Game
