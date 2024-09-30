#pragma once

#include <cstdint>
#include <type_traits>

#if __has_include(<glm/glm.hpp>)
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#else
#error "No GLM recognized in the system"
#endif

using s8 = std::int8_t;
using s16 = std::int16_t;
using s32 = std::int32_t;
using s64 = std::int64_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using f32 = float;
using f64 = double;

using rot = glm::vec3;
using pos = glm::vec3;

template <typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

template <typename T>
concept Floating = std::is_floating_point_v<T>;

template <typename T>
concept Integral = std::is_integral_v<T>;
