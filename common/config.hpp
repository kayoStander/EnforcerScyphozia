#include <string>

#include "types.hpp"

namespace Config {
std::string GetMainWindowName() noexcept;
u16 GetMainWindowGeometryWidth() noexcept;
u16 GetMainWindowGeometryHeight() noexcept;

void SetMainWindowGeometryHeight() noexcept;
void SetMainWindowGeometryWidth() noexcept;
void SetMainWindowName() noexcept;
} // namespace Config
