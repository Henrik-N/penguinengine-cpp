#pragma once

#include <stdexcept>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using f32 = float;
using f64 = double;

using usize = size_t;

// Use as much as possible to avoid unnessary heap allocations.
using stringv = std::string_view;
using u8stringv = std::u8string_view;

#include <optional>

template <typename T>
using Option = std::optional<T>;

// template <typename T>
//typedef typename std::optiona
// using template <typename T> Option<T> = std::optional<T>;

// todo fixed string
// template <usize N>
// struct String {
//     char data[N + 1]{};
//     constexpr String(const char(&str)[N + 1]) {
//         std::copy_n(str, N + 1, data);
//     }
//     auto operator<=>(const String&) const = default;
// };
// template <usize N>
// String(const char(&str)[N])->String<N - 1>;




// platform detection
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define PENGUIN_PLATFORM_WINDOWS 1 
#ifndef _WIN64
#error "Only 64-bit is supported on Windows"
#endif
#elif defined(__linux__) || defined(__gnu_linux__)
#define PENGUIN_PLATFORM_LINUX 1
#if defined(__ANDROID__)
#define PENGUIN_PLATFORM_ANDROID 1
#endif
#else
#error "unsupported platform"
#endif

#ifdef PENGUIN_EXPORT
    // export as dll/so
#ifdef _MSC_VER 
#define PENGUIN_API __declspec(dllexport)
#else
#define PENGUIN_API __attribute__((visibility("default")))
#endif

// import from dll/so
#else
#ifdef _MSC_VER 
#define PENGUIN_API __declspec(dllimport)
#else
#define PENGUIN_API
#endif
#endif

// renderer used
#define PENGUIN_RENDERER_VULKAN


// always returns okaay
#define OK noexcept

