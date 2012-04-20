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

    /* Neighbor list */
    nlnode* nle; // The object's entry in the neighbor list //TODO: Remove this member; it is superfluous
    nlnode* mnle; // The matching entry in the neighbor's neighbor list

    /* Neighbor */
    octcell* n; /* Pointer to the neighbor cell */

    /* Distance between cells */
    pfvec  dist;
    pftype dist_abs;

    /* Surface */
    pftype cf_area;

    /* Direction of the octneighbor */
    uint dim;
    bool rev_dir;

    /* Level of detail */
    uint low_lvl; // Lowest level of detail the neighbor is still relevant for

public:
    /* Public methods */
    void set(nlnode* neighbor_list_entry, nlnode* matching_neighbor_list_entry, octcell* neighbor_cell, pfvec distance, pftype distance_absolute_value, pftype cell_face_area, uint dimension, bool reverse_direction, uint lowest_level);
    void remove();

private:
    /*************************
     * Disabled constructors *
     *************************/
    octneighbor(octneighbor&); // Copy constructor prevented from all use
};

#endif // OCTNEIGHBOR_H
