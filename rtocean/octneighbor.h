#ifndef OCTNEIGHBOR_H
#define OCTNEIGHBOR_H

////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

// Own includes
#include "definitions.h"
#include "dllist.h"

////////////////////////////////////////////////////////////////
// PREDECLARATIONS
////////////////////////////////////////////////////////////////

class octneighbor;
class octcell;

////////////////////////////////////////////////////////////////
// TYPEDEFS
////////////////////////////////////////////////////////////////

typedef  dllist<octneighbor>   nlist ;
typedef  dllnode<octneighbor>  nlnode;

////////////////////////////////////////////////////////////////
// CLASS DEFINTION
////////////////////////////////////////////////////////////////

class octneighbor
{
public:
    octneighbor();

public:

    /***********
     * General *
     ***********/

    /* Neighbor */
    octcell* n; /* Pointer to the neighbor cell */
    nlnode*  cnle; // The neighbor's corresponding neighbor list entry

    /* Direction of the octneighbor */
    uint dim;
    bool pos_dir;

    /***************************
     * Advection of properties *
     ***************************/

    /* Volume of fluid method */
    pftype water_vol_coeff; /* [1] The volume of the water divided by the volujme of the cell (between 0 and total_vol_coeff) */
    pftype total_vol_coeff; /* [1] The volume of the water and air divided by the volujme of the cell (should stay around 1) */

    /* Advection of momentum */
    pfvec  quasi_momentum_vector; /* [kg/(s*m^2)] The quasi-momentum vector to be advected */
    pftype quasi_momentum_out; /* [kg/(s*m^2)] The quasi-momentum scalar, could be united with the velocity scalar */

    /*****************
     * Navier-Stokes *
     *****************/

    /* Water flow */
    pftype vel_out; /* [m/s] Velocity of the water in the direction out from the cell, towards the neighbor cell */

    /* Distance between cells */
    pfvec  dist;     /* [m] Distande to the neighbor cell (dependent of direction) */
    pftype dist_abs; /* [m] Absolute distande (independent of direction) */

    /* Surface */
    pftype cf_area;  /* [m^2] Cell face area */

public:
    /* Public methods */
    void   set(octcell* neighbor_cell, nlnode* corresponding_neighbor_list_entry, uint dimension, bool positive_direction, pftype water_volume_coefficient, pftype total_volume_coefficient, pftype water_velocity_in_the_out_direction, pfvec distance, pftype distance_absolute_value, pftype cell_face_area);
    void   set_velocity_out(pftype velocity_out);
    void   set_volume_coefficients(pftype water_volume_coefficient, pftype total_volume_coefficient);
    int    get_signed_dir();
    pftype get_vel_in_pos_dir();                /* [m/s] */
    pftype get_average_cell_density();          /* [kg/m^3] */
    pftype get_associated_mass_per_unit_area(); /* [kg/m^2] */
    pftype get_cell_face_density();             /* [kg/m^3] */

    /* Simulation */
    bool should_calculate_new_velocity();
    void update_velocity(octcell* cell1, octcell* cell2, pftype dt);

private:
    /*************************
     * Disabled constructors *
     *************************/
    octneighbor(octneighbor&); // Copy constructor prevented from all use
};

////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

// Own includes
//#include "octcell.h"

////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////

inline
int octneighbor::get_signed_dir()
{
    return pos_dir ? 1 : -1;
}

inline
pftype octneighbor::get_vel_in_pos_dir()
{
    return vel_out * get_signed_dir();
}



inline
bool octneighbor::should_calculate_new_velocity()
{
    return pos_dir;
}



#endif // OCTNEIGHBOR_H
