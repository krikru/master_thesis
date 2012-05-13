#ifndef FVOCTREE_H
#define FVOCTREE_H

////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

#include "octcell.h"

////////////////////////////////////////////////////////////////
// CLASS DEFINITION
////////////////////////////////////////////////////////////////

class fvoctree
{
public:
    fvoctree();
    fvoctree(pftype surface, pftype bottom);
    ~fvoctree();

public:
    octcell *root;

private:
    /* Private non-static methods */
    bool refine_subtree(octcell* c, pftype surface, pftype bottom);
    void prepare_cells_for_water_recursively(octcell* cell);

public:
    /* Public static methods */
    static pftype size_accuracy(pfvec r);

private:
    /*************************
     * Disabled constructors *
     *************************/
    fvoctree(fvoctree&); // Copy constructor prevented from all use
};

#endif // FVOCTREE_H
