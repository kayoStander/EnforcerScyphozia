#pragma once
#include "logging/log.hpp"

#include <vulkan/vulkan.h>

void assert_fail_impl();
// template <bool B, typename std::enable_if_t<B> * = nullptr>
// void static_assert_impl();
[[noreturn]] inline void unreachable_impl();

#ifdef _MSC_VER
#define NO_INLINE __declspec(noinline)
#else
#define NO_INLINE __attribute__((noinline))
#endif

#define ASSERT(x)                                                                                                      \
  do {                                                                                                                 \
    if (!(x)) [[unlikely]] {                                                                                           \
      LOG_CRITICAL(Debug, "Assertion failed ");                                                                        \
      assert_fail_impl();                                                                                              \
    }                                                                                                                  \
  } while (0)

#define ASSERT_LOG(x, ...)                                                                                             \
  do {                                                                                                                 \
    if (!(x)) [[unlikely]] {                                                                                           \
      LOG_CRITICAL(Debug, "Assertion failed => " __VA_ARGS__);                                                         \
      assert_fail_impl();                                                                                              \
    }                                                                                                                  \
  } while (0)

#define UNREACHABLE()                                                                                                  \
  do {                                                                                                                 \
    LOG_CRITICAL(Debug, "Unreachable code");                                                                           \
    unreachable_impl();                                                                                                \
  } while (0)

#define UNREACHABLE_LOG(...)                                                                                           \
  do {                                                                                                                 \
    LOG_CRITICAL(Debug, "Unreachable code => " __VA_ARGS__);                                                           \
    unreachable_impl();                                                                                                \
  } while (0)

#ifdef _DEBUG
#define DEBUG_ASSERT(x) ASSERT(x);
#define DEBUG_ASSERT_LOG(x, ...) ASSERT_LOG(x, __VA_ARGS__);
#else
#define DEBUG_ASSERT(x)                                                                                                \
  do {                                                                                                                 \
  } while (0)
#define DEBUG_ASSERT_LOG(x)                                                                                            \
  do {                                                                                                                 \
  } while (0)
#endif

#define UNIMPLEMENTED() ASSERT_LOG(false, "Unimplemented code")
#define UNIMPLEMENTED_LOG(...) ASSERT_LOG(false, "Unimplemented code => " __VA_ARGS__)

#define UNIMPLEMENTED_IF(x) ASSERT_LOG(!(x), "Unimplemented code");
#define UNIMPLEMENTED_IF_LOG(x, ...) ASSERT_LOG(!(x), "Unimplemented code => " __VA_ARGS__);

#define ASSERT_OR_EXECUTE(x, y)                                                                                        \
  do {                                                                                                                 \
    ASSERT(x);                                                                                                         \
    if (!(x)) [[unlikely]] {                                                                                           \
      y                                                                                                                \
    }                                                                                                                  \
  } while (0);

#define ASSERT_OR_EXECUTE_LOG(x, y, ...)                                                                               \
  do {                                                                                                                 \
    ASSERT_LOG(x, __VA_ARGS__);                                                                                        \
    if (!(x)) [[unlikely]] {                                                                                           \
      y                                                                                                                \
    }                                                                                                                  \
  } while (0);

#define VK_CHECK_RESULT_LOG(call, ...)                                                                                 \
  do {                                                                                                                 \
    VkResult x{(call)};                                                                                                \
    ASSERT_LOG(x == VK_SUCCESS, __VA_ARGS__);                                                                          \
  } while (0)
