////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

// Standard includes
#include <iostream>
using std::cout;
using std::endl;

// Own include files
#include "fvoctree.h"

////////////////////////////////////////////////////////////////
// CONSTRUCTORS AND DESTRUCTOR
////////////////////////////////////////////////////////////////

fvoctree::fvoctree()
{
    root = 0;
}

fvoctree::fvoctree(pftype surface, pftype bottom)
{
    bottom = bottom;
    surface = surface;
    octcell *c = root = new octcell(1, 0, 0, 0);
#if 0
    c->refine();
    c = c->c[octcell::child_index(0, 1, 0)];
    c->refine();
    c = c->c[octcell::child_index(1, 0, 0)];
    c->refine();
    c = c->c[octcell::child_index(0, 1, 0)];
    c->refine();
    for (int i = 0; i < 2; i++) {
        c = c->c[octcell::child_index(0, 1, 0)];
        c->refine();
    }
#else
    refine_octcell(c, surface, bottom, size_accuracy);
#endif
}

fvoctree::~fvoctree()
{
    if (root) {
        delete root;
    }
}

////////////////////////////////////////////////////////////////
// PRIVATE METHODS
////////////////////////////////////////////////////////////////

pftype fvoctree::size_accuracy(pfvec3 r)
{
    return 0.01 + 0.03 * r.e[DIR_X];
}

int fvoctree::refine_octcell(octcell* c, pftype surface, pftype bottom, pftype (* accuracy_function)(pfvec3))
{
    static int tot_num_cells = 1;
    static int num_leaf_cells = 1;
    pftype s = c->s;
    pftype min_surf_height = 0.55 + 0.2*c->x + 0.1*c->y;
    pftype max_surf_height = 0.55 + 0.2*(c->x+s) + 0.1*(c->y+s);
    if (c->z >= max_surf_height) {
        // Cell is over the surface, remove it
        return 1;
    }
    if (c->z+s <= min_surf_height) {
        // Cell is under the surface, keep it
        return 0;
    }
    if (s <= accuracy_function(c->cell_center())) {
        // Accuracy is good enough, quit refining
        return 0;
    }
    // Cell is a surface cell and not fine enough, refine it and treat the children recursivelly
    c->refine();
    tot_num_cells += 8;
    num_leaf_cells += 7;
    for (uint i = 0; i < octcell::MAX_NUM_CHILDREN; i++) {
        if (refine_octcell(c->c[i], surface, bottom, accuracy_function)) {
            c->remove_child(i);
            tot_num_cells--;
            num_leaf_cells--;
        }
    }
#if 0
    cout << "Total number of cells: " << tot_num_cells << endl;
    cout << "Number of leaf cells:  " << num_leaf_cells << endl;
    cout << endl;
#endif
    return 0;
}
