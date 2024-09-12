#pragma once

void assert_fail_impl();
[[noreturn]] void unreachable_impl();

#ifdef _MSC_VER
#define NO_INLINE __declspec(noinline)
#else
#define NO_INLINE __attribute__((noinline))
#endif

#define ASSERT(x)                                                              \
  ([&]() NO_INLINE {                                                           \
    if (!(x)) [[unlikely]] {                                                   \
      printf("Assertion failed: %s\n", #x);                                    \
      assert_fail_impl();                                                      \
    }                                                                          \
  })

#define ASSERT_LOG(x, ...)                                                     \
  ([&]() NO_INLINE {                                                           \
    if (!(x)) [[unlikely]] {                                                   \
      printf("Assertion failed: " __VA_ARGS__ "\n");                           \
      assert_fail_impl();                                                      \
    }                                                                          \
  })

#define UNREACHABLE()                                                          \
  do {                                                                         \
    LOG_CRITICAL("Unreachable code: %d", __LINE__)                             \
    unreachable_impl();                                                        \
  } while (0)

#ifdef _DEBUG
#define DEBUG_ASSERT(x) ASSERT(x);
#define DEBUG_ASSERT_LOG(x, ...) ASSERT_LOG(x, __VA_ARGS__);
#else
#define DEBUG_ASSERT(x)                                                        \
  do {                                                                         \
  } while (0)
#define DEBUG_ASSERT_LOG(x)                                                    \
  do {                                                                         \
  } while (0)
#endif

#define UNIMPLEMENTED() ASSERT_LOG(false, "Unimplemented code")
#define UNIMPLEMENTED_LOG(...) ASSERT_LOG(false, __VA_ARGS__)

#define UNIMPLEMENTED_IF(x) ASSERT_LOG(!(x), "Unimplemented code");
#define UNIMPLEMENTED_IF_LOG(x, ...) ASSERT_LOG(!(x), __VA_ARGS__);

#define ASSERT_OR_EXECUTE(x, y)                                                \
  do {                                                                         \
    ASSERT(x);                                                                 \
    if (!(x)) [[unlikely]] {                                                   \
      y                                                                        \
    }                                                                          \
  } while (0);

#define ASSERT_OR_EXECUTE_LOG(x, y, ...)                                       \
  do {                                                                         \
    ASSERT_LOG(x, __VA_ARGS__);                                                \
    if (!(x)) [[unlikely]] {                                                   \
      y                                                                        \
    }                                                                          \
  } while (0);
