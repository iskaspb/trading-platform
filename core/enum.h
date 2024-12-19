#pragma once

// original source: https://www.scs.stanford.edu/~dm/blog/va-opt.html
#include <string_view>

#define PARENS ()

// Rescan macro tokens 256 times
#define EXPAND(arg) EXPAND1(EXPAND1(EXPAND1(EXPAND1(arg))))
#define EXPAND1(arg) EXPAND2(EXPAND2(EXPAND2(EXPAND2(arg))))
#define EXPAND2(arg) EXPAND3(EXPAND3(EXPAND3(EXPAND3(arg))))
#define EXPAND3(arg) EXPAND4(EXPAND4(EXPAND4(EXPAND4(arg))))
#define EXPAND4(arg) arg

#define FOR_EACH(macro, ...) __VA_OPT__(EXPAND(FOR_EACH_HELPER(macro, __VA_ARGS__)))
#define FOR_EACH_HELPER(macro, a1, ...) macro(a1) __VA_OPT__(FOR_EACH_AGAIN PARENS(macro, __VA_ARGS__))
#define FOR_EACH_AGAIN() FOR_EACH_HELPER

#define ENUM_CASE(name)                                                                                                \
    case name:                                                                                                         \
        return #name##sv;

#define ENUM(type, ...)                                                                                                \
    enum type                                                                                                          \
    {                                                                                                                  \
        __VA_ARGS__                                                                                                    \
    };                                                                                                                 \
    ENUM_TO_STRING(type, __VA_ARGS__)

#define ENUM_CLASS(type, ...)                                                                                          \
    enum class type                                                                                                    \
    {                                                                                                                  \
        __VA_ARGS__                                                                                                    \
    };                                                                                                                 \
    ENUM_TO_STRING(type, __VA_ARGS__)

#define ENUM_TO_STRING(type, ...)                                                                                      \
    constexpr std::string_view toString(type _e) noexcept                                                              \
    {                                                                                                                  \
        using enum type;                                                                                               \
        using namespace std::string_view_literals;                                                                     \
        switch (_e)                                                                                                    \
        {                                                                                                              \
            FOR_EACH(ENUM_CASE, __VA_ARGS__)                                                                           \
        default:                                                                                                       \
            return #type "::<unknown>";                                                                                \
        }                                                                                                              \
    }
