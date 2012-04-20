#ifndef OCTCELL_H
#define OCTCELL_H

////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

// Standard includes
#include <stdexcept>
using std::exception;
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
    static const uint MAX_NUM_CHILDREN = 1 << NUM_DIMENSIONS;
public:
    /*******************************
     * Constructors and destructor *
     *******************************/
    octcell(pftype size, pfvec pos, uint level, uint internal_layer_advancement = 0);
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
    //TODO: Change tho vector instead of separate coordinates
#if 1
    pfvec  r; /* Position of the first corner */
#else
    pftype x; /* X-position of first corner */
    pftype y; /* Y-position of first corner */
#if  NUM_DIMENSIONS == 3
    pftype z; /* Z-position of first corner */
#endif
#endif

    /* Level of detail */
    uint lvl; /* The level of the cell, 0 = root */
    //uint ila; /* Internal layer advancement, the advancement of the cell in the layer in terms of cells: 1, 2, ..., t_n (0 = unknown) */
    //bool changed; /* Whether the ila has changed since last update or not */

    /* Children */
    octcell **_c; /* The possible children */

    /* Neighbors */
    nlist leaf_neighbor_list; /* Lists all leaf neighbors. This list is empty if this is a parent cell. */
    nlist coarse_neighbor_list; /* These connections are between cells on the same level and involves at least one non-leaf cell. */

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
    void make_leaf();
    octcell* get_child(uint idx);
    octcell* set_child(uint idx, octcell* child);
    void refine(); // Creates new child array and new children
    void coarsen(); // Decreases the level of detail ro this level
    //void unleaf(); // Creates new child array but no children
    //octcell* add_child(uint idx);
    void remove_child_and_neighbor_connections(uint idx);
    void remove_child(uint idx);

    /* Neighbors */
#if  GENERATE_NEIGHBORS_STATICALLY
    void generate_all_internal_leaf_neighbors();
#endif
    nlnode* get_first_leaf_neighbor_list_node();
    nlnode* get_first_coarse_neighbor_list_node();
    void remove_all_leaf_neighbor_connections();
    void remove_all_coarse_neighbor_connections();

public:
    /*************************
     * Public static methods *
     *************************/
    /* Neighbors */
    static void make_leaf_neighbors(octcell* c1, octcell* c2, uint dimension, uint lowest_level);
    static void make_coarse_neighbors(octcell* c1, octcell* c2, uint dimension, uint lowest_level);
#if  GENERATE_NEIGHBORS_STATICALLY
    static void generate_all_cross_cell_leaf_neighbors(octcell* c1, octcell* c2, uint normal_dimension, uint lowest_level);
#endif
    static void un_leaf_neighbor(nlnode* leaf_neighbor_list_entry);
    static void un_coarse_neighbor(nlnode* coarse_neighbor_list_entry);

    /* Indexes */
#if  NUM_DIMENSIONS == 2
    static uint child_index(uint dim0, uint dim1);
    static uint child_index_xy(uint x, uint y);
#elif  NUM_DIMENSIONS == 3
    static uint child_index(uint dim0, uint dim1, uint dim2);
    static uint child_index_xyz(uint x, uint y, uint z);
#endif
    static uint child_index_offset(uint dim);
    static uint direction(uint dim, bool rev_dir);
    static uint dimension(uint dir);
    static bool reverse_direction(uint dir);
    static bool positive_direction_of_child(uint child_index, uint dim);

private:
    /******************************
     * Private non-static methods *
     ******************************/
void steal_child_leaf_neighbor_connection(nlnode* child_node);
void make_leaf_neighbor_coarse_neighbor(nlnode* list_entry);
void make_leaf_neighbor_coarse_neighbor_or_throw_away(nlnode* list_entry);

private:
    /**************************
     * Private static methods *
     **************************/
static void _un_neighbor(nlnode* list_entry);

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
 * Geometry *
 ************/

inline
pfvec octcell::cell_center()
{
    pftype s_2 = 0.5 * s;
    //TODO: Create function for generating the vector added to r
#if    NUM_DIMENSIONS == 2
    return r + pfvec(s_2, s_2);
#elif  NUM_DIMENSIONS == 3
    return r + pfvec(s_2, s_2, s_2);
#endif
}

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
void octcell::make_leaf()
{
#if  DEBUG
    if (is_leaf()) {
        throw logic_error("Trying to make a leaf cell a leaf");
    }
    for (uint i = 0; i < MAX_NUM_CHILDREN; i++) {
        if (get_child(i)) {
            throw logic_error("Trying to make a cell with at least one child a leaf cell");
        }
    }
#endif
    delete[] _c;
    _c = 0;
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

inline
void octcell::remove_child(uint idx)
{
    octcell* c = get_child(idx);
#if DEBUG
    if (is_leaf()) {
        throw logic_error("Trying to remove a child cell from a leaf cell");
    }
    if (idx < 0 || idx >= MAX_NUM_CHILDREN) {
        throw out_of_range("Trying to remove a child cell with index out of bound");
    }
    if (!c) {
        throw logic_error("Trying to remove a child cell that does not exist");
    }
#endif

    /* Remove child */
    delete c;
    c = set_child(idx, 0);
}

inline
void octcell::remove_child_and_neighbor_connections(uint idx)
{
    octcell* c = get_child(idx);
#if DEBUG
    if (is_leaf()) {
        throw logic_error("Trying to remove a child cell from a leaf cell");
    }
    if (idx < 0 || idx >= MAX_NUM_CHILDREN) {
        throw out_of_range("Trying to remove a child cell with index out of bound");
    }
    if (!c) {
        throw logic_error("Trying to remove a child cell that does not exist");
    }
#endif

    c->remove_all_leaf_neighbor_connections();
    c->remove_all_coarse_neighbor_connections();

    /* Remove child */
    delete c;
    c = set_child(idx, 0);
}

/*************
 * Neighbors *
 *************/

inline
nlnode* octcell::get_first_leaf_neighbor_list_node()
{
    return leaf_neighbor_list.get_first_element();
}

inline
nlnode* octcell::get_first_coarse_neighbor_list_node()
{
    return coarse_neighbor_list.get_first_element();
}

inline
void octcell::un_leaf_neighbor(nlnode* leaf_neighbor_list_entry)
{
    _un_neighbor(leaf_neighbor_list_entry);
}

inline
void octcell::un_coarse_neighbor(nlnode* coarse_neighbor_list_entry)
{
    _un_neighbor(coarse_neighbor_list_entry);
}

inline
void octcell::_un_neighbor(nlnode* list_entry)
{
    list_entry->v.cnle->remove_from_list_and_delete();
    list_entry->remove_from_list_and_delete();
}

inline
void octcell::remove_all_leaf_neighbor_connections()
{
    nlnode* current_node;
    nlnode* next_node;
    for (current_node = get_first_leaf_neighbor_list_node(); current_node; current_node = next_node) {
        next_node = current_node->get_next_node();
        un_leaf_neighbor(current_node);
    }
}

inline
void octcell::remove_all_coarse_neighbor_connections()
{
    nlnode* current_node;
    nlnode* next_node;
    for (current_node = get_first_coarse_neighbor_list_node(); current_node; current_node = next_node) {
        next_node = current_node->get_next_node();
        un_leaf_neighbor(current_node);
    }
}

/***********
 * Indexes *
 ***********/

inline
#if  NUM_DIMENSIONS == 2
uint octcell::child_index(uint dim0, uint dim1)
#elif  NUM_DIMENSIONS == 3
uint octcell::child_index(uint dim0, uint dim1, uint dim2)
#endif
{
#if    NUM_DIMENSIONS == 2
    return (dim0 << 0) | (dim1 << 1);
#elif  NUM_DIMENSIONS == 3
    return (dim0 << 0) | (dim1 << 1) | (dim2 << 2);
#endif
}

inline
#if  NUM_DIMENSIONS == 2
uint octcell::child_index_xy(uint x, uint y)
#elif  NUM_DIMENSIONS == 3
uint octcell::child_index_xyz(uint x, uint y, uint z)
#endif
{
#if    NUM_DIMENSIONS == 2
    return (x << DIM_X) | (y << DIM_Y);
#elif  NUM_DIMENSIONS == 3
    return (x << DIM_X) | (y << DIM_Y) | (z << DIM_Z);
#endif
}

inline
uint octcell::child_index_offset(uint dim)
{
    return 1 << dim;
}

inline
uint octcell::direction(uint dim, bool rev_dir)
{
    return (dim << 1) | rev_dir;
}

inline
uint octcell::dimension(uint dir)
{
    return dir >> 1;
}

inline
bool octcell::reverse_direction(uint dir)
{
    return dir & 1;
}

inline
bool octcell::positive_direction_of_child(uint child_index, uint dim)
{
    return (child_index >> dim) & 1;
}

#endif // OCTCELL_H
