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
    octcell *c = root = new octcell(1, pfvec(), 0);
#if 0
    c->refine();
    c = c->get_child(octcell::child_index(0, 1, 0));
    c->refine();
    c = c->get_child(octcell::child_index(1, 0, 0));
    c->refine();
    c = c->get_child(octcell::child_index(0, 1, 0));
    c->refine();
    for (int i = 0; i < 2; i++) {
        c = c->get_child(octcell::child_index(0, 1, 0));
        c->refine();
    }
#else
    refine_subtree(c, surface, bottom, size_accuracy);
    //TODO: Remove the refinement of selected octcells
#if 1
#if  NUM_DIMENSIONS == 2
    c->get_child(octcell::child_index_xy(0, 0))->refine();
    //c->get_child(octcell::child_index_xy(1, 0))->refine();
    //c->get_child(octcell::child_index_xy(1, 0))->get_child(octcell::child_index_xy(0, 0))->refine();
    //c->get_child(octcell::child_index_xy(1, 1))->refine();
#elif  NUM_DIMENSIONS == 3
    c->get_child(octcell::child_index_xyz(1, 0, 0))->refine();
    c->get_child(octcell::child_index_xyz(1, 0, 0))->get_child(octcell::child_index_xyz(0, 0, 0))->refine();
    c->get_child(octcell::child_index_xyz(1, 1, 0))->refine();
#endif
#endif
#if  GENERATE_NEIGHBORS_STATICALLY
    generate_neighbor_lists();
#endif
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

pftype fvoctree::size_accuracy(pfvec r)
{
    return 0.02 + 0.1 * r.e[DIR_X];
}

/* Returns true if the cell should be removed from the simulation */
bool fvoctree::refine_subtree(octcell* c, pftype surface, pftype bottom, pftype (*accuracy_function)(pfvec))
{

    static int tot_num_cells = 1;
    static int num_leaf_cells = 1;
    pftype s = c->s;

    pftype height = c->r[UP_DIRECTION];
#if  NUM_DIMENSIONS == 2
    pftype min_surf_height = 0.55 + 0.2*c->r[HORIZONTAL_DIRECTION1];
    pftype max_surf_height = 0.55 + 0.2*(c->r[HORIZONTAL_DIRECTION1]+s);
#elif  NUM_DIMENSIONS == 3
    pftype min_surf_height = 0.55 + 0.2*c->r[HORIZONTAL_DIRECTION1] + 0.1*c->r[HORIZONTAL_DIRECTION2];
    pftype max_surf_height = 0.55 + 0.2*(c->r[HORIZONTAL_DIRECTION1]+s) + 0.1*(c->r[HORIZONTAL_DIRECTION2]+s);
#endif
    if (height >= max_surf_height) {
        // Cell is over the surface, remove it
        return true;
    }
    if (height + s <= min_surf_height) {
        // Cell is under the surface, keep it
        return false;
    }
    if (s <= accuracy_function(c->cell_center())) {
        // Accuracy is good enough, quit refining
        return false;
    }
    // Cell is a surface cell and not fine enough, refine it and treat the children recursivelly
    c->refine();
    tot_num_cells += octcell::MAX_NUM_CHILDREN;
    num_leaf_cells += octcell::MAX_NUM_CHILDREN - 1;
    for (uint i = 0; i < octcell::MAX_NUM_CHILDREN; i++) {
        if (refine_subtree(c->get_child(i), surface, bottom, accuracy_function)) {
            c->remove_child(i);
            tot_num_cells--;
            num_leaf_cells--;
        }
    }
#if DEBUG
#if 0
    cout << "Total number of cells: " << tot_num_cells << endl;
    cout << "Number of leaf cells:  " << num_leaf_cells << endl;
    cout << endl;
#endif
#endif
    return 0;
}

#if  GENERATE_NEIGHBORS_STATICALLY
void fvoctree::generate_neighbor_lists()
{
    if (root) {
        root->generate_all_internal_neighbors();
    }
}
#endif
