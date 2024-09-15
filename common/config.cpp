
#include "config.hpp"

namespace Config {
s32 gpuId{-1};

std::string MainWindowName{"Enforcer-Schyphozia-v0-1"};
u32 MainWindowGeometryWidth{1280};
u32 MainWindowGeometryHeight{720};

std::string GetMainWindowName() noexcept { return MainWindowName; }
u16 GetMainWindowGeometryWidth() noexcept { return MainWindowGeometryWidth; }
u16 GetMainWindowGeometryHeight() noexcept { return MainWindowGeometryHeight; }

} // namespace Config
