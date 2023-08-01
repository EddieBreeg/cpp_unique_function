#ifndef LIBSTRA_CONFIG_H
#define LIBSTRA_CONFIG_H

#if __cplusplus < 202002L
#define _unlikely
#else
#define _unlikely [[unlikely]]
#endif

#if __cplusplus >= 202302L
#define _assume(expr) [[assume(expr)]]
#else
#define _assume(exp)
#endif

#endif