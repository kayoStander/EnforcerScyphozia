#include <ctime>

#include "discord.h"

namespace Discord {
void RPC::init() {
  DiscordEventHandlers handlers{};
  Discord_Initialize("1139939140494971051", &handlers, 1, nullptr);

  startTimestamp = time(nullptr);
  enabled = true;
}

void RPC::update(Discord::RPCStatus status) {
  DiscordRichPresence rpc{};

  if (status == Discord::RPCStatus::Playing) {
    rpc.details = "playing testing EnforcerScyphozia";
    rpc.state = "EnforcerScyphozia";
  }
  rpc.details = "Idle";

  rpc.largeImageKey = "largeImageKey";
  rpc.largeImageText = "largeImageText";
  rpc.startTimestamp = startTimestamp;

  Discord_UpdatePresence(&rpc);
}

void RPC::stop() {
  if (!enabled) {
    return;
  }
  enabled = false;
  Discord_ClearPresence();
  Discord_Shutdown();
}
} // namespace Discord
