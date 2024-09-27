#pragma once

#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "types.hpp"

namespace Config {

///////////////////////////////////////////////////////////////////////////////
//                                      GETTER                               //
///////////////////////////////////////////////////////////////////////////////

// namespace Window{
[[gnu::pure, nodiscard]] auto GetMainWindowName() noexcept -> std::string;
[[gnu::pure, nodiscard]] auto GetMainWindowGeometryWidth() noexcept -> u32;
[[gnu::pure, nodiscard]] auto GetMainWindowGeometryHeight() noexcept -> u32;
//}
namespace Scissors {
[[gnu::pure, nodiscard]] auto GetScissorsOffset() noexcept -> VkOffset2D;
}

namespace DeviceFeatures {
enum class PhysicalDeviceFeatures : u32 {
  wideLines = 0,
  samplerAnisotropy,
  count
};

[[gnu::pure, nodiscard]] auto
GetPhysicalDeviceFeatures() noexcept -> std::vector<PhysicalDeviceFeatures>;
} // namespace DeviceFeatures

///////////////////////////////////////////////////////////////////////////////
//                                      SETTER                               //
///////////////////////////////////////////////////////////////////////////////

namespace Scissors {
constexpr void SetScissorsOffset(const u32 width, const u32 height) noexcept;
}
// namespace Window {
constexpr void SetMainWindowName(const std::string name) noexcept;
constexpr void SetMainWindowGeometryHeight(const u32 height) noexcept;
constexpr void SetMainWindowGeometryWidth(const u32 width) noexcept;
//} // namespace Window
} // namespace Config
