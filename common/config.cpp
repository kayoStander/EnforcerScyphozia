
#include "config.hpp"

namespace Config {
s8 gpuId{-1}; // vulkan gpu index negative for auto select

std::string MainWindowName{"Enforcer-Schyphozia-v0-1"};
u32 MainWindowGeometryWidth{1280};
u32 MainWindowGeometryHeight{720};

std::string GetMainWindowName() noexcept { return MainWindowName; }
u32 GetMainWindowGeometryWidth() noexcept { return MainWindowGeometryWidth; }
u32 GetMainWindowGeometryHeight() noexcept { return MainWindowGeometryHeight; }

constexpr void SetMainWindowName(const std::string name) noexcept {
  MainWindowName = name;
}
constexpr void SetMainWindowGeometryHeight(const u16 height) noexcept {
  MainWindowGeometryHeight = height;
}
constexpr void SetMainWindowGeometryWidth(const u16 width) noexcept {
  MainWindowGeometryWidth = width;
}

} // namespace Config
