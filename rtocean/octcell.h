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
#include "nlset.h"

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
    /* Geometry */

    // The cell is a cube with size s and the first corner in r
    pftype s; /* Size of the cell (the length of an edge) */
    pfvec  r; /* Position of the first corner */

    /* Navier-Stokes */
    pftype p; /* Reduced pressure = pressure/density */
    /*Since the velocities are located in the cell faces, they are stored in the octneighbors */

    /* Volume of fluid */
    pftype water_vol_coeff; /* [1] The volume the water in this cell would occupy at NORMAL_PRESSURE divided by the volume of the cell */
    pftype total_vol_coeff; /* [1] The volume the water and the air in this cell would occupy at NORMAL_PRESSURE divided by the volume of the cell (should stay relatively close to 1) */

    /* Level of detail */
    uint lvl; /* The level of the cell, 0 = root */
    //uint ila; /* Internal layer advancement, the advancement of the cell in the layer in terms of cells: 1, 2, ..., t_n (0 = unknown) */
    //bool changed; /* Whether the ila has changed since last update or not */

    /* Children */
    octcell **_c; /* The possible children */

    /* Neighbors */
    //nlist leaf_neighbor_list; /* Lists all leaf neighbors. This list is empty if this is a parent cell. */
    //nlist coarse_neighbor_list; /* These connections are between cells on the same level and involves at least one non-leaf cell. */
    /*
     * This array contains neighbor lists that together contain all neighbors, leaf cell and parent cells,
     * that differ no more than one level of detail from this cell
     */
    nlist neighbor_lists[NUM_NEIGHBOR_LISTS];

public:
    /*****************************
     * Public non-static methods *
     *****************************/

    /* Geometry */
    pfvec  get_cell_center();
    pftype get_side_area();
    pftype get_cube_volume();
    pftype get_volume_of_water();

    /* Simulation */
    bool is_water_cell();
    bool is_non_water_cell();
    bool is_air_cell();
    bool is_non_air_cell();
    bool is_mixed_cell();
    pftype get_air_volume_coefficient();
    pftype get_alpha();
    void set_volume_coefficients(pftype water_volume_coefficient, pftype total_volume_coefficient);

    /* Level of detail */

    /* Children */
    bool has_child_array();
    bool is_leaf();
    void make_leaf();
    octcell* get_child(uint idx);
    octcell* set_child(uint idx, octcell* child);
    uint get_number_of_children();
    void refine(); // Creates a new full child array
    void coarsen(); // Decreases the level of detail to this level by removing the children and the child array
    void remove_child(uint idx);

    /* Neighbors */
    void move_neighbor_connection_to_other_list(nlnode* node, uint new_list_index);
    void break_all_neighbor_connections();

public:
    /*************************
     * Public static methods *
     *************************/
    /* Neighbors */

    /* Indexes */
#if  NUM_DIMENSIONS == 2
    static uint child_index(uint dim0, uint dim1);
    static uint child_index_xy(uint x, uint y);
#elif  NUM_DIMENSIONS == 3
    static uint child_index(uint dim0, uint dim1, uint dim2);
    static uint child_index_xyz(uint x, uint y, uint z);
#endif
    static uint child_index_offset(uint dim);
    static bool positive_direction_of_child(uint child_index, uint dim);
    static uint child_index_flip_direction(uint child_index, uint dim);
    static pftype cube_side_area(pftype side_length);
    static pftype cube_volume(pftype side_length);

private:
    /******************************
     * Private non-static methods *
     ******************************/
void make_neighbors(octcell* cell1, octcell* cell2, uint cell1_neighbor_list_idx, uint cell2_neighbor_list_idx, uint dimension, bool pos_dir);

private:
    /**************************
     * Private static methods *
     **************************/
static void un_neighbor(nlnode* list_entry);

private:
    /*************************
     * Disabled constructors *
     *************************/
    octcell(); // Default constructor prevented from all use
    octcell(octcell&); // Copy constructor prevented from all use
};

////////////////////////////////////////////////////////////////
// INLINE NON-STATIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////

/************
 * Geometry *
 ************/

inline
pfvec octcell::get_cell_center()
{
    pftype s_2 = 0.5 * s;
    //TODO: Create function for generating the vector added to r
#if    NUM_DIMENSIONS == 2
    return r + pfvec(s_2, s_2);
#elif  NUM_DIMENSIONS == 3
    return r + pfvec(s_2, s_2, s_2);
#endif
}

inline
pftype octcell::get_side_area()
{
    return cube_side_area(s);
}

inline
pftype octcell::get_cube_volume()
{
    return cube_volume(s);
}

/**************
 * Simulation *
 **************/

inline
bool octcell::is_water_cell() {
    return water_vol_coeff >= total_vol_coeff;
}

inline
bool octcell::is_non_water_cell() {
    return !is_water_cell();
}

inline
bool octcell::is_air_cell() {
    return water_vol_coeff <= 0;
}

inline
bool octcell::is_non_air_cell() {
    return !is_air_cell();
}

inline
bool octcell::is_mixed_cell() {
    return is_non_water_cell() && is_non_air_cell();
}

inline
pftype octcell::get_air_volume_coefficient() {
    return total_vol_coeff - water_vol_coeff;
}

inline
pftype octcell::get_alpha() {
    return water_vol_coeff/total_vol_coeff;
}

inline
void octcell::set_volume_coefficients(pftype water_volume_coefficient, pftype total_volume_coefficient)
{
#if  DEBUG
    if (IS_NAN(water_volume_coefficient)) {
        throw logic_error("Trying to set a NaN water_volume_coefficient in cell");
    }
    if (IS_NAN(total_volume_coefficient)) {
        throw logic_error("Trying to set a NaN total_volume_coefficient in cell");
    }
    if (water_volume_coefficient < 0) {
        throw logic_error("Trying to set a negative water_volume_coefficient in cell");
    }
    if (total_volume_coefficient < 0) {
        throw logic_error("Trying to set a negative total_volume_coefficient in cell");
    }
    if (water_volume_coefficient > total_volume_coefficient) {
        throw logic_error("Trying to set a higher water_volume_coefficient than total_volume_coefficient in cell");
    }
#endif
    water_vol_coeff = water_volume_coefficient;
    total_vol_coeff = total_volume_coefficient;
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
#if  DEBUG
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
#if  DEBUG
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
uint octcell::get_number_of_children() {
#if  DEBUG
    if (is_leaf()) {
        throw logic_error("Trying to get number of children for a leaf cell");
    }
#endif
    uint num = 0;
    for (uint idx = 0; idx < MAX_NUM_CHILDREN; idx++) {
        if (get_child(idx)) {
            num++;
        }
    }
    return num;
}

inline
void octcell::remove_child(uint idx)
{
    octcell* c = get_child(idx);
#if  DEBUG
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
    set_child(idx, 0);
}

////////////////////////////////////////////////////////////////
// INLINE STATIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////

/*************
 * Neighbors *
 *************/

inline
void octcell::un_neighbor(nlnode* list_entry)
{
    list_entry->v.cnle->remove_from_list_and_delete();
    list_entry->remove_from_list_and_delete();
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
bool octcell::positive_direction_of_child(uint child_index, uint dim)
{
    return (child_index >> dim) & 1;
}

inline
uint octcell::child_index_flip_direction(uint child_index, uint dim)
{
    return child_index ^ (1 << dim);
}

/* Geometry */

inline
pftype octcell::cube_side_area(pftype side_length)
{
    return inline_int_pow(side_length, NUM_DIMENSIONS-1);
}

inline
pftype octcell::cube_volume(pftype side_length)
{
    return inline_int_pow(side_length, NUM_DIMENSIONS);
}

#endif // OCTCELL_H
