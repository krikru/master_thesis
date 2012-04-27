
////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

// Standard includes
//#include <stdexcept>
//using std::exception;
//using std::logic_error;
//using std::out_of_range;

// Own includes
#include "octneighbor.h"
#include "octcell.h"

////////////////////////////////////////////////////////////////
// CONSTRUCTORS AND DESTRUCTOR
////////////////////////////////////////////////////////////////

// TODO: Remove the default constructor, this is just unnecessary
/* Default constructor */
octneighbor::octneighbor()
{
}

void octneighbor::set(octcell* neighbor_cell, nlnode* corresponding_neighbor_list_entry, uint dimension, bool positive_direction, pftype water_velocity_in_the_positive_direction, pfvec distance, pftype distance_absolute_value, pftype cell_face_area)
{
    n        = neighbor_cell                    ;
    cnle     = corresponding_neighbor_list_entry;
    dim      = dimension                        ;
    pos_dir  = positive_direction               ;
    vel      = water_velocity_in_the_positive_direction;
    dist     = distance                         ;
    dist_abs = distance_absolute_value          ;
    cf_area  = cell_face_area                   ;
}

void octneighbor::update_velocity(octcell* cell1, octcell* cell2, pftype dt)
{
    /* No advection term implemented */
    vel += (cell1->rp - cell2->rp - dist[VERTICAL_DIMENSION] * P_G) / dist_abs * dt;
    cnle->v.vel = vel;
}
