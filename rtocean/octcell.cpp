
////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

// Standard includes
#include <stdexcept>
//using std::exception;
using std::logic_error;
using std::out_of_range;

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

bool octcell::has_child_array()
{
    return c;
}

bool octcell::is_leaf()
{
    return !c;
}

void octcell::refine()
{
#if DEBUG
    if (has_child_array()) {
        throw logic_error("Trying to refine a cell that already has child array");
    }
#endif

    // Create child array
    c = new octcell*[MAX_NUM_CHILDREN];
    // Create new values for children
    pftype s_2 = 0.5 * s;
    pftype x_2 = x + s_2;
    pftype y_2 = y + s_2;
    pftype z_2 = z + s_2;
    // Assign values to children
    c[0] = new octcell(s_2, x  , y  , z  );
    c[1] = new octcell(s_2, x_2, y  , z  );
    c[2] = new octcell(s_2, x  , y_2, z  );
    c[3] = new octcell(s_2, x_2, y_2, z  );
    c[4] = new octcell(s_2, x  , y  , z_2);
    c[5] = new octcell(s_2, x_2, y  , z_2);
    c[6] = new octcell(s_2, x  , y_2, z_2);
    c[7] = new octcell(s_2, x_2, y_2, z_2);
}

void octcell::unleaf()
{
#if DEBUG
    if (has_child_array()) {
        throw logic_error("Trying to unleaf a cell that already has child array");
    }
#endif

    // Create children
    c = new octcell*[MAX_NUM_CHILDREN];
    for (int i = 0; i < MAX_NUM_CHILDREN; i++) {
        c[i] = 0;
    }
}


octcell* octcell::add_child(int idx)
{
#if DEBUG
    if (is_leaf()) {
        throw logic_error("Trying to add a child cell to a leaf cell (it first has to be un-leafed)");
    }
    if (idx < 0 || idx >= MAX_NUM_CHILDREN) {
        throw out_of_range("Trying to add a child cell with index out of bound");
    }
    if (c[idx]) {
        throw logic_error("Trying to add a child cell that already exist");
    }
#endif

    pftype s_2 = 0.5 * s;
    pftype x1 = x + ((idx >> 0) & 1) * s_2;
    pftype y1 = y + ((idx >> 1) & 1) * s_2;
    pftype z1 = z + ((idx >> 2) & 1) * s_2;

    c[idx] = new octcell(s_2, x1, y1, z1);

    return c[idx];
}

void octcell::remove_child(int idx)
{
#if DEBUG
    if (is_leaf()) {
        throw logic_error("Trying to remove a child cell from a leaf cell");
    }
    if (idx < 0 || idx >= MAX_NUM_CHILDREN) {
        throw out_of_range("Trying to remove a child cell with index out of bound");
    }
    if (!c[idx]) {
        throw logic_error("Trying to remove a child cell that doesn't exist");
    }
#endif

    delete c[idx];
    c[idx] = 0;
}
