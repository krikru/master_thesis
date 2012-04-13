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
    int refine_octcell(octcell* c, pftype surface, pftype bottom, pftype accuracy);
};

#endif // FVOCTREE_H
