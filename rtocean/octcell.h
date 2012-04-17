#ifndef OCTCELL_H
#define OCTCELL_H

////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

// Own includes
#include "definitions.h"
#include "octneighbor.h"

////////////////////////////////////////////////////////////////
// CLASS DEFINITION
////////////////////////////////////////////////////////////////

/*
 * The indexes of the children cells (if existing) are calculated
 * as 1*ix + 2*iy + 4*iz, where ix, iy and iz are either 0 or 1
 * depending on the relative (to the parent cell) x, y and z
 * positions respectivelly.
 */
class octcell
{
public:
    static const uint MAX_NUM_CHILDREN = 8;
public:
    /*******************************
     * Constructors and destructor *
     *******************************/
    octcell(pftype size, pftype x_pos, pftype y_pos, pftype z_pos, uint level, uint internal_layer_advancement = 0, octcell **children = 0);
    ~octcell();

public:
    /***************************
     * Public member variables *
     ***************************/
    /*
     * Geometry
     * --------
     *
     * The cell is a cube that stretches from (x, y, z) to (x + s, y + s, z + s)
     */
    pftype s; /* Size of the cell (the length of an edge) */
    pftype x; /* X-position of first corner */
    pftype y; /* Y-position of first corner */
    pftype z; /* Z-position of first corner */

    /* Level of detail */
    uint lvl; /* The level of the cell, 0 = root */
    uint ila; /* Internal layer advancement, the advancement of the cell in the layer in terms of cells: 1, 2, ..., t_n (0 = unknown) */
    //bool changed; /* Whether the ila has changed since last update or not */

    /* Children */
    octcell **_c; /* The possible children */

    /* Neighbors */
    nlist neighborlist;

public:
    /*****************************
     * Public non-static methods *
     *****************************/

    /* Geometry */
    pfvec3 cell_center();

    /* Level of detail */

    /* Children */
    inline bool has_child_array();
    inline bool is_leaf();
    inline octcell* get_child(uint idx);
    inline octcell* set_child(uint idx, octcell* child);
    void refine(); // Creates new child array and new children
    void unleaf(); // Creates new child array but no children
    octcell* add_child(uint idx);
    void remove_child(uint idx);

    /* Neighbors */
    void generate_all_internal_neighbors();
    nlnode* get_first_neighbor_list_node();

public:
    /*************************
     * Public static methods *
     *************************/
    /* Neighbors */
    void make_neighbors(octcell* c1, octcell* c2);
    void generate_all_cross_cell_neighbors(octcell* c1, octcell* c2, uint normal_direction);

    /* Indexes */
    inline static uint child_index(uint x, uint y, uint z);
    inline static uint index_offset(uint dir);

private:
    /*************************
     * Disabled constructors *
     *************************/
    octcell(); // Default constructor prevented from all use
    octcell(octcell&); // Copy constructor prevented from all use
};

#endif // OCTCELL_H
