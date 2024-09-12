#pragma once

#include <memory>

namespace Common {
template <typename T> class Singleton {
public:
  static T *Instance() noexcept {
    if (!instance) {
      instance = std::make_unique<T>();
    }
    return instance.get();
  }

protected:
  Singleton();
  ~Singleton();

private:
  static inline std::unique_ptr<T> instance;
};
} // namespace Common
