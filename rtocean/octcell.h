#ifndef OCTCELL_H
#define OCTCELL_H

////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

// Standard includes
#include <stdexcept>
//using std::exception;
using std::logic_error;
using std::out_of_range;

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
    static const uint MAX_NUM_CHILDREN = 1 << NUM_DIRECTIONS;
public:
    /*******************************
     * Constructors and destructor *
     *******************************/
#if  NUM_DIRECTIONS == 2
    octcell(pftype size, pftype x_pos, pftype y_pos, uint level, uint internal_layer_advancement = 0, octcell **children = 0);
#elif  NUM_DIRECTIONS == 3
    octcell(pftype size, pftype x_pos, pftype y_pos, pftype z_pos, uint level, uint internal_layer_advancement = 0, octcell **children = 0);
#endif
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
#if  NUM_DIRECTIONS == 3
    pftype z; /* Z-position of first corner */
#endif

    /* Level of detail */
    uint lvl; /* The level of the cell, 0 = root */
    //uint ila; /* Internal layer advancement, the advancement of the cell in the layer in terms of cells: 1, 2, ..., t_n (0 = unknown) */
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
    pfvec  cell_center();

    /* Level of detail */

    /* Children */
    bool has_child_array();
    bool is_leaf();
    octcell* get_child(uint idx);
    octcell* set_child(uint idx, octcell* child);
    void refine(); // Creates new child array and new children
    void unleaf(); // Creates new child array but no children
    octcell* add_child(uint idx);
    void remove_child(uint idx);

    /* Neighbors */
    void generate_all_internal_neighbors();
    nlnode* get_first_neighbor_list_node();
    void unneighbor(nlnode* neighbor_list_entry);

public:
    /*************************
     * Public static methods *
     *************************/
    /* Neighbors */
    void make_neighbors(octcell* c1, octcell* c2, uint direction);
    void generate_all_cross_cell_neighbors(octcell* c1, octcell* c2, uint normal_direction);

    /* Indexes */
#if  NUM_DIRECTIONS == 2
    static uint child_index(uint x, uint y);
#elif  NUM_DIRECTIONS == 3
    static uint child_index(uint x, uint y, uint z);
#endif
    static uint index_offset(uint dir);

private:
    /*************************
     * Disabled constructors *
     *************************/
    octcell(); // Default constructor prevented from all use
    octcell(octcell&); // Copy constructor prevented from all use
};

////////////////////////////////////////////////////////////////
// INLINE MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////

/************
 * Children *
 ************/

inline
bool octcell::has_child_array()
{
    return _c;
}

inline
bool octcell::is_leaf()
{
    return !_c;
}

inline
octcell* octcell::get_child(uint idx) {
#if DEBUG
    if (is_leaf()) {
        throw logic_error("Trying to get a child from a leaf cell");
    }
    if (idx >= MAX_NUM_CHILDREN) {
        throw out_of_range("Trying to get a child with an index that is too high");
    }
#endif
    return _c[idx];
}

inline
octcell* octcell::set_child(uint idx, octcell* child) {
#if DEBUG
    if (is_leaf()) {
        throw logic_error("Trying to set a child for a leaf cell");
    }
    if (idx >= MAX_NUM_CHILDREN) {
        throw out_of_range("Trying to get a child with an index that is too high");
    }
#endif
    return _c[idx] = child;
}

/***********
 * Indexes *
 ***********/

inline
#if  NUM_DIRECTIONS == 2
uint octcell::child_index(uint x, uint y)
#elif  NUM_DIRECTIONS == 3
uint octcell::child_index(uint x, uint y, uint z)
#endif
{
#if    NUM_DIRECTIONS == 2
    return (x << DIR_X) | (y << DIR_Y);
#elif  NUM_DIRECTIONS == 3
    return (x << DIR_X) | (y << DIR_Y) | (z << DIR_Z);
#endif
}

inline
uint octcell::index_offset(uint dir)
{
    return 1 << dir;
}

#endif // OCTCELL_H
