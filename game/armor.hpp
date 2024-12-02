#pragma once

#include <types.hpp>
#include <string>
#include <unordered_map>

namespace Game{
  class player;
  struct Item;

  struct [[gnu::packed]] Armor {
    struct [[gnu::packed]] Helmet {
      const std::string name;
      const std::string description;
    };
    struct [[gnu::packed]] Chestplate {
      const std::string name;
      const std::string description;
    };
    struct [[gnu::packed]] Leggings {
      const std::string name;
      const std::string description;
    };

    u32 Perk;
  private:
    static std::unordered_map<u32, Armor> armors;
  };
}
