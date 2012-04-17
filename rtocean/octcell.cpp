
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

octcell::octcell(pftype size, pftype x_pos, pftype y_pos, pftype z_pos, uint level, uint internal_layer_advancement, octcell **children)
{
    s = size;
    x = x_pos;
    y = y_pos;
    z = z_pos;
    lvl = level;
    ila = internal_layer_advancement;
    c = children;
}

octcell::~octcell()
{
    if (c) {
        for (uint i = 0; i < MAX_NUM_CHILDREN; i++) {
            if (c[i]) {
                delete c[i];
            }
        }
        delete c;
    }
}

////////////////////////////////////////////////////////////////
// PUBLIC NON-STATIC METHODS
////////////////////////////////////////////////////////////////

/************
 * Geometry *
 ************/

pfvec3 octcell::cell_center()
{
    pftype s_2 = 0.5 * s;
    return pfvec3(x + s_2, y + s_2, z + s_2);
}

/*******************
 * Level of detail *
 *******************/

/************
 * Children *
 ************/

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
    uint   new_level = lvl + 1;
    // Assign values to children
    c[0] = new octcell(s_2, x  , y  , z  , new_level);
    c[1] = new octcell(s_2, x_2, y  , z  , new_level);
    c[2] = new octcell(s_2, x  , y_2, z  , new_level);
    c[3] = new octcell(s_2, x_2, y_2, z  , new_level);
    c[4] = new octcell(s_2, x  , y  , z_2, new_level);
    c[5] = new octcell(s_2, x_2, y  , z_2, new_level);
    c[6] = new octcell(s_2, x  , y_2, z_2, new_level);
    c[7] = new octcell(s_2, x_2, y_2, z_2, new_level);
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
    for (uint i = 0; i < MAX_NUM_CHILDREN; i++) {
        c[i] = 0;
    }
}


octcell* octcell::add_child(uint idx)
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
    pftype x1 = x + ((idx >> DIR_X) & 1) * s_2;
    pftype y1 = y + ((idx >> DIR_Y) & 1) * s_2;
    pftype z1 = z + ((idx >> DIR_Z) & 1) * s_2;

    c[idx] = new octcell(s_2, x1, y1, z1);

    return c[idx];
}

void octcell::remove_child(uint idx)
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

void octcell::generate_all_internal_neighbors()
{
    //TODO: Add DEBUG check around all cout's
#if DEBUG
    if (is_leaf()) {
        throw logic_error("Trying to generate beighbors within a leaf node");
    }
#endif
    //TODO: Optimize
    for (uint i1 = 0; i1 < MAX_NUM_CHILDREN; i1++) {
        if (c[i1]) {
            /* First child exists */
            for (DIRECTION dir = 0; dir < NUM_DIRECTIONS; dir++) {
                uint i2 = i1 + index_offset(dir);
                if (i2 < MAX_NUM_CHILDREN && c[i2]) {
                    /* Second child also exists, generate neighbors in interface */
                    generate_all_neighbors_in_interface(c1, c2, dir);
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////
// PUBLIC STATIC METHODS
////////////////////////////////////////////////////////////////

void octcell::make_neighbors(octcell* c1, octcell* c2)
{
    nlnode* node1 = c1->neighborlist.add_new_element();
    nlnode* node2 = c2->neighborlist.add_new_element();
    pfvec3 dist = c2->cell_center() - c1->cell_center();
    pftype dist_abs = dist.length();
    pftype min_s = MIN(c1->s, c2->s);
    pftype area = min_s*min_s;
    node1->v.n = c2;
    node2->v.n = c1;
    node1->v.dist =  dist;
    node2->v.dist = -dist;
    node1->v.dist_abs = dist_abs;
    node2->v.dist_abs = dist_abs;
    node1->v.cf_area = area;
    node2->v.cf_area = area;
}

void octcell::generate_all_neighbors_in_interface(octcell* c1, octcell* c2, DIRECTION normal_direction)
{
    // The normal direction will point from c1 to c2 (not the other way around)
#if  DEBUG
    // Control cell sizes
    if (c1->lvl != c2->lvl) {
        throw logic_error("Trying to build generate neighbors between cells of different sizes")
    }
#endif
    if (c1->is_leaf() && c2->is_leaf()) {
        // Both nodes are leaf nodes, add them to each others neighbor lists
        make_neighbors(c1, c2);
    }
    else if (c1->is_leaf() && !(c2->is_leaf())) {
        /* The children of c2 will be leaf nodes */
        c2->generate_all_internal_neighbors();
        // TODO: Optimize
        for (uint i = 0; i < MAX_NUM_CHILDREN; i++) {
            if ((i >> normal_direction & 1) == 0) {
                /* Potential child cell with this index in c2 borders to c1 */
                if (c2->c[i]) {
                    /* Child exists */
                    make_neighbors(c1, c2->c[i]);
                }
            }
        }
    }
    else if (!(c1->is_leaf()) && c2->is_leaf()) {
        /* The children of c1 will be leaf nodes */
        c1->generate_all_internal_neighbors();
        // TODO: Optimize
        for (uint i = 0; i < MAX_NUM_CHILDREN; i++) {
            if (i >> normal_direction & 1) {
                /* Potential child cell with this index in c1 borders to c2 */
                if (c1->c[i]) {
                    /* Child exists */
                    make_neighbors(c1->c[i], c2);
                }
            }
        }
    }
    else {
        /* Both c1 and c2 have children */
        c1->generate_all_internal_neighbors();
        c2->generate_all_internal_neighbors();
        // TODO: Optimize
        uint dir_offset = 1 << normal_direction;
        for (uint i = 0; i < MAX_NUM_CHILDREN; i++) {
            if (i >> normal_direction & 1) {
                /* Potential child cell with this index in c1 borders to c2 */
                if (c1->c[i] && c2->c[i-dir_offset]) {
                    /* Children exist in both c1 and c2 */
                    generate_all_neighbors_in_interface(c1->c[i], c2->c[i-dir_offset]);
                }
            }
        }
    }
}

uint octcell::child_index(uint x, uint y, uint z)
{
    return (x << DIR_X) | (y << DIR_Y) | (z << DIR_Z);
}

uint octcell::index_offset(DIRECTION dir)
{
    return 1 << dir;
}
