
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

void octneighbor::set(octcell* neighbor_cell, nlnode* corresponding_neighbor_list_entry, uint dimension, bool positive_direction, pftype water_volume_coefficient, pftype total_volume_coefficient, pftype water_velocity_in_the_out_direction, pfvec distance, pftype distance_absolute_value, pftype cell_face_area)
{
    n               = neighbor_cell                      ;
    cnle            = corresponding_neighbor_list_entry  ;
    dim             = dimension                          ;
    pos_dir         = positive_direction                 ;
    water_vol_coeff = water_volume_coefficient           ;
    total_vol_coeff = total_volume_coefficient           ;
    vel_out         = water_velocity_in_the_out_direction;
    dist            = distance                           ;
    dist_abs        = distance_absolute_value            ;
    cf_area         = cell_face_area                     ;
}

void octneighbor::set_velocity_out(pftype velocity_out)
{
    vel_out = velocity_out;
    cnle->v.vel_out = -velocity_out;
}

void octneighbor::set_volume_coefficients(pftype water_volume_coefficient, pftype total_volume_coefficient)
{
#if  DEBUG
    if (IS_NAN(water_volume_coefficient)) {
        throw logic_error("Trying to set a NaN water_volume_coefficient in cell wall");
    }
    if (IS_NAN(total_volume_coefficient)) {
        throw logic_error("Trying to set a NaN total_volume_coefficient in cell wall");
    }
    if (water_volume_coefficient < 0) {
        throw logic_error("Trying to set a negative water_volume_coefficient in cell wall");
    }
    if (total_volume_coefficient < 0) {
        throw logic_error("Trying to set a negative total_volume_coefficient in cell wall");
    }
    if (water_volume_coefficient > total_volume_coefficient) {
        throw logic_error("Trying to set a higher water_volume_coefficient than total_volume_coefficient in cell");
    }
#endif
    water_vol_coeff = water_volume_coefficient;
    total_vol_coeff = total_volume_coefficient;
    cnle->v.water_vol_coeff = water_volume_coefficient;
    cnle->v.total_vol_coeff = total_volume_coefficient;
}

void octneighbor::update_velocity(octcell* cell1, octcell* cell2, pftype dt)
{
    /* No advection term implemented */
    pftype distance = cell1->s + cell2->s;
#if  !NO_ATMOSPHERE
    pftype average_total_density = (cell1->total_density*cell1->s + cell2->total_density*cell2->s) / distance;
#endif
    //average_total_density = MIN(average_total_density, P_WATER_DENSITY); // Prevent nasty circulation behaviours in the water
    //TDO: Prevevt circulation behaviour even in the air
    distance *= 0.5;
#if  NO_ATMOSPHERE
    vel_out += ((cell1->p - cell2->p) / (distance * P_WATER_DENSITY) - dist[VERTICAL_DIMENSION]/dist_abs * P_G) * dt;
#else
    vel_out += ((cell1->p - cell2->p) / (distance * average_total_density) - dist[VERTICAL_DIMENSION]/dist_abs * P_G * (average_total_density > P_WATER_DENSITY ? P_WATER_DENSITY/average_total_density : 1)) * dt;
#endif
    cnle->v.vel_out = -vel_out;
}
