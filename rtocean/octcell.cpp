
////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

// Own includes
#include "octcell.h"

////////////////////////////////////////////////////////////////
// CONSTRUCTORS AND DESTRUCTOR
////////////////////////////////////////////////////////////////

octcell::octcell(pftype size, pfvec pos, uint level, uint internal_layer_advancement, octcell **children) :
    neighborlist()
{
    s = size;
    r = pos;
    lvl = level;
    //ila = internal_layer_advancement;
    internal_layer_advancement = internal_layer_advancement;
    _c = children;
}

octcell::~octcell()
{
    /* Remove neighbor connections */
    nlnode* node = get_first_neighbor_list_node();
    nlnode* next_node;
    for (; node; node = next_node) {
        next_node = node->get_next_node();
        unneighbor(node);
    }

    /* Delete potential children */
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
    pftype s_2 = 0.5*s;
    pfvec corners[2];
    corners[0] = r;
    corners[1] = cell_center();
    uint   new_level = lvl + 1;

    // Assign values to children
    pfvec new_r;
    for (uint i = 0; i < MAX_NUM_CHILDREN; i++) {
        for (uint dir = 0; dir < NUM_DIMENSIONS; dir++) {
            new_r[dir] = corners[(i >> dir) & 1][dir];
        }
        set_child(i, new octcell(s_2, new_r, new_level));
    }

#if  GENERATE_NEIGHBORS_DYNAMICALLY

    /* Create new neighbors between child cells */
    for (uint i1 = 0; i1 < MAX_NUM_CHILDREN; i1++) {
        for (uint dir = 0; dir < NUM_DIMENSIONS; dir++) {
            uint i2 = i1 | index_offset(dir);
            if (i2 >= MAX_NUM_CHILDREN || i2 < 0) {
                i2 = i2;
            }
            if (i2 > i1) {
                make_neighbors(get_child(i1), get_child(i2), dir);
            }
        }
    }

    /* Refine existing neighbors */
    nlnode* node = get_first_neighbor_list_node();
    nlnode* next_node;
    /* Loop through all neighbors on the parent (this) level */
    for (; node; node = next_node) {
        uint dir = node->v.dir;
        bool rev_dir = node->v.rev_dir;
        octcell* neighbor = node->v.n;
        if (neighbor->lvl <= lvl) {
            /* Neighbor cell is at least as large as this cell */
            /* Make half on the child cells (on the correct side of the cell) neighbors with this neighbor */
            // TODO: Optimize loop
            for (uint i = 0; i < MAX_NUM_CHILDREN; i++) {
                /* The direction between the neighbors is reversed */
                if ((i >> dir & 1) ^ rev_dir) {
                    /* Child cell is adjactent */
                    if (rev_dir) {
                        make_neighbors(neighbor, get_child(i), dir);
                    }
                    else {
                        make_neighbors(get_child(i), neighbor, dir);
                    }
                }
            }
        }
        else {
            /* Neighbor cell is smaller than this cell, only one of the child cells get this as a neighbor */
            /* Calculate index of neighbor cell */
            uint idx = 0;
            for (uint dir2 = 0; dir2 < NUM_DIMENSIONS; dir2++) {
                if (dir2 == dir) {
                    /* dir2 is parallel to dir */
                    idx |= (!(rev_dir) << dir2);
                }
                else {
                    /* dir2 is orthogonal to dir */
                    idx |= ((cell_center()[dir2] < neighbor->cell_center()[dir2]) << dir2);
                }
            }
            /* Index calculated, make neighbor */
            if (rev_dir) {
                make_neighbors(neighbor, get_child(idx), dir);
            }
            else {
                make_neighbors(get_child(idx), neighbor, dir);
            }
        }
        next_node = node->get_next_node();
        unneighbor(node);
    }
#endif
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
    pfvec r1;
    for (uint dir = 0; dir < NUM_DIMENSIONS; dir++) {
        r1[dir] = r[dir] + ((idx >> dir) & 1) * s_2;
    }
    set_child(idx, new octcell(s_2, r1, lvl + 1));

    return get_child(idx);
}

#if  GENERATE_NEIGHBORS_STATICALLY
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
            for (uint dir = 0; dir < NUM_DIMENSIONS; dir++) {
                uint i2 = i1 | index_offset(dir);
                if (i2 > i1 && get_child(i2)) {
                    /* Second child also exists, generate neighbors in interface */
                    generate_all_cross_cell_neighbors(get_child(i1), get_child(i2), dir);
                }
            }
        }
    }
}
#endif

////////////////////////////////////////////////////////////////
// PUBLIC STATIC METHODS
////////////////////////////////////////////////////////////////

void octcell::make_neighbors(octcell* c1, octcell* c2, uint direction)
{
    nlnode* node1 = c1->neighborlist.add_new_element();
    nlnode* node2 = c2->neighborlist.add_new_element();
    pfvec dist = c2->cell_center() - c1->cell_center();
    pftype dist_abs = dist.length();
    pftype min_s = MIN(c1->s, c2->s);
    pftype area = min_s*min_s;
    node1->v.initialize(node1, node2, c2,  dist, dist_abs, area, direction, false);
    node2->v.initialize(node2, node1, c1, -dist, dist_abs, area, direction, true );
}

#if  GENERATE_NEIGHBORS_STATICALLY
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

    if (c1->is_leaf() && c2->is_leaf()) { /* CASE 1 */
        // Both nodes are leaf nodes, add them to each others neighbor lists
        make_neighbors(c1, c2, normal_direction);
    }
    else if (c1->is_leaf() && !(c2->is_leaf())) { /* CASE 2 */
        /* c1 is a leaf cell but c2 has child cells */
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
                        make_neighbors(c1, c2->get_child(i), normal_direction);
                    }
                }
            }
        }
    }
    else if (!(c1->is_leaf()) && c2->is_leaf()) {  /* CASE 3 */
        /* c2 is a leaf cell but c1 has child cells */
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
                        make_neighbors(c1->get_child(i), c2, normal_direction);
                    }
                }
            }
        }
    }
    else {  /* CASE 4 */
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
#endif

