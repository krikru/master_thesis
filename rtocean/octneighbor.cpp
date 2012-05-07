
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

void octneighbor::set(octcell* neighbor_cell, nlnode* corresponding_neighbor_list_entry, uint dimension, bool positive_direction, pftype water_velocity_in_the_out_direction, pfvec distance, pftype distance_absolute_value, pftype cell_face_area)
{
    n        = neighbor_cell                    ;
    cnle     = corresponding_neighbor_list_entry;
    dim      = dimension                        ;
    pos_dir  = positive_direction               ;
    vel_out  = water_velocity_in_the_out_direction;
    dist     = distance                         ;
    dist_abs = distance_absolute_value          ;
    cf_area  = cell_face_area                   ;
}

void octneighbor::set_velocity_out(pftype velocity_out)
{
    vel_out = velocity_out;
    cnle->v.vel_out = -velocity_out;
}

void octneighbor::set_densities(pftype new_water_density, pftype new_total_density)
{
    cnle->v.water_density = water_density = new_water_density;
    cnle->v.total_density = total_density = new_total_density;
}

void octneighbor::update_velocity(octcell* cell1, octcell* cell2, pftype dt)
{
    /* No advection term implemented */
    pftype distance = cell1->s + cell2->s;
    pftype average_total_density = (cell1->total_density*cell1->s + cell2->total_density*cell2->s) / distance;
    //average_total_density = MIN(average_total_density, P_WATER_DENSITY); // Prevent nasty circulation behaviours in the water
    //TDO: Prevevt circulation behaviour even in the air
    distance *= 0.5;
    vel_out += ((cell1->p - cell2->p) / (distance * average_total_density) - dist[VERTICAL_DIMENSION] * P_G * (average_total_density > P_WATER_DENSITY ? P_WATER_DENSITY/average_total_density : 1)) * dt;
    cnle->v.vel_out = -vel_out;
}
