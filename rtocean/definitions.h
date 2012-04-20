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
/* Essential */
#define  DEBUG                      1
#define  RUN_SAFE                   1
#define  NUM_DIMENSIONS             3 /* 2 or 3 */
#define  NUM_DIRECTIONS             (2*NUM_DIMENSIONS)
#define  LOGICAL_AXIS_ORDER         1
#define  GENERATE_NEIGHBORS_STATICALLY   0
#define  GENERATE_NEIGHBORS_DYNAMICALLY  1

/* Program speed */
#define  FRAME_MS                   (1000/60)

/* VIsualization */
#define  TEST_DEPTH                 1
#define  DRAW_CHILD_CELLS_FIRST_IF_DEPTH_TESTING  1
#define  DRAW_SMOOTH_LINES          0
#define  DRAW_CELL_CUBES            1
#define  DRAW_PARENT_CELLS          1
#define  DRAW_NEIGHBOR_CONNECTIONS  1
#define  MARK_MIDDLE_OF_CONNECTION  0
#define  MIDDLE_MARK_SIZE           0.05
#define  PARENT_CUBE_DIST_SCALEING  1.0001

/* Precision */
#define  USE_DOUBLE_PRECISION_FOR_PHYSICS  0

/* Graphics */
const float  BACKGROUND_R           = 1;
const float  BACKGROUND_G           = 1;
const float  BACKGROUND_B           = 1;
const float  BACKGROUND_A           = 1;
const float  LEAF_CUBE_R            = 0;
const float  LEAF_CUBE_G            = 0;
const float  LEAF_CUBE_B            = 0;
const float  LEAF_CUBE_A            = 1;
const float  PARENT_CUBE_R          = .75;
const float  PARENT_CUBE_G          = .75;
const float  PARENT_CUBE_B          = .75;
const float  PARENT_CUBE_A          = 1;
const float  NEIGHBOR_CONNECTION_R  = 0;
const float  NEIGHBOR_CONNECTION_G  = 0;
const float  NEIGHBOR_CONNECTION_B  = 1;
const float  NEIGHBOR_CONNECTION_A  = 1;
const float  MIDDLE_MARK_R          = 1;
const float  MIDDLE_MARK_G          = 0;
const float  MIDDLE_MARK_B          = 0;
const float  MIDDLE_MARK_A          = 1;

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

//TODO: Generalize the code so it works for 1, 2 and 3 dimensions
//TODO: The real order should be X, Y, Z (change back if it isn't)
#if LOGICAL_AXIS_ORDER
enum DIMENSION {
    DIM_X,
    DIM_Y,
    DIM_Z
};
#else
enum DIMENSION {
    DIM_Y,
    DIM_X,
    DIM_Z
};
#endif

#if    NUM_DIMENSIONS == 2
#define  HORIZONTAL_DIMENSION1  DIM_X
#define  UP_DIMENSION           DIM_Y
#elif  NUM_DIMENSIONS == 3
#define  HORIZONTAL_DIMENSION1  DIM_X
#define  HORIZONTAL_DIMENSION2  DIM_Y
#define  UP_DIMENSION           DIM_Z
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
