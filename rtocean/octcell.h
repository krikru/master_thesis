#ifndef OCTCELL_H
#define OCTCELL_H

////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

// Own includes
#include "definitions.h"
//#include "octface.h"

////////////////////////////////////////////////////////////////
// CLASS DEFINITION
////////////////////////////////////////////////////////////////

/*
 * The indexes of the children cells (if existing) are calculated
 * as 1*ix + 2*iy + 4*iz, where ix, iy and iz are either 0 or 1
 * depending on the relative (to the parent cell) x, y and z
 * positions respectivelly.
 */
class octcell
{
public:
    static const uint MAX_NUM_CHILDREN = 8;
public:
    /*******************************
     * Constructors and destructor *
     *******************************/
    //octcell(pftype size, pftype x_pos, pftype y_pos, pftype z_pos, pftype desired_level_of_detail = 0, octcell **children = 0);
    octcell(pftype size, pftype x_pos, pftype y_pos, pftype z_pos, octcell **children = 0);
    ~octcell();

public:
    /***************************
     * Public member variables *
     ***************************/
    /*
     * Geometry
     * --------
     *
     * The cell is a cube that stretches from (x, y, z) to (x + s, y + s, z + s)
     *
     * The cell size should be smaller than or equal to the desired level of detail (dlod)
     */
    pftype s; /* Size of the cell (the length of an edge) */
    pftype x; /* X-position of first corner */
    pftype y; /* Y-position of first corner */
    pftype z; /* Z-position of first corner */

    //pftype dlod; /* Desired level of feature */

    /* Children */
    octcell **c; /* The possible children */

public:
    /*****************************
     * Public non-static methods *
     *****************************/

    /* Geometry */
    //bool has_desired_level_of_detail();
    pfvec3 cell_center();

    /* Children */
    bool has_child_array();
    bool is_leaf();
    void refine(); // Creates new child array and new children
    void unleaf(); // Creates new child array but no children
    octcell* add_child(uint idx);
    void remove_child(uint idx);

public:
    /*************************
     * Public static methods *
     *************************/
    static uint child_index(uint x, uint y, uint z);

private:

    /*************************
     * Disabled constructors *
     *************************/
    octcell(); // Default constructor prevented from all use
    octcell(octcell&); // Default constructor prevented from all use
};

#endif // OCTCELL_H
