#ifndef  DEFINITIONS_H
#define  DEFINITIONS_H

////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

/* Standard includes */
#include <stdint.h>

/* Own includes */
#include "naninit.h"
#include "mustinit.h"

////////////////////////////////////////////////////////////////
// COMPILER DEFINITIONS
////////////////////////////////////////////////////////////////

//#define WIN32_LEAN_AND_MEAN

////////////////////////////////////////////////////////////////
// COMPILE TIME OPTIONS
////////////////////////////////////////////////////////////////

/* Essential */
#define  DEBUG                      0
#define  INITIALIZE_FLOATS_TO_NAN                   1 // Weaker
#define  CHECK_INITIALIZATION_OF_FLOATS             0 // Stronger
#define  NUM_DIMENSIONS             2 /* 2 or 3 */
//#define  NUM_DIRECTIONS             (2*NUM_DIMENSIONS)
#define  LOGICAL_AXIS_ORDER         1
#define  NO_ATMOSPHERE              0
#define  VACUUM_HAS_PRESSURE        0
#define  ALLOW_NEGATIVE_PRESSURES   0
#define  INTERPOLATE_SURFACE_PRESSURE               0
#define  COMPRESS_INTERFACE_VERTICALLY              1
#define  TIME_STEP_CHANGE_CORRECTION                1 // Weaker
#define  COURANT_NUMBER_LIMITATION                  1 // Stronger

/* Precision */
#define  USE_DOUBLE_PRECISION_FOR_PHYSICS           1

/* Advection scheme */
#define  NO_SCHEME                  0
#define  UPWIND                     1
//#define  HRIC                       2
#define  HYPER_C                    3
//#define  HIGH_CONTRAST_SCHEME       4

//#define  ALPHA_ADVECTION_SCHEME     UPWIND
//#define  ALPHA_ADVECTION_SCHEME     HRIC
#define  ALPHA_ADVECTION_SCHEME     HYPER_C
//#define  ALPHA_ADVECTION_SCHEME     HIGH_CONTRAST_SCHEME

/* Simulation parameters */
//#define  FRAME_MS                   150 // [ms]
//#define  FRAME_MS                   (1000/60) // [ms]
#define  FRAME_MS                   0 // [ms]
//#define  NUM_TIME_STEPS_PER_FRAME   1 // [1]
#define  NUM_TIME_STEPS_PER_FRAME   3 // [1]
//#define  NUM_TIME_STEPS_PER_FRAME   10 // [1]
//#define  SIMULATION_TIME_STEP       (FRAME_MS/1000.0) // [s]
//#define  SIMULATION_TIME_STEP       .01 // [s]
//#define  SIMULATION_TIME_STEP       .001 // [s]
#define  SIMULATION_TIME_STEP       .0003 // [s]
//#define  SIMULATION_TIME_STEP       .0001 // [s]
//#define  SIMULATION_TIME_STEP       .000075 // [s]
//#define  SIMULATION_TIME_STEP       .00003 // [s]
//#define  SIMULATION_TIME_STEP       .00001 // [s]
//#define  SIMULATION_TIME_STEP       .000003 // [s]
//#define  SIMULATION_TIME_STEP       .0 // [s]
#define  MAX_RECOMMENDED_V          .9
#define  MAX_ALLOWED_V              10

/* Grid */
#define  MIN_LOD_LAYER_THICKNESS    1    // [Number of cells]
#define  SURFACE_HEIGHT             0.65 // [m]
#define  SURFACE_ACCURACY           0.02 // [m] Maximum size of the surface cells
//#define  SURFACE_ACCURACY           0.01 // [m] Maximum size of the surface cells
//#define  SURFACE_ACCURACY           0.005 // [m] Maximum size of the surface cells

/* Navier-Stokes */
#define  USE_ARTIFICIAL_COMPRESSIBILITY              1
/* 122.92: Works; 122.93: Doesn't work. (dt = 0.001, maximal spatial resolution = 0.02) */
#define  ARTIFICIAL_COMPRESSIBILITY_FACTOR           (10.00 * NORMAL_WATER_DENSITY) // [Pa] (Delta pressure = ARTIFICIAL_COMPRESSIBILITY_FACTOR * Delta water volume coefficient)
//#define  NORMAL_AIR_PRESSURE                         (0.01 * (NO_ATMOSPHERE ? 0.0 : P_1ATM))
#define  NORMAL_AIR_PRESSURE                         (0.001 * (NO_ATMOSPHERE ? 0.0 : P_1ATM))
#define  NORMAL_AIR_DENSITY                          P_AIR_DENSITY_AT_1_ATM
#define  AIR_COMPRESSIBILITY_FACTOR                  (NORMAL_AIR_PRESSURE/NORMAL_AIR_DENSITY)
#define  NORMAL_WATER_DENSITY                        P_WATER_DENSITY_AT_1_ATM

/* VIsualization */
//#define  DEFAULT_SCALAR_PROPERTY_TO_VISUALIZE        SP_ALPHA
#define  DEFAULT_SCALAR_PROPERTY_TO_VISUALIZE        SP_WATER_VOLUME_COEFFICIENT
#define  TEST_DEPTH                 1
#define  DRAW_CHILD_CELLS_FIRST_IF_DEPTH_TESTING     1
#define  DRAW_SMOOTH_LINES          0
#define  LINE_WIDTH                 (DRAW_SMOOTH_LINES ? 1.5 : 1)
#define  INITIAL_PGF_LINE_WIDTH    "0.4pt" // From the PGF manual
#define  CELL_MARK_LINE_WIDTH       3
#define  VELOCITY_LINE_WIDTH        LINE_WIDTH
#define  NUM_LINES_IN_CIRCLES       16
#define  MARK_CELLS                 0
#define  VEL_DIV_SCALE_FACTOR       0.1 // [s]
//#define  VEL_DIV_SCALE_FACTOR       SIMULATION_TIME_STEP // [s]
#define  FLOW_DIV_SCALE_FACTOR      VEL_DIV_SCALE_FACTOR // [s]
#define  DRAW_CELL_CENTER_VELOCITIES                 0
#define  DRAW_CELL_FACE_VELOCITIES                   1
#define  DRAW_ALL_VELOCITIES                         1 // Draw velocities for all cells
//#define  VEL_TO_ARROW_LENGTH_FACTOR                  0.1 // [s]
#define  VEL_TO_ARROW_LENGTH_FACTOR                  (1*SIMULATION_TIME_STEP) // [s]
#define  DRAW_CELL_CUBES            1
#define  DRAW_PARENT_CELLS          1
#define  DRAW_ONLY_SURFACE_CELLS    0
#define  DRAW_WATER_LEVEL           0
#define  DRAW_NEIGHBOR_CONNECTIONS  0
#define  VISUALIZE_ONLY_FINEST_NEIGHBOR_CONNECTIONS  1
#define  MARK_MIDDLE_OF_CONNECTION  0
#define  MIDDLE_MARK_SIZE           0.05
#define  RANDOMIZE_NEIGHBOR_CONNECTION_MIDPOINTS     0
#define  NEIGHBOR_CONNECTION_MIDPOINT_RANDOMIZATION  0.1
/* Scaling to prevent the same z-value */
#define  SCALE_FACTOR               1.0003
#define  SCALAR_PROPERTIES_SCALING  (SCALE_FACTOR * SCALE_FACTOR)
#define  PARENT_CUBE_DIST_SCALING   SCALE_FACTOR
#define  LEAF_CUBE_DIST_SCALING     1
#define  CELL_MARK_DIST_SCALING     1
#define  VELOCITY_DISTANCE_SCALING                   (1 / SCALE_FACTOR)
#define  NEIGHBOR_CONNECTIONS_DIST_SCALING           (1 / SCALE_FACTOR)

/* Tests */

/* Graphics constants */
const float  BACKGROUND_BRIGHTNESS  = 1;
const float  LEAF_CUBE_BRIGHTNESS   = BACKGROUND_BRIGHTNESS >= 0.5 ? 0   :   1;
const float  PARENT_CUBE_BRIGHTNESS = BACKGROUND_BRIGHTNESS >= 0.5 ? BACKGROUND_BRIGHTNESS-0.25 : BACKGROUND_BRIGHTNESS+0.25;

const float  BACKGROUND_R           = BACKGROUND_BRIGHTNESS;
const float  BACKGROUND_G           = BACKGROUND_BRIGHTNESS;
const float  BACKGROUND_B           = BACKGROUND_BRIGHTNESS;
const float  BACKGROUND_A           = 1;
const float  SURFACE_R              = 0;
const float  SURFACE_G              = 0;
const float  SURFACE_B              = 1;
const float  SURFACE_A              = 1;
const float  LEAF_CUBE_R            = LEAF_CUBE_BRIGHTNESS;
const float  LEAF_CUBE_G            = LEAF_CUBE_BRIGHTNESS;
const float  LEAF_CUBE_B            = LEAF_CUBE_BRIGHTNESS;
const float  LEAF_CUBE_A            = 1;
const float  PARENT_CUBE_R          = PARENT_CUBE_BRIGHTNESS;
const float  PARENT_CUBE_G          = PARENT_CUBE_BRIGHTNESS;
const float  PARENT_CUBE_B          = PARENT_CUBE_BRIGHTNESS;
const float  PARENT_CUBE_A          = 1;
// Red
const float  LOWER_LOD_NON_LEAF_NEIGHBOR_CONNECTION_R  = 1;
const float  LOWER_LOD_NON_LEAF_NEIGHBOR_CONNECTION_G  = 0;
const float  LOWER_LOD_NON_LEAF_NEIGHBOR_CONNECTION_B  = 0;
const float  LOWER_LOD_NON_LEAF_NEIGHBOR_CONNECTION_A  = 1;
// Yellow
const float  LOWER_LOD_LEAF_NEIGHBOR_CONNECTION_R      = 1;
const float  LOWER_LOD_LEAF_NEIGHBOR_CONNECTION_G      = 1;
const float  LOWER_LOD_LEAF_NEIGHBOR_CONNECTION_B      = 0;
const float  LOWER_LOD_LEAF_NEIGHBOR_CONNECTION_A      = 1;
// Green
const float  SAME_LOD_NON_LEAF_NEIGHBOR_CONNECTION_R   = 0;
const float  SAME_LOD_NON_LEAF_NEIGHBOR_CONNECTION_G   = 1;
const float  SAME_LOD_NON_LEAF_NEIGHBOR_CONNECTION_B   = 0;
const float  SAME_LOD_NON_LEAF_NEIGHBOR_CONNECTION_A   = 1;
// Cyan
const float  SAME_LOD_LEAF_NEIGHBOR_CONNECTION_R       = 0;
const float  SAME_LOD_LEAF_NEIGHBOR_CONNECTION_G       = 1;
const float  SAME_LOD_LEAF_NEIGHBOR_CONNECTION_B       = 1;
const float  SAME_LOD_LEAF_NEIGHBOR_CONNECTION_A       = 1;
// Blue
const float  HIGHER_LOD_NEIGHBOR_CONNECTION_R          = 0;
const float  HIGHER_LOD_NEIGHBOR_CONNECTION_G          = 0;
const float  HIGHER_LOD_NEIGHBOR_CONNECTION_B          = 1;
const float  HIGHER_LOD_NEIGHBOR_CONNECTION_A          = 1;

const float  FINEST_NEIGHBOR_CONNECTION_R              = 0;
const float  FINEST_NEIGHBOR_CONNECTION_G              = 0;
const float  FINEST_NEIGHBOR_CONNECTION_B              = 0;
const float  FINEST_NEIGHBOR_CONNECTION_A              = 1;
const float  MIDDLE_MARK_R          = 0.5;
const float  MIDDLE_MARK_G          = 0.5;
const float  MIDDLE_MARK_B          = 0.5;
const float  MIDDLE_MARK_A          = 1;

const float  WATER_CELL_MARK_R      = 0.5;
const float  WATER_CELL_MARK_G      = 0.5;
const float  WATER_CELL_MARK_B      = 0.5;
const float  WATER_CELL_MARK_A      = 1;
const float  AIR_CELL_MARK_R        = 1;
const float  AIR_CELL_MARK_G        = 0;
const float  AIR_CELL_MARK_B        = 0;
const float  AIR_CELL_MARK_A        = 1;
const float  VELOCITY_R             = 0;
const float  VELOCITY_G             = 0;
const float  VELOCITY_B             = 0;
const float  VELOCITY_A             = 1;

////////////////////////////////////////////////////////////////
// TYPEDEFS
////////////////////////////////////////////////////////////////

#if  USE_DOUBLE_PRECISION_FOR_PHYSICS
typedef  double  base_float_type;
#else
typedef  float   base_float_type;
#endif

#if    DEBUG && CHECK_INITIALIZATION_OF_FLOATS
typedef  mustinit<base_float_type>  pftype;
#elif  DEBUG && INITIALIZE_FLOATS_TO_NAN
typedef  naninit <base_float_type>  pftype;
#else
typedef  base_float_type            pftype;
#endif

typedef unsigned int uint;

typedef    int8_t    int8;
typedef   uint8_t   uint8, byte;
typedef   int16_t   int16;
typedef  uint16_t  uint16;
typedef   int32_t   int32;
typedef  uint32_t  uint32;
typedef   int64_t   int64;
typedef  uint64_t  uint64;

////////////////////////////////////////////////////////////////
// ENUMS
////////////////////////////////////////////////////////////////

//TODO: Generalize the code so it works for 1, 2 and 3 dimensions
/* The real order should be X, Y, Z (change back if it isn't) */
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
#define  VERTICAL_DIMENSION     DIM_Y
#elif  NUM_DIMENSIONS == 3
#define  HORIZONTAL_DIMENSION1  DIM_X
#define  HORIZONTAL_DIMENSION2  DIM_Y
#define  VERTICAL_DIMENSION     DIM_Z
#endif

////////////////////////////////////////////////////////////////
// CONSTANTS
////////////////////////////////////////////////////////////////

/* Miscellaneous constants */
#define  NUM_PRIMARY_COLORS         3 /* Do not change */

/* Mathematical constants */
#ifndef  _MATH_H_
#define  M_E         2.7182818284590452354  // e, Euler's number
#define  M_LOG2E     1.4426950408889634074  // log_2(e)
#define  M_LOG10E    0.43429448190325182765 // log_10(e)
#define  M_LN2       0.69314718055994530942 // ln(2)
#define  M_LN10      2.30258509299404568402 // ln(10)
#define  M_PI        3.14159265358979323846 // pi
#define  M_PI_2      1.57079632679489661923 // pi/2
#define  M_PI_4      0.78539816339744830962 // pi/4
#define  M_1_PI      0.31830988618379067154 // 1/pi
#define  M_2_PI      0.63661977236758134308 // 2/pi
#define  M_2_SQRTPI  1.12837916709551257390 // 2/sqrt(pi)
#define  M_SQRT2     1.41421356237309504880 // sqrt(2)
#define  M_SQRT1_2   0.70710678118654752440 // sqrt(1/2)
#endif  // _MATH_H_

/* More mathematical constants */
#define  M_2PI               6.28318530717958647693 // 2*pi

/* Physical constants */
#define  P_G                 9.82000000000000000000 // [m/s^2] Gravitational acceleration
#define  P_1ATM              101325.0               // [Pa] The atmospheric pressure
#define  P_WATER_DENSITY_AT_1_ATM               999.9720               // [kg/m^3] The density of waterat +4 °C, see http://en.wikipedia.org/wiki/Properties_of_water#Density_of_water_and_ice

/* Independent physical constant variables */
//#define  P_AIR_PRESSURE      (1.0 * P_1ATM)            // [Pa] The atmospheric pressure //Not necessary to have
#define  P_WATER_TEMP        10.0000000000000000000 // [°C] The water temperature in degrees Celcius
#define  P_AIR_TEMP          10.0000000000000000000 // [°C] The air temperature in degrees Celcius

/* Dependent physical constant variables */
#define  P_AIR_DENSITY_AT_1_ATM                 (1.2898   - 0.00432   * P_AIR_TEMP) // [kg/m^3] The density of air, see http://en.wikipedia.org/wiki/Density_of_air#Temperature_and_pressure
#define  P_WATER_VISCOUSITY    (0.001614 - 0.0000306 * P_WATER_TEMP) // [Pa*s] The viscousity of the water, see http://en.wikipedia.org/wiki/Viscosity#Viscosity_of_water

////////////////////////////////////////////////////////////////
// MACROS
////////////////////////////////////////////////////////////////

// TODO: Make some of these inline functions instead

/* Mathematical macros */
#define  ABS(x)     ((x) >= 0 ? (x) : -(x))
#define  MIN(x, y)  ((x) <= (y) ? (x) : (y))
#define  MAX(x, y)  ((x) >= (y) ? (x) : (y))
#define  SQUARE(x)  ((x) * (x))
#define  IS_NAN(x)  ((x) != (x))

/* Miscellaneous macros */
#define  NO_OP()                  {float f = 0; if (f != 0) exit(0);}
#define  MAKE_STRING(x)           #x
#define  LINE_UNREACHABLE2(line)  {throw logic_error("Should not be able to reach this line: line " MAKE_STRING(line) " in " __FILE__);}
#define  LINE_UNREACHABLE()       LINE_UNREACHABLE2(__LINE__)

#define  TEMP_SWAP(x, y, temp) { \
    (temp) = (x);                \
    (x) = (y);                   \
    (y) = (temp);                \
    }

#if  0
#define  XOR_SWAP(x, y) { \
    (x) ^= (y)            \
    (y) ^= (x)            \
    (x) ^= (y)            \
    }
#endif

#endif  /* DEFINITIONS_H */
