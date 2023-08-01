#ifndef LIBSTRA_CONFIG_H
#define LIBSTRA_CONFIG_H

#if __cplusplus < 202002L
#define _unlikely
#else
#define _unlikely [[unlikely]]
#endif

#if __cplusplus >= 202302L
#define _assume(expr) [[assume(expr)]]
#elif defined(_MSC_VER) && !defined(__clang__)
#define _assume(expr) __assume(expr)
#elif defined(__clang__)
#define _assume(expr) __builtin_assume(expr)
#elif defined(__GNUC__)
#define _assume(expr)                                                          \
	if (!(expr)) __builtin_unreachable()
#else
#define _assume(exp)
#endif

#endif