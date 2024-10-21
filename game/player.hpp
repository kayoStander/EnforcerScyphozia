#pragma once

#include "../common/types.hpp"

namespace Game {
class Player {
public:
  static Player &Get() noexcept {
    static Player Instance;
    return Instance;
  }

  Player(const Player &) = delete;
  Player &operator=(const Player &) = delete;

private:
  Player();
  ~Player();
};
} // namespace Game
