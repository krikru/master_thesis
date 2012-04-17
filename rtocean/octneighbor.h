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
    nlnode* nle; // The object's entry in the neighbor list

    /* Neighbor */
    octcell* n;

    /* Distance between cells */
    pfvec3 dist;
    pftype dist_abs;

    /* Surface */
    pftype cf_area;

public:
    /* Public methods */
    void initialize(nlnode* neighbor_list_entry, octcell* neighbor_cell, pfvec3 distance, pftype distance_absolute_value, pftype cell_face_area);

private:
    /*************************
     * Disabled constructors *
     *************************/
    octneighbor(octneighbor&); // Copy constructor prevented from all use
};

#endif // OCTNEIGHBOR_H
