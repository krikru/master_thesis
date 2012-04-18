#ifndef  DEFINITIONS_H
#define  DEFINITIONS_H

////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
// COMPILER DEFINITIONS
////////////////////////////////////////////////////////////////

//#define WIN32_LEAN_AND_MEAN

////////////////////////////////////////////////////////////////
// COMPILE TIME OPTIONS
////////////////////////////////////////////////////////////////

//#define  DEBUG                      0
#define  DEBUG                      1
#define  ELABORATE                  0
#define  DRAW_SMOOTH_LINES          0
#define  DRAW_CELL_CUBES            0
#define  DRAW_PARENT_CELLS          0
#define  DRAW_NEIGHBOR_CONNECTIONS  1

////////////////////////////////////////////////////////////////
// MISCELANEOUS DEFINITIONS
////////////////////////////////////////////////////////////////

#define  USE_DOUBLE_PRECISION_FOR_PHYSICS      0

////////////////////////////////////////////////////////////////
// TYPEDEFS
////////////////////////////////////////////////////////////////

typedef  unsigned int            uint ;
#if USE_DOUBLE_PRECISION_FOR_PHYSICS
typedef  double                  pftype;
#else
typedef  float                   pftype;
#endif

////////////////////////////////////////////////////////////////
// ENUMS
////////////////////////////////////////////////////////////////

//TODO: The real order should be X, Y, Z (change back if it isn't)
#if ELABORATE
enum DIRECTION {
    DIR_Y,
    DIR_X,
    DIR_Z,
    NUM_DIRECTIONS
};
#else
enum DIRECTION {
    DIR_X,
    DIR_Y,
    DIR_Z,
    NUM_DIRECTIONS
};
#endif

//TODO: Make this work
#if (DIR_X == 0) && (DIR_Y == 1) && (DIR_Z == 2)
#define  XYZ_COORDINATE_SYSTEM  1
#else
#define  XYZ_COORDINATE_SYSTEM  0
#endif

////////////////////////////////////////////////////////////////
// CONSTANTS
////////////////////////////////////////////////////////////////

/* Mathematical constants */
#ifndef _MATH_H_
#define M_E         2.7182818284590452354  // Euler's number
#define M_LOG2E		1.4426950408889634074  // log_2(e)
#define M_LOG10E	0.43429448190325182765 // log_10(e)
#define M_LN2		0.69314718055994530942 // ln(2)
#define M_LN10		2.30258509299404568402 // ln(10)
#define M_PI		3.14159265358979323846 // pi
#define M_PI_2		1.57079632679489661923 // pi/2
#define M_PI_4		0.78539816339744830962 // pi/4
#define M_1_PI		0.31830988618379067154 // 1/pi
#define M_2_PI		0.63661977236758134308 // 2/pi
#define M_2_SQRTPI	1.12837916709551257390 // 2/sqrt(pi)
#define M_SQRT2		1.41421356237309504880 // sqrt(2)
#define M_SQRT1_2	0.70710678118654752440 // sqrt(1/2)
#endif

/* Physical constants */
#define P_G         9.82000000000000000000 // [m/s^2] Gravitational acceleration

////////////////////////////////////////////////////////////////
// MACROS
////////////////////////////////////////////////////////////////

// TODO: Make some of these inline functions instead

#define  MIN(x, y)  ((y) < (x) ? (y) : (x))
#define  MAX(x, y)  ((y) > (x) ? (y) : (x))

#define  TEMP_SWAP(x, y, temp) { \
    (temp) = (x);                \
    (x) = (y);                   \
    (y) = (temp);                \
    }

#if 0
#define  XOR_SWAP(x, y) { \
    (x) ^= (y)            \
    (y) ^= (x)            \
    (x) ^= (y)            \
    }
#endif

#define  BEGIN_TAKE_TIME(action)              \
    if (DEBUG) {                              \
        cout << (action) << endl;             \
        t1 = (double)clock()/CLOCKS_PER_SEC;  \
    }

#define  END_TAKE_TIME()                                          \
    if (DEBUG) {                                                  \
        t2 = (double)clock()/CLOCKS_PER_SEC;                      \
        cout << "Took " << t2-t1 << " seconds." << endl << endl;  \
    }

#endif  /* DEFINITIONS_H */
