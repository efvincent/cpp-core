#pragma once

/**
 * @file core_debug.h
 * @brief Assertion macros used by base modules.
 * @note Assertions are enabled in debug/instrument builds and compiled out in release.
 */

/**
 * @def NON_COPYABLE(TypeName)
 * @brief Suppresses copy constructors but allows move constructors.
 * @param TypeName The name of the struct or class being modified.
 * @note Use this for mutable containers (e.g., Arraylist, Hash Map). These 
 * containers hold pointers to buffers that can reallocate during mutation. 
 * Banning copies prevents severe dangling-pointer desync bugs caused by 
 * shallow copying. Move operations are defaulted to allow returning newly 
 * created containers from functions by value.
 */
#define NON_COPYABLE(TypeName) \
    TypeName(const TypeName&) = delete; \
    TypeName& operator=(const TypeName&) = delete; \
    TypeName(TypeName&&) = default; \
    TypeName& operator=(TypeName&&) = default;

/**
 * @def NON_COPYABLE_NOR_MOVABLE(TypeName)
 * @brief Suppresses both copy and move constructors entirely.
 * @param TypeName The name of the struct or class being modified.
 * @note Use this for root memory structures (e.g., Arena, Memory Pool) or 
 * system resources. These objects act as the strict source of truth for 
 * memory blocks or handles. They must be pinned in memory and passed strictly 
 * by pointer or reference to prevent lifetime confusion or identity loss.
 */
#define NON_COPYABLE_NOR_MOVABLE(TypeName) \
    TypeName(const TypeName&) = delete; \
    TypeName& operator=(const TypeName&) = delete; \
    TypeName(TypeName&&) = delete; \
    TypeName& operator=(TypeName&&) = delete;

#if defined(BUILD_DEBUG) || defined(BUILD_INSTRUMENT)
  #if defined(_MSC_VER)
    /**
     * @def BASE_ASSERT(expression)
     * @brief Terminates immediately when \p expression evaluates to false.
     * @param expression Boolean condition that must hold.
     * @note MSVC path uses __debugbreak().
     */
    #define BASE_ASSERT(expression) if(!(expression)) { __debugbreak(); }
  #else
    /**
     * @def BASE_ASSERT(expression)
     * @brief Terminates immediately when \p expression evaluates to false.
     * @param expression Boolean condition that must hold.
     * @note Clang/GCC path uses __builtin_trap().
     */
    #define BASE_ASSERT(expression) if(!(expression)) { __builtin_trap(); }
  #endif
#else
  /**
   * @def BASE_ASSERT(expression)
   * @brief No-op assertion macro for non-debug builds.
   * @param expression Ignored.
   */
  #define BASE_ASSERT(expression)
#endif