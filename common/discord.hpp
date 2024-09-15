#pragma once

#include "types.hpp"
#include <discord_rpc.h>

namespace Discord {
enum class RPCStatus { Idling, Playing };
class RPC {
public:
  explicit RPC() = default;
  ~RPC() = default;

  RPC(RPC &&) = delete;
  RPC(const RPC &) = delete;
  RPC &operator=(RPC &&) = delete;
  RPC &operator=(const RPC &) = delete;

  void init();
  void update(RPCStatus status);
  void stop();

private:
  u64 startTimestamp;
  bool enabled{false};
};
}; // namespace Discord
