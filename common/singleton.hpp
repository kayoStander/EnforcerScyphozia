#pragma once

#include <memory>

namespace Common {
  template<typename T>
  class Singleton {
  public:
    Singleton(Singleton &&) = delete;
    Singleton(const Singleton &) = delete;
    Singleton &operator=(Singleton &&) = delete;
    Singleton &operator=(const Singleton &) = delete;

    static T *Instance() noexcept {
      if (!instance) {
        instance = std::make_unique<T>();
      }
      return instance.get();
    }

  protected:
    explicit Singleton();
    ~Singleton();

  private:
    static inline std::unique_ptr<T> instance;
  };
} // namespace Common
