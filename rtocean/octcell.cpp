
////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

// Standard includes
#include <stdexcept>
//using std::exception;
using std::logic_error;

// Own includes
#include "octcell.h"

////////////////////////////////////////////////////////////////
// CONSTRUCTORS AND DESTRUCTOR
////////////////////////////////////////////////////////////////

octcell::octcell(pftype size, pftype x_pos, pftype y_pos, pftype z_pos, octcell **children)
{
    s = size;
    x = x_pos;
    y = y_pos;
    z = z_pos;
    c = children;
}

octcell::~octcell()
{
    if (c) {
        for (int i = 0; i < MAX_NUM_CHILDREN; i++) {
            if (c[i]) {
                delete c[i];
            }
        }
        delete c;
    }
}

////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////

void octcell::refine()
{
#if DEBUG
    if (c) {
        throw logic_error("Trying to refine a cell that already has children");
    }
#endif

    // Create children
    c = new *octcell[MAX_NUM_CHILDREN];
    for (int i = 0; i < MAX_NUM_CHILDREN; i++) {
        c[i] = new octcell;
    }

    pftype s_2 = 0.5 * s;
    // Assign sizes
    c[0].s = c[1].s = c[2].s = c[3].s = c[4].s = c[5].s = c[6].s = c[7].s = s_2;
    // Assign x-positions
    c[0].x = c[2].x = c[4].x = c[6].x = x;
    c[1].x = c[3].x = c[5].x = c[7].x = x + s_2;
    // Assign y-positions
    c[0].y = c[1].y = c[4].y = c[5].y = y;
    c[2].y = c[3].y = c[6].y = c[7].y = y + s_2;
    // Assign z-positions
    c[0].z = c[1].z = c[2].z = c[3].z = z;
    c[4].z = c[5].z = c[6].z = c[7].z = z + s_2;
}
