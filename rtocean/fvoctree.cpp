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
    prepare_cells_for_water_recursively(c);
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
        return SURFACE_ACCURACY + (SURFACE_HEIGHT - r.e[VERTICAL_DIMENSION])
                * (1/(MIN_LOD_LAYER_THICKNESS + 0.5));
    }
    else {
        return SURFACE_ACCURACY;
    }
}

/* Returns true if the cell should be removed from the simulation */
bool fvoctree::refine_subtree(octcell* c, pftype surface, pftype bottom, pftype (*accuracy_function)(pfvec))
{

    static int tot_num_cells = 1;
    static int num_leaf_cells = 1;
    pftype s = c->s;

    pftype lowest_cell_height = c->r[VERTICAL_DIMENSION];
#if    NUM_DIMENSIONS == 2
#if 0
    pftype hdiff = .24118956;
    pftype local_surface_height = c->get_cell_center().e[HORIZONTAL_DIMENSION1] > .5 ? SURFACE_HEIGHT + hdiff/2 : SURFACE_HEIGHT - hdiff/2;
    pftype min_surf_height = local_surface_height;
    pftype max_surf_height = local_surface_height;
#else
#if 1
    pftype left_edge_height = 0.5501;
    pftype slope = 0.4;
#else
    pftype left_edge_height = 0.9501;
    pftype slope = -0.4;
#endif
    pftype min_surf_height = left_edge_height + slope*(c->r[HORIZONTAL_DIMENSION1] + (slope < 0 ? s : 0));
    pftype max_surf_height = left_edge_height + slope*(c->r[HORIZONTAL_DIMENSION1] + (slope > 0 ? s : 0));
#endif
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
    if (s <= accuracy_function(c->get_cell_center())) { // Accuracy is good enough, stop refining
#endif
        /* Stop refining */
        /* Calculate properties */
        pftype beta; /* Water volume divided by the volume of the cell */
        pftype mean_height = lowest_cell_height + s/2;
        pftype mean_surface_height = (min_surf_height + max_surf_height)/2;
        c->p = NORMAL_PRESSURE + (mean_surface_height - mean_height) * (P_G * P_WATER_DENSITY);
        if (lowest_cell_height + s > min_surf_height) {
            /* Cell is a surface cell */
            /* Estimate beta */
            pftype mean_height_diff = (MAX(min_surf_height - lowest_cell_height, 0) + MIN(max_surf_height - lowest_cell_height, s))/2;
            beta = mean_height_diff / c->s;
        }
        else {
            /* Cell is not a surface cell and contains only water */
            beta = 1;
        }
        pftype water_vol_coeff = 1 + (c->p - NORMAL_PRESSURE)*(1/ARTIFICIAL_COMPRESSIBILITY_FACTOR);
#if  NO_ATMOSPHERE
        pftype air_vol_coeff = water_vol_coeff;
#else
        pftype air_vol_coeff = c->p / NORMAL_PRESSURE;
#endif
        water_vol_coeff *= beta;
        air_vol_coeff   *= (1 - beta);
#if  DEBUG
        if (water_vol_coeff < 0) {
            NO_OP();
        }
#endif
        c->set_volume_coefficients(water_vol_coeff, water_vol_coeff + air_vol_coeff);
        return false;
    }
    // Cell is not fine enough, refine it and then handle the children recursivelly
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
    if (!c->get_number_of_children()) {
        throw logic_error("Cell has no children and is not a leaf cell, yet it is allowed to exist");
    }
#endif
    return 0;
}

void fvoctree::prepare_cells_for_water_recursively(octcell* cell)
{
    if (cell->is_leaf()) {
        cell->prepare_for_water();
        return;
    }
    for (uint i = 0; i < octcell::MAX_NUM_CHILDREN; i++) {
        if (cell->get_child(i)) {
            prepare_cells_for_water_recursively(cell->get_child(i));
        }
    }
}
