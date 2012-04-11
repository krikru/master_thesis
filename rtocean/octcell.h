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
    static const int MAX_NUM_CHILDREN = 8;
public:
    /*******************************
     * Constructors and destructor *
     *******************************/
    octcell(pftype size, pftype x_pos, pftype y_pos, pftype z_pos, octcell **children = 0);
    ~octcell();

public:
    /***************************
     * Public member variables *
     ***************************/
    pftype s; /* Size of the cell (the length of an edge) */
    pftype x; /* X-position of first corner */
    pftype y; /* Y-position of first corner */
    pftype z; /* Z-position of first corner */

    octcell **c; /* The possible children */

public:
    /******************
     * Public methods *
     ******************/
    bool has_child_array();
    bool is_leaf();
    void refine(); // Creates new child cells
    void unleaf(); // Doesn't create new child cells
    octcell* add_child(int idx);
    void remove_child(int idx);

private:

    /*************************
     * Disabled constructors *
     *************************/
    octcell(); // Default constructor prevented from all use
    octcell(octcell&); // Default constructor prevented from all use
};

#endif // OCTCELL_H
