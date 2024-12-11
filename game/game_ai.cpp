#include "game_ai.hpp"
#include "game_math.hpp"
#include "game_player.hpp"

namespace Game{
  void Ai::Search(const float range, const float fovAngleDegrees){
    for (const auto& [id, player] : Player::playerList) {
      if (state != State::Searching) {
        return;
      }

      if (const glm::vec3 playerPosition{player->GetPosition()};
          Math::Distance(playerPosition,glm::vec3{0.f,0.f,0.f}) < range) {
        const glm::vec3 directionToPlayer{glm::normalize(playerPosition - glm::vec3{0.f, 0.f, 0.f})};
        const float dotProduct{glm::dot(glm::vec3{0.f,0.f,1.f}, directionToPlayer)};

        if (const float fovCosThreshold{static_cast<float>(cos(glm::radians(fovAngleDegrees / 2.f)))};
            dotProduct > fovCosThreshold){
          Found(*player);
        }
      }
    }
  }
  void Ai::Found([[maybe_unused]] Player& player) {
  }
}