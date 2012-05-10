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

    /* Private methods */
private:
    static pftype size_accuracy(pfvec r);
    bool refine_subtree(octcell* c, pftype surface, pftype bottom, pftype (*accuracy_function)(pfvec));
    void prepare_cells_for_water_recursively(octcell* cell);

private:
    /*************************
     * Disabled constructors *
     *************************/
    fvoctree(fvoctree&); // Copy constructor prevented from all use
};

#endif // FVOCTREE_H
