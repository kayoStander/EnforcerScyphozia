#include "filter.hpp"
#include <string_view>
#include "../assert.h"

namespace Common::Logging {
  namespace {
    template<typename It>
    Level GetLevelByName(const It begin, const It end) {
      for (u8 i{0}; i < static_cast<u8>(Level::Count); ++i) {
        const char *levelName = GetLevelName(static_cast<Level>(i));
        if (std::string_view{begin, end}.compare(levelName) == 0) {
          return static_cast<Level>(i);
        }
      }
      return Level::Count;
    }
    template<typename It>
    Class GetClassByName(const It begin, const It end) {
      for (u8 i{0}; i < static_cast<u8>(Class::Count); ++i) {
        const char *levelName = GetLogClassName(static_cast<Class>(i));
        if (std::string_view{begin, end}.compare(levelName) == 0) {
          return static_cast<Class>(i);
        }
      }
      return Class::Count;
    }
  } // namespace

#define ALL_LOG_CLASSES()                                                                                              \
  CLS(Logging)                                                                                                         \
  CLS(Common)                                                                                                          \
  CLS(Core)                                                                                                            \
  CLS(Debug)                                                                                                           \
  CLS(Discord) CLS(Config) CLS(GLFW) CLS(Vulkan) CLS(Shader) CLS(Client) CLS(Server)

  const char *GetLogClassName(Class logClass) {
    switch (logClass) {
#define CLS(x)                                                                                                         \
  case Class::x:                                                                                                       \
    return #x;
#define SUB(x, y)                                                                                                      \
  case Class::x##_##y:                                                                                                 \
    return #x "." #y;
      ALL_LOG_CLASSES();

#undef CLS
#undef SUB
      case Class::Count:
      default:
        break;
    }
    return "Unknown class";
  }

  const char *GetLevelName(Level logLevel) {
#define LVL(x)                                                                                                         \
  case Level::x:                                                                                                       \
    return #x
    switch (logLevel) {
      LVL(Trace);
      LVL(Debug);
      LVL(Info);
      LVL(Warning);
      LVL(Error);
      LVL(Critical);
      case Level::Count:
      default:
        break;
    }
#undef LVL
    return "Unknown level";
  }
} // namespace Common::Logging
