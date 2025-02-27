#pragma once
#include <type_traits>

template <typename T> inline constexpr
int Sign(T x, std::false_type is_signed) {
    return T(0) < x;
}

template <typename T> inline constexpr
int Sign(T x, std::true_type is_signed) {
    return (T(0) < x) - (x < T(0));
}

template <typename T> inline constexpr
int Sign(T x) {
    return Sign(x, std::is_signed<T>());
}