#ifndef  PHYSICS_H
#define  PHYSICS_H

////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

/* Own include files */
#include "definitions.h"

namespace physics {

////////////////////////////////////////////////////////////////
// PUBLIC FUNCTION DEFINITIONS
////////////////////////////////////////////////////////////////

inline
pftype vol_coeffs_to_density(pftype water_volume_coeff, pftype total_volume_coeff)
{
    return  water_volume_coeff * (NORMAL_WATER_DENSITY - NORMAL_AIR_DENSITY) +
            total_volume_coeff * NORMAL_AIR_DENSITY                              ;
}

} // namespace physics

#endif  /* PHYSICS_H */
