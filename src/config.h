
#ifndef CONFIG_H
#define CONFIG_H
#pragma once

// Platform
#ifdef _WIN32
#define EENG_PLATFORM_WINDOWS
#else
#define EENG_PLATFORM_UNIX
#define EENG_PLATFORM_LINUX
#define XI_PLATFORM_APPLE
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

// GL version
#ifdef XI_PLATFORM_APPLE
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

#endif
