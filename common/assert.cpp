#include "assert.hpp"
#include <cstdio>

#ifdef ARCH_X86_64
#define CRASH() __asm__ __volatile__("int $3")
#elif defined(ARCH_ARM64)
#define CRASH() __asm__ __volatile__("brk 0")
#else
#define CRASH()                                                                \
  do {                                                                         \
    LOG_CRITICAL(Core, "Crash is not supported in this structure");            \
    __asm__ __volatile__("int $3");                                            \
    std::abort();                                                              \
  } while (0)
#endif

void assert_fail_impl() {
  std::fflush(stdout);
  CRASH();
  __builtin_unreachable();
}

[[noreturn]] inline void unreachable_impl() {
  std::fflush(stdout);
  CRASH();
  __builtin_unreachable();
}
