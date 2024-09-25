#include <ctime>

#include "discord.hpp"
#include "logging/log.hpp"

namespace Discord {
RPC::~RPC() {
  LOG_DEBUG(Discord, "Discord RPC stopped");

  Discord_ClearPresence();
  Discord_Shutdown();
}

void RPC::Init() {
  DiscordEventHandlers handlers{};
  Discord_Initialize("1283468042298327131", &handlers, 1, NULL);
  startTimestamp = static_cast<u64>(time(nullptr));
  enabled = true;
  LOG_INFO(Discord, "Discord RPC initialized");
}

void RPC::Update(Discord::RPCStatus status) {
  if (!enabled) {
    LOG_ERROR(Discord, "RPC is disabled but it's being updated");
    return;
  }

  DiscordRichPresence discordPresence{};
  memset(&discordPresence, 0, sizeof(discordPresence));

  if (status == Discord::RPCStatus::Playing) {
    discordPresence.details = "Working at project NULL";
    discordPresence.state = "Using Enforcer Engine!";
  } else {
    discordPresence.details = "Idling";
  }

  discordPresence.largeImageKey = "largeImageKey";
  discordPresence.largeImageText = "largeImageText";

  discordPresence.startTimestamp = static_cast<u32>(startTimestamp);
  // discordPresence.endTimestamp = 1507665886;

  Discord_UpdatePresence(&discordPresence);

  LOG_TRACE(Discord, "Discord RPC updated");
}

} // namespace Discord
