#include "assert.hpp"
#include <cstdio>
#include <stdexcept>

#if defined(ARCH_X86_64)
#define CRASH() __asm__ __volatile__("int %3")
#elif defined(ARCH_ARM64)
#define CRASH() __asm__ __volatile__("brk 0")
#else
#define CRASH()                                                                \
  do {                                                                         \
    fprintf(stderr, "CRASH() not implemented on this architecture");           \
    std::abort();                                                              \
  } while (0)
#endif

void assert_fail_impl() {
  std::fflush(stdout);
  CRASH();
}

[[noreturn]] void unreachable_impl() {
  std::fflush(stdout);
  CRASH();
  throw std::runtime_error("Unreachable code");
}

/*void assert_fail_debug_log(const char *__restrict msg) {
  LOG_CRITICAL("Assertion failed\n{}", msg);
  assert_fail_imp();
} FIX */
