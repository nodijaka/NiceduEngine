
#ifndef CONFIG_H
#define CONFIG_H
#pragma once

#include <iostream>
#include <string_view>
#include <format>

// Misc global defines
#define EENG_NULL_INDEX -1

// Platform
#ifdef _WIN32
#define EENG_PLATFORM_WINDOWS
#else
#define EENG_PLATFORM_UNIX
#define EENG_PLATFORM_LINUX
#define EENG_PLATFORM_APPLE
#endif

// Compiler
#ifdef _MSC_VER
#define EENG_COMPILER_MSVC
#endif
#ifdef __clang__
#define EENG_COMPILER_CLANG
#endif
#ifdef __GNUC__
#define EENG_COMPILER_GCC
#endif

// Debug
#if not defined(NDEBUG) or defined(_DEBUG)
#define EENG_DEBUG
#define EENG_ENABLE_ASSERTS
#endif

// Debug break
#ifdef EENG_PLATFORM_WINDOWS
#define EENG_DEBUG_BREAK() __debugbreak()
#else
#if defined(__ARM_ARCH) || defined(__aarch64__)
#define EENG_DEBUG_BREAK() __asm__ volatile("svc #0")
#elif defined(__x86_64__) || defined(__i386__)
#define EENG_DEBUG_BREAK() __asm__ volatile("int $0x03")
#else
#error "Unsupported architecture"
#endif
#endif

// Assert
#ifdef EENG_ENABLE_ASSERTS
template <class... Args>
static void EENG_ERROR(std::string_view fmt, Args &&...args)
{
    auto msg = std::vformat(fmt, std::make_format_args(args...));
    std::cerr << "Error: " << msg << std::endl;
}
#define EENG_ASSERT(x, ...)          \
    {                                \
        if (!(x))                    \
        {                            \
            EENG_ERROR(__VA_ARGS__); \
            EENG_DEBUG_BREAK();      \
        }                            \
    }
#else
#define EENG_ASSERT(x, ...)
#endif

// GL version
#ifdef EENG_PLATFORM_APPLE
#define EENG_GLVERSION_MAJOR 4
#define EENG_GLVERSION_MINOR 1
#else
#define EENG_GLVERSION_MAJOR 4
#define EENG_GLVERSION_MINOR 3
#endif
//
#if EENG_GLVERSION_MAJOR >= 4
#if EENG_GLVERSION_MINOR >= 1
#define EENG_GLVERSION_41
#endif
#if EENG_GLVERSION_MINOR >= 3
#define EENG_GLVERSION_43
#endif
#endif
#ifndef EENG_GLVERSION_41
static_assert(false, "OpenGL 4.1 is required");
#endif

/// Convert an enum class to its underlying type
template<typename E>
constexpr auto to_integral(E e) -> typename std::underlying_type<E>::type
{
   return static_cast<typename std::underlying_type<E>::type>(e);
}

#endif
