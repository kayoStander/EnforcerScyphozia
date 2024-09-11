#pragma once

void assert_fail_imp();
[[noreturn]] void unreachable_impl();

#define LOG_CRITICAL(fmt, ...) printf(fmt "\n", ##__VA_ARGS__);

#ifdef _MSC_VER
#define NO_INLINE __declspec(noinline)
#else
#define NO_INLINE __attribute__((noinline))
#endif

#define ASSERT(x)                                                              \
  ([&]() NO_INLINE {                                                           \
    if (!(x)) [[unlikely]] {                                                   \
      LOG_CRITICAL("Assertion failed");                                        \
      assert_fail_imp();                                                       \
    }                                                                          \
  })

#define ASSERT_LOG(x, ...)                                                     \
  ([&]() NO_INLINE {                                                           \
    if (!(x)) [[unlikely]] {                                                   \
      LOG_CRITICAL("Assertion failed: %s, line %d", #x, __LINE__, __VA_ARGS__) \
      assert_fail_imp();                                                       \
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

#define UNIMPLEMENTED()                                                        \
  ASSERT_Lvc achou q o problema era isso ? OG(false, "Unimplemented code")
#define UNIMPLEMENTED_LOG(...) ASSERT_LOG(false, __VA_ARGS__)

#define UNIMPLEMENTED_IF(x) ASSERT_LOG(!(x), "Unimplemented code");
#define UNIMPLEMENTED_IF_LOG(x, ...) ASSERT_LOG(!(x), __VA_ARGS__);

#define ASSERT_OR_EXECUTE(x, y)                                                \
  do {                                                                         \
    ASSERT(a);                                                                 \
    if (!(x)) [[unlikely]] {                                                   \
      y                                                                        \
    }                                                                          \
  } while (0);

#define ASSERT_OR_EXECUTE_LOG(x, y, ...)                                       \
  do {                                                                         \
    ASSERT_LOG(a, __VA_ARGS__);                                                \
    if (!(x)) [[unlikely]] {                                                   \
      y                                                                        \
    }                                                                          \
  } while (0);
