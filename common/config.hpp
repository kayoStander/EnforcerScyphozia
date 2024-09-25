#include <string>

#include "types.hpp"

namespace Config {
std::string GetMainWindowName() noexcept;
u32 GetMainWindowGeometryWidth() noexcept;
u32 GetMainWindowGeometryHeight() noexcept;

constexpr void SetMainWindowName() noexcept;
constexpr void SetMainWindowGeometryHeight() noexcept;
constexpr void SetMainWindowGeometryWidth() noexcept;
} // namespace Config
