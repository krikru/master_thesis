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

    /*******************
     * Volume of fluid *
     *******************/

    pftype water_vol_coeff; /* [1] The volume of the water divided by the volujme of the cell (between 0 and total_vol_coeff) */
    pftype total_vol_coeff; /* [1] The volume of the water and air divided by the volujme of the cell (should stay around 1) */

    /*****************
     * Navier-Stokes *
     *****************/

    /* Water flow */
    pftype vel_out; /* Velocity of the water in the direction out from the cell, towards the neighbor cell */

    /* Distance between cells */
    pfvec  dist;
    pftype dist_abs;

    /* Surface */
    pftype cf_area; /* Cell face area */

public:
    /* Public methods */
    void set(octcell* neighbor_cell, nlnode* corresponding_neighbor_list_entry, uint dimension, bool positive_direction, pftype water_volume_coefficient, pftype total_volume_coefficient, pftype water_velocity_in_the_out_direction, pfvec distance, pftype distance_absolute_value, pftype cell_face_area);
    void set_velocity_out(pftype velocity_out);
    void set_volume_coefficients(pftype water_volume_coefficient, pftype total_volume_coefficient);
    int  get_signed_dir();

    /* Simulation */
    bool should_calculate_new_velocity();
    void update_velocity(octcell* cell1, octcell* cell2, pftype dt);

private:
    /*************************
     * Disabled constructors *
     *************************/
    octneighbor(octneighbor&); // Copy constructor prevented from all use
};

inline
int octneighbor::get_signed_dir()
{
    return pos_dir ? 1 : -1;
}

inline
bool octneighbor::should_calculate_new_velocity()
{
    return pos_dir;
}



#endif // OCTNEIGHBOR_H
