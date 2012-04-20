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
#if  DEBUG
    if (children) {
        throw logic_error("Creating cell with already generated children");
    }
#endif
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
#if  RUN_SAFE
    for (uint i = 0; i < MAX_NUM_CHILDREN; i++) {
        _c[i] = 0;
    }
#endif

    // Create new values for children
    pftype s_2 = 0.5*s;
    pfvec corners[2];
    corners[0] = r;
    corners[1] = cell_center();
    uint   new_level = lvl + 1;

    // Create children with new values
    pfvec new_r;
    for (uint i = 0; i < MAX_NUM_CHILDREN; i++) {
        for (uint dim = 0; dim < NUM_DIMENSIONS; dim++) {
            new_r[dim] = corners[(i >> dim) & 1][dim];
        }
        set_child(i, new octcell(s_2, new_r, new_level));
    }

#if  GENERATE_NEIGHBORS_DYNAMICALLY

    /* Create new neighbors between child cells */
    for (uint i1 = 0; i1 < MAX_NUM_CHILDREN; i1++) {
        for (uint dim = 0; dim < NUM_DIMENSIONS; dim++) {
            uint i2 = i1 | child_index_offset(dim);
            if (i2 >= MAX_NUM_CHILDREN || i2 < 0) {
                i2 = i2;
            }
            if (i2 > i1) {
                make_neighbors(get_child(i1), get_child(i2), dim, new_level);
            }
        }
    }

    /* Refine existing neighbors */
    nlnode* node = get_first_neighbor_list_node();
    nlnode* next_node;
    /* Loop through all neighbors on the parent (this) level */
    for (; node; node = next_node) {
        uint lowest_connection_level = node->v.low_lvl;
        uint dim = node->v.dim;
        bool rev_dir = node->v.rev_dir;
        octcell* neighbor = node->v.n;
        if (neighbor->lvl <= lvl) {
            /* Neighbor cell is at least as large as this cell */
            /* Make half on the child cells (on the correct side of the cell) neighbors with this neighbor */
            // TODO: Optimize loop
            for (uint i = 0; i < MAX_NUM_CHILDREN; i++) {
                /* The direction between the neighbors is reversed */
                if (positive_direction_of_child(i, dim) ^ rev_dir) {
                    /* Child cell is adjactent */
                    if (rev_dir) {
                        make_neighbors(neighbor, get_child(i), dim, lowest_connection_level);
                    }
                    else {
                        make_neighbors(get_child(i), neighbor, dim, lowest_connection_level);
                    }
                }
            }
        }
        else {
            /* Neighbor cell is smaller than this cell, only one of the child cells get this as a neighbor */
            /* Calculate index of neighbor cell */
            uint idx = 0;
            for (uint dim2 = 0; dim2 < NUM_DIMENSIONS; dim2++) {
                if (dim2 == dim) {
                    /* dim2 is parallel to dim */
                    idx |= (!(rev_dir) << dim2);
                }
                else {
                    /* dim2 is orthogonal to dim */
                    idx |= ((cell_center()[dim2] < neighbor->cell_center()[dim2]) << dim2);
                }
            }
            /* Index calculated, make neighbor */
            if (rev_dir) {
                make_neighbors(neighbor, get_child(idx), dim, lowest_connection_level);
            }
            else {
                make_neighbors(get_child(idx), neighbor, dim, lowest_connection_level);
            }
        }
        next_node = node->get_next_node();
        unneighbor(node);
    }
#endif
}

void octcell::coarsen()
{
    /* This function will make the cell a leaf cell */
    if (is_leaf()) {
        throw logic_error("Trying to coarsen a leaf cell");
    }

    for (uint idx = 0; idx < MAX_NUM_CHILDREN; idx++) {
        octcell* c = get_child(idx);
        if (!c) {
            /* This child does not exist */
            continue;
        }
        /* Coarsen the children if it is not already a leaf */
        if (c->has_child_array()) {
            c->coarsen();
        }

        /* Neighbors already connected with */
        bool connected_with_neighbor[NUM_DIRECTIONS];
        for (uint dir = 0; dir < NUM_DIRECTIONS; dir++) {
            connected_with_neighbor[dir] = false;
        }
        /* Loop through all neighbors of the children and see which to keep (steal) and which to throw away */
        nlnode* node = c->get_first_neighbor_list_node();
        nlnode* next_node;
        for (; node; node = next_node) {
            next_node = node->get_next_node();
            if (node->v.low_lvl > lvl) {
                /* The neighbor connection is internal within this cell, remove connection */
                c->unneighbor(node);
                continue; /* Continue with next neighbor connection */
            }
            octcell* neighbor = node->v.n;
            if (neighbor->lvl < lvl) {
                /*
                 * This neighbor is smaller than this cell and therefore only has one
                 * neighbor connection to this cell, keep connection
                 */
                steal_child_neighbor_connection(node);
            }
            else {
                /*
                 * This neighbor is at least as large as this cell and can therefore
                 * have several connections with this cell.
                 */
                uint dir = direction(node->v.dim, node->v.rev_dir);
                if (connected_with_neighbor[dir]) {
                    /* This cell is already connected with the neighbor, remove connection */
                    c->unneighbor(node);
                }
                else {
                    /* This cell has not yet connected to neighbor, so keep connection */
                    steal_child_neighbor_connection(node);
                    connected_with_neighbor[dir] = true;
                }
            }
        } // node

        /* Finished with child cell, delete it */
        remove_child(idx);
    } // idx

    /* All child cells deleted, make leaf cell */
    make_leaf();
}

#if 0
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
#endif

#if 0
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
    for (uint dim = 0; dim < NUM_DIMENSIONS; dim++) {
        r1[dim] = r[dim] + ((idx >> dim) & 1) * s_2;
    }
    set_child(idx, new octcell(s_2, r1, lvl + 1));

    return get_child(idx);
}
#endif

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
            for (uint dim = 0; dim < NUM_DIMENSIONS; dim++) {
                uint i2 = i1 | index_offset(dim);
                if (i2 > i1 && get_child(i2)) {
                    /* Second child also exists, generate neighbors in interface */
                    generate_all_cross_cell_neighbors(get_child(i1), get_child(i2), dim);
                }
            }
        }
    }
}
#endif

////////////////////////////////////////////////////////////////
// PUBLIC STATIC METHODS
////////////////////////////////////////////////////////////////

void octcell::make_neighbors(octcell* c1, octcell* c2, uint dimension, uint lowest_level)
{
    /* Create elements to work with */
    nlnode* node1 = c1->neighborlist.add_new_element();
    nlnode* node2 = c2->neighborlist.add_new_element();
    /* Calculate properties */
    pfvec dist = c2->cell_center() - c1->cell_center();
    pftype dist_abs = dist.length();
    pftype min_s = MIN(c1->s, c2->s);
    pftype area = min_s*min_s;
    /* Set properties */
    node1->v.set(node1, node2, c2,  dist, dist_abs, area, dimension, false, lowest_level);
    node2->v.set(node2, node1, c1, -dist, dist_abs, area, dimension, true , lowest_level);
}

#if  GENERATE_NEIGHBORS_STATICALLY
void octcell::generate_all_cross_cell_neighbors(octcell* c1, octcell* c2, uint normal_dimension)
{
    // The normal will point from c1 to c2 (not the other way around)
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
        make_neighbors(c1, c2, normal_dimension);
    }
    else if (c1->is_leaf() && !(c2->is_leaf())) { /* CASE 2 */
        /* c1 is a leaf cell but c2 has child cells */
        // TODO: Optimize
        for (uint i = 0; i < MAX_NUM_CHILDREN; i++) {
            if ((i >> normal_dimension & 1) == 0) {
                /* Potential child cell with this index in c2 borders to c1 */
                if (c2->get_child(i)) {
                    /* Child exists */
                    if (c2->get_child(i)->has_child_array()) {
                        generate_all_cross_cell_neighbors(c1, c2->get_child(i), normal_dimension);
                    }
                    else {
                        make_neighbors(c1, c2->get_child(i), normal_dimension);
                    }
                }
            }
        }
    }
    else if (!(c1->is_leaf()) && c2->is_leaf()) {  /* CASE 3 */
        /* c2 is a leaf cell but c1 has child cells */
        // TODO: Optimize
        for (uint i = 0; i < MAX_NUM_CHILDREN; i++) {
            if (i >> normal_dimension & 1) {
                /* Potential child cell with this index in c1 borders to c2 */
                if (c1->get_child(i)) {
                    /* Child exists */
                    if (c1->get_child(i)->has_child_array()) {
                        generate_all_cross_cell_neighbors(c1->get_child(i), c2, normal_dimension);
                    }
                    else {
                        make_neighbors(c1->get_child(i), c2, normal_dimension);
                    }
                }
            }
        }
    }
    else {  /* CASE 4 */
        /* Both c1 and c2 have children */
        // TODO: Optimize
        uint dim_offset = child_index_offset(normal_dimension);
        for (uint i = 0; i < MAX_NUM_CHILDREN; i++) {
            if (i >> normal_dimension & 1) {
                /* Potential child cell with this index in c1 borders to c2 */
                if (c1->get_child(i) && c2->get_child(i-dim_offset)) {
                    /* Children exist in both c1 and c2 */
                    generate_all_cross_cell_neighbors(c1->get_child(i), c2->get_child(i-dim_offset), normal_dimension);
                }
            }
        }
    }
}
#endif

////////////////////////////////////////////////////////////////
// PRIVATE NON-STATIC METHODS
////////////////////////////////////////////////////////////////

void octcell::steal_child_neighbor_connection(nlnode* child_node)
{
    /* Create and extract elements to work with */
    octcell* neighbor = child_node->v.n;
    nlnode* new_node = neighborlist.add_new_element();
    nlnode* matching_node = child_node->v.mnle;
    /* Calculate properties */
    pfvec dist = neighbor->cell_center() - cell_center();
    pftype dist_abs = dist.length();
    pftype min_s = MIN(s, neighbor->s);
    pftype area = min_s*min_s;
    uint dim = child_node->v.dim;
    bool rev_dir = child_node->v.rev_dir;
    uint low_lvl = child_node->v.low_lvl;
    /* Set properties */
    new_node     ->v.set(new_node     , matching_node, neighbor,  dist, dist_abs, area, dim,  rev_dir, low_lvl);
    matching_node->v.set(matching_node, new_node     , this    , -dist, dist_abs, area, dim, !rev_dir, low_lvl);
    /* Remove neighbor connection for the child */
    child_node->remove_from_list();
}
