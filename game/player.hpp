#pragma once

#include "../common/types.hpp"

namespace Game {
class Player {
public:
  Player() noexcept;
  ~Player() noexcept;

  Player(const Player &) = delete;
  Player &operator=(const Player &) = delete;

private:
};
} // namespace Game
