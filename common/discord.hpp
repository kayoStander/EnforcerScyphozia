#pragma once

#include <discord_register.h>
#include <discord_rpc.h>
#include "types.hpp"

namespace Discord {
  enum class RPCStatus { Idling, Playing };
  class RPC {
  public:
    explicit RPC() = default;
    ~RPC();

    RPC(RPC &&) = delete;
    RPC(const RPC &) = delete;
    RPC &operator=(RPC &&) = delete;
    RPC &operator=(const RPC &) = delete;

    void Init();
    void Update(RPCStatus status);

  private:
    [[maybe_unused]] u64 startTimestamp;
    bool enabled{false};
  };
}; // namespace Discord
