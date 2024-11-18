#pragma once

#include <cstdint>
#include <stdfloat>
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

#ifdef __x86_64
using s128 = __int128_t;
using u128 = __uint128_t;
#else
#warning Not implemented for this architecture
using s128 = std::intmax_t;
using u128 = std::uintmax_t;
#endif

using f16 = std::float16_t;
using f32 = std::float32_t;
using f64 = std::float64_t;
using f128 = std::float128_t;

/*
using rot = glm::vec3;
using pos = glm::vec3;

using mat2 = glm::mat2;
using mat3 = glm::mat3;
using mat4 = glm::mat4;
*/
template<typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

template<typename T>
concept Floating = std::is_floating_point_v<T>;

template<typename T>
concept Integral = std::is_integral_v<T>;
