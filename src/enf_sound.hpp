#pragma once

#include "../common/types.hpp"
#include <vector>

namespace Enforcer {
class Sound {
public:
  static Sound CreateSound(const char *filePath) noexcept {
    static u32 soundId{0};
    return Sound{soundId++, filePath};
  }
  void Play();
  u8 volume{10u};

  u32 GetId() const noexcept { return id; }

private:
  Sound(u32 id, const char *filePath);
  const u32 id;
  std::vector<u8> soundData;
  u32 sampleCount;
};
} // namespace Enforcer
