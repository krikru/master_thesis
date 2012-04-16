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
    static pftype size_accuracy(pfvec3 r);
    int refine_octcell(octcell* c, pftype surface, pftype bottom, pftype (*accuracy_function)(pfvec3));
};

#endif // FVOCTREE_H
