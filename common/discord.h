#pragma once

#include "types.h"
#include <discord_rpc.h>

namespace Discord {
enum class RPCStatus { Idling, Playing };
class RPC {

  uint64_t startTimestamp;
  bool enabled{false};

public:
  void init();
  void update(RPCStatus status);
  void stop();
};
}; // namespace Discord
