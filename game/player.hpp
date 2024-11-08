#pragma once

#include <vector>
#include <string_view>

#include "../common/types.hpp"

namespace Game {
class Player {
public:
  	Player() noexcept;
  	~Player() noexcept;

 	Player(const Player &) = delete;
 	Player &operator=(const Player &) = delete;

    void TakeDamage(const f32 damage) noexcept{
    	if (damage == 0){ return; }
        health -= damage - (defense == 0 ? 1.2f32 : defense) * 1.2f32;
    }

    auto GetHealth() const noexcept -> decltype(health) { return health; }
    auto GetDefense() const noexcept -> decltype(defense) { return defense; }
    auto GetDamage() const noexcept -> decltype(damage) { return damage; }
    std::string_view GetName() const noexcept { return name; }

private:
	f32 health{100.f32};
	f32 defense{1.f32};
    f32 damage{10.f32};

    std::string_view name{"Nameless"};

    void CalculateDamage() const noexcept;
protected:

};
} // namespace Game
