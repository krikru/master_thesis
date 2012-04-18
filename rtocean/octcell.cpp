
////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

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
    _c = children;
}

octcell::~octcell()
{
    if (has_child_array()) {
        for (uint i = 0; i < MAX_NUM_CHILDREN; i++) {
            if (get_child(i)) {
                delete get_child(i);
            }
        }
        delete _c;
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
    //TODO: Stop making tests!!
#if XYZ_COORDINATE_SYSTEM
    return pfvec3(x + s_2, y + s_2, z + s_2);
#else
    pfvec3 cc;
    cc[DIR_X] = x + s_2;
    cc[DIR_Y] = y + s_2;
    cc[DIR_Z] = z + s_2;
    return cc;
#endif
}

/*******************
 * Level of detail *
 *******************/

/************
 * Children *
 ************/

void octcell::refine()
{
#if DEBUG
    if (has_child_array()) {
        throw logic_error("Trying to refine a cell that already has child array");
    }
#endif

    // Create child array
    _c = new octcell*[MAX_NUM_CHILDREN];
    // Create new values for children
    pftype s_2 = 0.5 * s;
    pftype x_2 = x + s_2;
    pftype y_2 = y + s_2;
    pftype z_2 = z + s_2;
    uint   new_level = lvl + 1;
    // Assign values to children
    set_child(0, new octcell(s_2, x  , y  , z  , new_level));
    set_child(1, new octcell(s_2, x_2, y  , z  , new_level));
    set_child(2, new octcell(s_2, x  , y_2, z  , new_level));
    set_child(3, new octcell(s_2, x_2, y_2, z  , new_level));
    set_child(4, new octcell(s_2, x  , y  , z_2, new_level));
    set_child(5, new octcell(s_2, x_2, y  , z_2, new_level));
    set_child(6, new octcell(s_2, x  , y_2, z_2, new_level));
    set_child(7, new octcell(s_2, x_2, y_2, z_2, new_level));
}

void octcell::unleaf()
{
#if DEBUG
    if (has_child_array()) {
        throw logic_error("Trying to unleaf a cell that already has child array");
    }
#endif

    // Create children
    _c = new octcell*[MAX_NUM_CHILDREN];
    for (uint i = 0; i < MAX_NUM_CHILDREN; i++) {
        set_child(i, 0);
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
    if (get_child(idx)) {
        throw logic_error("Trying to add a child cell that already exist");
    }
#endif

    pftype s_2 = 0.5 * s;
    pftype x1 = x + ((idx >> DIR_X) & 1) * s_2;
    pftype y1 = y + ((idx >> DIR_Y) & 1) * s_2;
    pftype z1 = z + ((idx >> DIR_Z) & 1) * s_2;

    set_child(idx, new octcell(s_2, x1, y1, z1, lvl + 1));

    return get_child(idx);
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
    if (!get_child(idx)) {
        throw logic_error("Trying to remove a child cell that does not exist");
    }
#endif

    delete get_child(idx);
    set_child(idx, 0);
}

void octcell::generate_all_internal_neighbors()
{
    if (is_leaf()) {
        return;
    }
    //TODO: Optimize:
    for (uint i1 = 0; i1 < MAX_NUM_CHILDREN; i1++) {
        if (get_child(i1)) {
            /* Child exists */
            /* Generate all internal neighbors in the child cell */
            get_child(i1)->generate_all_internal_neighbors();
            /* Generate all cross-child cell neighbors from this child to children with larger indexes */
            for (uint dir = 0; dir < NUM_DIRECTIONS; dir++) {
                if (i1 >> dir & 1) {
                    /* There is no neighbor in this direction */
                    continue;
                }
                uint i2 = i1 + index_offset(dir);
                if (i2 < MAX_NUM_CHILDREN && get_child(i2)) {
                    /* Second child also exists, generate neighbors in interface */
                    generate_all_cross_cell_neighbors(get_child(i1), get_child(i2), dir);
                }
            }
        }
    }
}

nlnode* octcell::get_first_neighbor_list_node()
{
    return neighborlist.h;
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
    node1->v.initialize(node1, c2,  dist, dist_abs, area);
    node2->v.initialize(node2, c1, -dist, dist_abs, area);
}

void octcell::generate_all_cross_cell_neighbors(octcell* c1, octcell* c2, uint normal_direction)
{
    // The normal direction will point from c1 to c2 (not the other way around)
#if  DEBUG
    // Control cell sizes
    if (c1 == c2) {
        throw logic_error("Trying to generate neighbors between a cell and itself");
    }
    if (c1->lvl != c2->lvl) {
        //throw logic_error("Trying to build generate neighbors between cells of different sizes");
    }
    // Control distance
    pftype sqr_dist = (c2->cell_center() - c1->cell_center()).sqr_length();
    pftype goal_sqrt_dist = c1->s * c1->s;
    if (sqr_dist > 1.5 * goal_sqrt_dist) {
        //throw logic_error("Trying to generate neighbors between unconnected cells");
    }
    if (sqr_dist < 0.75 * goal_sqrt_dist) {
        //throw logic_error("Trying to generate neighbors between intersecting cells");
    }
#endif

    if (c1->is_leaf() && c2->is_leaf()) {
        // Both nodes are leaf nodes, add them to each others neighbor lists
        make_neighbors(c1, c2);
    }
    else if (c1->is_leaf() && !(c2->is_leaf())) {
        // TODO: Optimize
        for (uint i = 0; i < MAX_NUM_CHILDREN; i++) {
            if ((i >> normal_direction & 1) == 0) {
                /* Potential child cell with this index in c2 borders to c1 */
                if (c2->get_child(i)) {
                    /* Child exists */
                    if (c2->get_child(i)->has_child_array()) {
                        generate_all_cross_cell_neighbors(c1, c2->get_child(i), normal_direction);
                    }
                    else {
                        make_neighbors(c1, c2->get_child(i));
                    }
                }
            }
        }
    }
    else if (!(c1->is_leaf()) && c2->is_leaf()) {
        /* The children of c1 will be leaf nodes */
        // TODO: Optimize
        for (uint i = 0; i < MAX_NUM_CHILDREN; i++) {
            if (i >> normal_direction & 1) {
                /* Potential child cell with this index in c1 borders to c2 */
                if (c1->get_child(i)) {
                    /* Child exists */
                    if (c1->get_child(i)->has_child_array()) {
                        generate_all_cross_cell_neighbors(c1->get_child(i), c2, normal_direction);
                    }
                    else {
                        make_neighbors(c1->get_child(i), c2);
                    }
                }
            }
        }
    }
    else {
        /* Both c1 and c2 have children */
        // TODO: Optimize
        uint dir_offset = 1 << normal_direction;
        for (uint i = 0; i < MAX_NUM_CHILDREN; i++) {
            if (i >> normal_direction & 1) {
                /* Potential child cell with this index in c1 borders to c2 */
                if (c1->get_child(i) && c2->get_child(i-dir_offset)) {
                    /* Children exist in both c1 and c2 */
                    generate_all_cross_cell_neighbors(c1->get_child(i), c2->get_child(i-dir_offset), normal_direction);
                }
            }
        }
    }
}

