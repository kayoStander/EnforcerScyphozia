#pragma once

#include "types.hpp"
#include <discord_rpc.h>

namespace Discord {
enum class RPCStatus { Idling, Playing };
class RPC {

  u64 startTimestamp;
  bool enabled{false};

public:
  void init();
  void update(RPCStatus status);
  void stop();
};
}; // namespace Discord
