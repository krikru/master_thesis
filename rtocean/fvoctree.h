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
#if  GENERATE_NEIGHBORS_STATICALLY
    void generate_neighbor_lists();
#endif

private:
    /*************************
     * Disabled constructors *
     *************************/
    fvoctree(fvoctree&); // Copy constructor prevented from all use
};

#endif // FVOCTREE_H
