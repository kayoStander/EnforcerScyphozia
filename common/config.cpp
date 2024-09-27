
#include "config.hpp"

#include <vector>
#include <vulkan/vulkan_core.h>

namespace Config {

///////////////////////////////////////////////////////////////////////////////
//                                      VARIABLE                             //
///////////////////////////////////////////////////////////////////////////////

std::string MainWindowName{"Enforcer [Schyphozia]"};
u32 MainWindowGeometryWidth{1280};
u32 MainWindowGeometryHeight{720};

///////////////////////////////////////////////////////////////////////////////
//                                      GETTER                               //
///////////////////////////////////////////////////////////////////////////////

[[gnu::pure, nodiscard]] auto GetMainWindowName() noexcept -> std::string {
  return MainWindowName;
}
[[gnu::pure, nodiscard]] auto GetMainWindowGeometryWidth() noexcept -> u32 {
  return MainWindowGeometryWidth;
}
[[gnu::pure, nodiscard]] auto GetMainWindowGeometryHeight() noexcept -> u32 {
  return MainWindowGeometryHeight;
}
namespace Scissors {
VkOffset2D scissorsOffset{0, 0};

[[gnu::pure, nodiscard]] auto GetScissorsOffset() noexcept -> VkOffset2D {
  return scissorsOffset;
}
} // namespace Scissors
namespace DeviceFeatures {
std::vector<PhysicalDeviceFeatures> deviceFeaturesEnabled{
    PhysicalDeviceFeatures::wideLines,
    PhysicalDeviceFeatures::samplerAnisotropy};

[[gnu::pure, nodiscard]] auto
GetPhysicalDeviceFeatures() noexcept -> std::vector<PhysicalDeviceFeatures> {
  return deviceFeaturesEnabled;
}
} // namespace DeviceFeatures

///////////////////////////////////////////////////////////////////////////////
//                                      SETTER                               //
///////////////////////////////////////////////////////////////////////////////

namespace Scissors {
constexpr void SetScissorsOffset(const s32 width, const s32 height) noexcept {
  scissorsOffset = {width, height};
}
} // namespace Scissors
constexpr void SetMainWindowName(const std::string name) noexcept {
  MainWindowName = name;
}
constexpr void SetMainWindowGeometryHeight(const u32 height) noexcept {
  MainWindowGeometryHeight = height;
}
constexpr void SetMainWindowGeometryWidth(const u32 width) noexcept {
  MainWindowGeometryWidth = width;
}

} // namespace Config
