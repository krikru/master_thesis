
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

////////////////////////////////////////////////////////////////
// CONSTRUCTORS AND DESTRUCTOR
////////////////////////////////////////////////////////////////

// TODO: Remove the default constructor, this is just unnecessary
/* Default constructor */
octneighbor::octneighbor()
{
#if DEBUG
    set(0, 0, pfvec(), 0, 0, 0, 0, 0);
#endif
}

void octneighbor::set(octcell* neighbor_cell, nlnode* corresponding_neighbor_list_entry, pfvec distance, pftype distance_absolute_value, pftype cell_face_area, uint dimension, bool positive_direction, uint lowest_level)
{
    n        = neighbor_cell                    ;
    cnle     = corresponding_neighbor_list_entry;
    dist     = distance                         ;
    dist_abs = distance_absolute_value          ;
    cf_area  = cell_face_area                   ;
    dim      = dimension                        ;
    pos_dir  = positive_direction               ;
    low_lvl  = lowest_level                     ;
}
