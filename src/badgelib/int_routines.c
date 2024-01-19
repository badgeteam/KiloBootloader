
// SPDX-License-Identifier: MIT

// This file contains definitions for all GCC routines for integer arithmetic with the exception of int bit shifts.

#pragma GCC optimize("O2")

#include <stdbool.h>


typedef int          si_t __attribute__((mode(SI)));
typedef unsigned int usi_t __attribute__((mode(SI)));
typedef int          di_t __attribute__((mode(DI)));
typedef unsigned int udi_t __attribute__((mode(DI)));
#if __SIZE_MAX__ > 0xffffffff
#define do_ti_math
typedef int          ti_t __attribute__((mode(TI)));
typedef unsigned int uti_t __attribute__((mode(TI)));
#endif


// Implement division and modulo for an unsigned type.
// Does not do any edge case checks; divisor must never be 0.
#define DIVMOD_IMPL(type, name)                                                                                        \
    typedef struct {                                                                                                   \
        u##type remainder;                                                                                             \
        u##type result;                                                                                                \
    } divmod_##name##_t;                                                                                               \
    static divmod_##name##_t divmod_##name(u##type remainder, u##type divisor) {                                       \
        u##type const msb    = 1ull << (sizeof(type) * 8 - 1);                                                         \
        u##type       result = 0;                                                                                      \
        unsigned int  shift  = 0;                                                                                      \
        while (!(divisor & msb)) {                                                                                     \
            divisor <<= 1;                                                                                             \
            shift++;                                                                                                   \
        }                                                                                                              \
        for (unsigned int i = 0; i <= shift; i++) {                                                                    \
            if (remainder >= divisor) {                                                                                \
                result    |= 1ull << (shift - i);                                                                      \
                remainder -= divisor;                                                                                  \
            }                                                                                                          \
            divisor >>= 1;                                                                                             \
        }                                                                                                              \
        return (divmod_##name##_t){remainder, result};                                                                 \
    }

// Division implementations.
DIVMOD_IMPL(si_t, si)
DIVMOD_IMPL(di_t, di)
#ifdef do_ti_math
DIVMOD_IMPL(ti_t, ti)
#endif

// Generate all division functions for a type.
#define DIVMOD_FUNCS(type, name)                                                                                       \
    u##type __udivmod##name##4(u##type a, u##type b, u##type * rem) {                                                  \
        if (b == 0) {                                                                                                  \
            *rem = a;                                                                                                  \
            return -1;                                                                                                 \
        }                                                                                                              \
        divmod_##name##_t res = divmod_##name(a, b);                                                                   \
        *rem                  = res.remainder;                                                                         \
        return res.result;                                                                                             \
    }                                                                                                                  \
    u##type __udiv##name##3(u##type a, u##type b) {                                                                    \
        if (b == 0)                                                                                                    \
            return -1;                                                                                                 \
        return divmod_##name(a, b).result;                                                                             \
    }                                                                                                                  \
    u##type __umod##name##3(u##type a, u##type b) {                                                                    \
        if (b == 0)                                                                                                    \
            return a;                                                                                                  \
        else                                                                                                           \
            return divmod_##name(a, b).remainder;                                                                      \
    }                                                                                                                  \
    type __divmod##name##4(type a, type b, type * rem) {                                                               \
        if (b == 0) {                                                                                                  \
            *rem = a;                                                                                                  \
            return -1;                                                                                                 \
        }                                                                                                              \
        bool              div_sign = (a < 0) ^ (b < 0);                                                                \
        bool              mod_sign = a < b;                                                                            \
        divmod_##name##_t res      = divmod_##name(a, b);                                                              \
        *rem                       = mod_sign ? -res.remainder : res.remainder;                                        \
        return div_sign ? -res.result : res.result;                                                                    \
    }                                                                                                                  \
    type __div##name##3(type a, type b) {                                                                              \
        if (b == 0)                                                                                                    \
            return -1;                                                                                                 \
        bool sign = (a < 0) ^ (b < 0);                                                                                 \
        if (a < 0)                                                                                                     \
            a = -a;                                                                                                    \
        if (b < 0)                                                                                                     \
            b = -b;                                                                                                    \
        divmod_##name##_t res = divmod_##name(a, b);                                                                   \
        if (sign)                                                                                                      \
            return -res.result;                                                                                        \
        else                                                                                                           \
            return res.result;                                                                                         \
    }                                                                                                                  \
    type __mod##name##3(type a, type b) {                                                                              \
        if (b == 0)                                                                                                    \
            return a;                                                                                                  \
        bool sign = a < 0;                                                                                             \
        if (a < 0)                                                                                                     \
            a = -a;                                                                                                    \
        if (b < 0)                                                                                                     \
            b = -b;                                                                                                    \
        divmod_##name##_t res = divmod_##name(a, b);                                                                   \
        if (sign)                                                                                                      \
            return -res.remainder;                                                                                     \
        else                                                                                                           \
            return res.remainder;                                                                                      \
    }

DIVMOD_FUNCS(si_t, si)
DIVMOD_FUNCS(di_t, di)
#ifdef do_ti_math
DIVMOD_FUNCS(ti_t, ti)
#endif


// Fake integer operations by letting GCC write them all out itself.
// This file is O2 optimised, so it should emit assembly for all of these functions.
#define FAKE_OPER(type, name, oper)                                                                                    \
    type name(type a, type b) {                                                                                        \
        return a oper(b & (sizeof(type) * 8 - 1));                                                                     \
    }

FAKE_OPER(si_t, __ashlsi3, <<)
FAKE_OPER(si_t, __ashrsi3, >>)
FAKE_OPER(si_t, __lshrsi3, >>)
FAKE_OPER(di_t, __ashldi3, <<)
FAKE_OPER(di_t, __ashrdi3, >>)
FAKE_OPER(di_t, __lshrdi3, >>)
#ifdef do_ti_math
FAKE_OPER(ti_t, __ashlti3, <<)
FAKE_OPER(ti_t, __ashrti3, >>)
FAKE_OPER(ti_t, __lshrti3, >>)
#endif

// Explicit implementation of integer multiply for the larger versions to rely upon if required.
int __mulsi3(int a, int b) {
    int res = 0;
    for (unsigned int i = 0; i < sizeof(int) * 8; i++) {
        if (b & 1) {
            res += a;
        }
        b >>= 1;
        a <<= 1;
    }
    return res;
}

FAKE_OPER(di_t, __muldi3, *)
#ifdef do_ti_math
FAKE_OPER(ti_t, __multi3, *)
#endif
