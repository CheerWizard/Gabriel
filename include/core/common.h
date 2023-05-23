#pragma once

#define null nullptr

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#define STR(s1, s2) s1 ## s2

constexpr glm::vec4 COLOR_CLEAR = { 0, 0, 0, 0 };
constexpr glm::vec4 COLOR_ZERO = { 0, 0, 0, 0 };
constexpr glm::vec4 COLOR_ONE = { 1, 1, 1, 1 };