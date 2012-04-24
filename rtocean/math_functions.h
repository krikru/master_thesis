#ifndef MATH_FUNCTIONS_H
#define MATH_FUNCTIONS_H

////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

// Standard includes
#include <cstdlib>
#include <cmath>
#include <limits>

// Own include
//#include "compile_time.h"

////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////

#if  0
/* Integer uniform distribution */
template<typename T>
inline T integer_uniform(T a, T b)
{
    STATIC_ASSERT(std::numeric_limits<T>::is_integer);
    /* STATIC_ASSERT: The arguments must be of be an integer type. */
    return a + rand() % (b + 1 - a);
}

/* Continuous uniform distribution */
template<typename T>
inline T uniform(T a, T b)
{
    STATIC_ASSERT(!std::numeric_limits<T>::is_integer);
    /* STATIC_ASSERT: The arguments must be of be a float type. */
    return a + rand()/(RAND_MAX + T(1)) * (b - a);
}
template<typename T>
inline T uniform(int a, T b)
{
    STATIC_ASSERT(!std::numeric_limits<T>::is_integer);
    return a + rand()/(RAND_MAX + T(1)) * (b - a);
}
template<typename T>
inline T uniform(T a, int b)
{
    STATIC_ASSERT(!std::numeric_limits<T>::is_integer);
    return a + rand()/(RAND_MAX + T(1)) * (b - a);
}
#else
/*
 * Integer uniform distribution
 */
inline int integer_uniform(int a, int b)
{
    return a + rand() % (b + 1 - a);
}

/*
 * Continuous uniform distribution
 */
inline float uniform(float a, float b)
{
    return a + rand()/(RAND_MAX + float(1)) * (b - a);
}

inline double uniform(double a, double b)
{
    return a + rand()/(RAND_MAX + double(1)) * (b - a);
}
#endif

////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////

/* Powers */
template<typename T>
inline
T inline_int_pow(T base, int e)
{
    // Cover all the possible values of the exponent
    if (e > 1) {
        T a = inline_int_pow(base, e >> 1);
        if (e & 1) {
            return a*a*base;
        }
        else {
            return a*a;
        }
    }
    else if (e == 1) {
        return base;
    }
    else if (e < 0) {
        return 1/inline_int_pow(base, -e);
    }
    else {
        /* e is zero (will not happen if inline_int_pow is the caller) */
        return 1;
    }
}

#endif // MATH_FUNCTIONS_H
