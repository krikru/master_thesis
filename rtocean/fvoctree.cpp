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
    refine_subtree(c, surface, bottom, size_accuracy);
    //TODO: Remove the refinement of selected octcells
#if  NUM_DIMENSIONS == 2
#if TEST_REFINING_AND_COARSENING
    c->get_child(octcell::child_index_xy(0, 0))->refine();
    //c->get_child(octcell::child_index_xy(1, 0))->refine();
    //c->get_child(octcell::child_index_xy(1, 0))->get_child(octcell::child_index_xy(0, 0))->refine();
    //c->get_child(octcell::child_index_xy(1, 1))->refine();
    c = c->get_child(octcell::child_index_xy(0, 1));
    //c = c->get_child(octcell::child_index_xy(1, 1));
    if (!c) {
        throw logic_error("Child node pointer is NULL! Cannot do what was planned :P");
    }
    c = c->get_child(octcell::child_index_xy(1, 0));
    //c = c->get_child(octcell::child_index_xy(0, 0));
    if (!c) {
        throw logic_error("Child node pointer is NULL! Cannot do what was planned :P");
    }
    if (c->is_leaf()) {
        throw logic_error("Node is a leaf! Cannot do what was planned (coarsen it) :P");
    }
    c->coarsen();
#endif // TEST_REFINING_AND_COARSENING
#elif  NUM_DIMENSIONS == 3
#if  TEST_REFINING_AND_COARSENING
    c->get_child(octcell::child_index_xyz(1, 0, 0))->refine();
    c->get_child(octcell::child_index_xyz(1, 0, 0))->get_child(octcell::child_index_xyz(0, 0, 0))->refine();
    c->get_child(octcell::child_index_xyz(1, 1, 0))->refine();
    c = root->get_child(octcell::child_index_xyz(0, 0, 1));
    if (!c) {
        throw logic_error("Child node pointer is NULL! Cannot do what was planned :P");
    }
    c = c->get_child(octcell::child_index_xyz(0, 1, 0));
    if (!c) {
        throw logic_error("Child node pointer is NULL! Cannot do what was planned :P");
    }
    if (c->is_leaf()) {
        throw logic_error("Node is a leaf! Cannot do what was planned (coarsen it) :P");
    }
    c->coarsen();
#endif  // TEST_REFINING_AND_COARSENING
#endif  // NUM_DIMENSIONS == 2, 3

#if  GENERATE_NEIGHBORS_STATICALLY
    generate_leaf_neighbor_lists();
#endif  // GENERATE_NEIGHBORS_STATICALLY
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
    if (r.e[VERTICAL_DIMENSION] < SURFACE_HEIGHT) {
        /* Cell is under the surface */
        return SURFACE_ACCURACY + (SURFACE_HEIGHT-r.e[VERTICAL_DIMENSION])
                * (1/(MIN_LOD_LAYER_THICKNESS + 0.5));
    }
    else {
        return SURFACE_ACCURACY;
    }
    //return SIZE_ACCURACY_FACTOR*(0.02 + 0.1 * r.e[DIM_X]);
}

/* Returns true if the cell should be removed from the simulation */
bool fvoctree::refine_subtree(octcell* c, pftype surface, pftype bottom, pftype (*accuracy_function)(pfvec))
{

    static int tot_num_cells = 1;
    static int num_leaf_cells = 1;
    pftype s = c->s;

    pftype lowest_cell_height = c->r[VERTICAL_DIMENSION];
#if    NUM_DIMENSIONS == 2
    pftype min_surf_height = 0.5501 + 0.2*c->r[HORIZONTAL_DIMENSION1];
    pftype max_surf_height = 0.5501 + 0.2*(c->r[HORIZONTAL_DIMENSION1]+s);
#elif  NUM_DIMENSIONS == 3
    pftype min_surf_height = 0.55 + 0.2*c->r[HORIZONTAL_DIMENSION1] + 0.1*c->r[HORIZONTAL_DIMENSION2];
    pftype max_surf_height = 0.55 + 0.2*(c->r[HORIZONTAL_DIMENSION1]+s) + 0.1*(c->r[HORIZONTAL_DIMENSION2]+s);
#endif
    if (lowest_cell_height >= max_surf_height) {
        // Cell is over the surface, remove it
        return true;
    }
#if 0
    if (lowest_cell_height + s <= min_surf_height     ||  // Cell is under the surface, keep it but stop refining
        s <= accuracy_function(c->cell_center()) ) { // Accuracy is good enough, stop refining
#else
    if (s <= accuracy_function(c->cell_center())) { // Accuracy is good enough, stop refining
#endif
        /* Stop refining */
        /* Calculate properties */
        pftype mean_height = lowest_cell_height + s/2;
        pftype mean_surface_height = (min_surf_height + max_surf_height)/2;
        c->rp = (mean_surface_height - mean_height) * P_G;
        if (lowest_cell_height + s > min_surf_height) {
            /* Cell is a surface cell */
            c->surface_cell = true;
            /* Estimate vof */
            pftype mean_height_diff = (MAX(min_surf_height - lowest_cell_height, 0) + MIN(max_surf_height - lowest_cell_height, s))/2;
            c->vof = mean_height_diff * c->get_side_area();
#if  DEBUG
            if (c->vof < 0 || c->vof > c->get_total_volume()) {
                throw logic_error("VOF incorrectly calculated");
            }
#endif
        }
        else {
            /* Cell is not a surface cell */
            c->surface_cell = false;
        }
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
#if  DEBUG
#if  0
    cout << "Total number of cells: " << tot_num_cells << endl;
    cout << "Number of leaf cells:  " << num_leaf_cells << endl;
    cout << endl;
#endif
#endif
    return 0;
}

#if  GENERATE_NEIGHBORS_STATICALLY
void fvoctree::generate_leaf_neighbor_lists()
{
    if (root) {
        root->generate_all_internal_leaf_neighbors();
    }
}
#endif
