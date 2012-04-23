////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

// Own includes
#include "octcell.h"

////////////////////////////////////////////////////////////////
// CONSTRUCTORS AND DESTRUCTOR
////////////////////////////////////////////////////////////////

octcell::octcell(pftype size, pfvec pos, uint level, uint internal_layer_advancement)
{
    s = size;
    r = pos;
    lvl = level;
    //ila = internal_layer_advancement;
    internal_layer_advancement = internal_layer_advancement;
    _c = 0;
}

octcell::~octcell()
{
#if 0
    nlnode* node;
    nlnode* next_node;
    /* Remove leaf neighbor connections */
    for (node = get_first_leaf_neighbor_list_node(); node; node = next_node) {
#if  DEBUG
        throw logic_error("Cell being deleted has leaf neighbor connections");
#endif
        next_node = node->get_next_node();
        un_leaf_neighbor(node);
    }

    /* Remove coarse neighbor connections */
    for (node = get_first_coarse_neighbor_list_node(); node; node = next_node) {
#if  DEBUG
        throw logic_error("Cell being deleted has coarse neighbor connections");
#endif
        next_node = node->get_next_node();
        un_coarse_neighbor(node);
    }
#endif

    /* Remove all neighbor connections */
    break_all_neighbor_connections();

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

    /* Find neighbors on this and the childrens' level */
    nlset same_level_set;
    same_level_set.add_neighbor_list(&neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_LEAF]);
    same_level_set.add_neighbor_list(&neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_NON_LEAF]);
    for (nlnode* node = same_level_set.get_first_node(); node; node = same_level_set.get_next_node()) {
        octcell* n = node->v.n; // Neighbor
        uint dim = node->v.dim;
        bool pos_dir = node->v.pos_dir;
        for (uint cidx = 0; cidx < MAX_NUM_CHILDREN; cidx++) { // Child index in this cell
            if (positive_direction_of_child(cidx, dim) == pos_dir) {
                /* Child index is in the right half */
                octcell* c = get_child(cidx); // Child
                if (c) {
                    /* Child exists, make neighbor */
                    if (n->has_child_array()) {
                        make_neighbors(c, n, NL_LOWER_LEVEL_OF_DETAIL_NON_LEAF, NL_HIGHER_LEVEL_OF_DETAIL, dim, pos_dir);
                        /* Cell has children, see if there is a neighbor at the child level */
                        uint ncidx = child_index_flip_direction(cidx, dim); // Neighbor child index
                        octcell* nc = n->get_child(ncidx); // Neighbor child
                        if (nc) {
                            /* Neighbor cell on child level exists too, make neighbors */
                            if (nc->has_child_array()) {
                                make_neighbors(c, nc, NL_SAME_LEVEL_OF_DETAIL_NON_LEAF, NL_SAME_LEVEL_OF_DETAIL_LEAF, dim, pos_dir);
                                /* Child cell has children too, see if any of these are neighbors */
                                for (uint nccidx = 0; nccidx < MAX_NUM_CHILDREN; nccidx++) { // Neighbor child child index
                                    if (positive_direction_of_child(nccidx, dim) != pos_dir) {
                                        // Potential neighbor child child is in righ half, make neighbor
                                        octcell* ncc = nc->get_child(nccidx);
                                        if (ncc) {
                                            // Cell exists, make neighbor
                                            make_neighbors(c, ncc, NL_HIGHER_LEVEL_OF_DETAIL, NL_LOWER_LEVEL_OF_DETAIL_LEAF, dim, pos_dir);
                                        }
                                    }
                                } // for nccidx
                            } // if nc->has_child_array()
                            else {
                                make_neighbors(c, nc, NL_SAME_LEVEL_OF_DETAIL_LEAF, NL_SAME_LEVEL_OF_DETAIL_LEAF, dim, pos_dir);
                            }
                        }

                    } // if n->has_child_array()
                    else {
                        make_neighbors(c, n, NL_LOWER_LEVEL_OF_DETAIL_LEAF, NL_HIGHER_LEVEL_OF_DETAIL, dim, pos_dir);
                    }
                } // if c
            } // if child index is in right half
        } // i
        /* This cell is no longer a leaf cell, update other end of neighbor connection */
        n->move_neighbor_connection_to_other_list(node->v.cnle, NL_SAME_LEVEL_OF_DETAIL_NON_LEAF);
    } // node

    /* This cell is no longer a leaf cell, update other end of the connections to the higher level */
    nlnode* node = neighbor_lists[NL_HIGHER_LEVEL_OF_DETAIL].get_first_node();
    nlnode* next_node;
    for (; node; node = next_node) {
        next_node = node->get_next_node();
        node->v.n->move_neighbor_connection_to_other_list(node->v.cnle, NL_LOWER_LEVEL_OF_DETAIL_NON_LEAF);
    }

    // Create all neighbor connections internally between the child cells
    // TODO: Optimize
    for (uint idx1 = 0; idx1 < MAX_NUM_CHILDREN; idx1++) {
        for (uint dim = 0; dim < NUM_DIMENSIONS; dim++) {
            uint idx2 = idx1 | child_index_offset(dim);
#if  DEBUG
            if (idx2 >= MAX_NUM_CHILDREN || idx2 < 0) {
                throw logic_error("Should end up here, overflow");
            }
#endif
            if (idx2 > idx1) {
                make_neighbors(get_child(idx1), get_child(idx2), NL_SAME_LEVEL_OF_DETAIL_LEAF, NL_SAME_LEVEL_OF_DETAIL_LEAF, dim, true);
            }
        }
    }

}

#if 0
void octcell::refine2()
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

    /* Create new coarse neighbors for the child cells */
    nlnode* cn = get_first_coarse_neighbor_list_node(); // Coarse neighbor
    nlnode* next_cn;
    uint child_level = lvl + 1;
    for (; cn; cn = next_cn) {
        next_cn = cn->get_next_node();
        octcell* neighbor = cn->v.n;
#if DEBUG
        if (neighbor->is_leaf()) {
            throw logic_error("Coarse neighbor connection exists between two leaf cells");
        }
#endif
        uint dim = cn->v.dim;
        bool pos_dir = cn->v.pos_dir;
        for (uint nci = 0; nci < MAX_NUM_CHILDREN; nci++) { // Neighbor child index
            if (positive_direction_of_child(nci, dim) == pos_dir) {
                /* Neighbor child cell lies in wrong half of the neighbor cell */
                continue;
            }
            octcell* nc = neighbor->get_child(nci); // Neighbor child
            if (nc && nc->has_child_array()) {
                /* Child cell is a non-leaf cell at the same level at this cells children, and therefor a coarse neighbor */
                octcell* tc = get_child(child_index_flip_direction(nci, dim));
                if (pos_dir) {
                    make_coarse_neighbors(tc, nc, dim, child_level);
                }
                else {
                    make_coarse_neighbors(nc, tc, dim, child_level);
                }
            }
        } // nci
    } // cn

    /* Create new leaf neighbors between child cells */
    for (uint i1 = 0; i1 < MAX_NUM_CHILDREN; i1++) {
        for (uint dim = 0; dim < NUM_DIMENSIONS; dim++) {
            uint i2 = i1 | child_index_offset(dim);
            if (i2 >= MAX_NUM_CHILDREN || i2 < 0) {
                i2 = i2;
            }
            if (i2 > i1) {
                make_leaf_neighbors(get_child(i1), get_child(i2), dim, new_level);
            }
        }
    }

    /* Refine existing leaf neighbors */
    nlnode* ln = get_first_leaf_neighbor_list_node();
    nlnode* next_ln;
    /* Loop through all neighbors on the parent (this) level */
    for (; ln; ln = next_ln) {
        next_ln = ln->get_next_node();
        uint lowest_connection_level = ln->v.low_lvl;
        uint dim = ln->v.dim;
        bool pos_dir = ln->v.pos_dir;
        octcell* neighbor = ln->v.n;
        if (neighbor->lvl <= lvl) {
            /* Neighbor cell is at least as large as this cell */
            /* Make half on the child cells (on the correct side of the cell) neighbors with this neighbor */
            // TODO: Optimize loop
            for (uint i = 0; i < MAX_NUM_CHILDREN; i++) {
                /* The direction between the neighbors is reversed (??? wut? TODO) */
                if (positive_direction_of_child(i, dim) == pos_dir) {
                    /* Child cell is adjactent */
                    if (pos_dir) {
                        make_leaf_neighbors(get_child(i), neighbor, dim, lowest_connection_level);
                    }
                    else {
                        make_leaf_neighbors(neighbor, get_child(i), dim, lowest_connection_level);
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
                    idx |= (pos_dir << dim2);
                }
                else {
                    /* dim2 is orthogonal to dim */
                    idx |= ((cell_center()[dim2] < neighbor->cell_center()[dim2]) << dim2);
                }
            }
            /* Index calculated, make neighbor */
            if (pos_dir) {
                make_leaf_neighbors(get_child(idx), neighbor, dim, lowest_connection_level);
            }
            else {
                make_leaf_neighbors(neighbor, get_child(idx), dim, lowest_connection_level);
            }
        }
        /* Finished with leaf node, decide what to do with it */
        make_leaf_neighbor_coarse_neighbor_or_throw_away(ln);
    }
#endif // GENERATE_NEIGHBORS_DYNAMICALLY
}
#endif

void octcell::coarsen()
{
#if  DEBUG
    /* This function will make the cell a leaf cell */
    if (is_leaf()) {
        throw logic_error("Trying to coarsen a leaf cell");
    }
#endif

    for (uint idx = 0; idx < MAX_NUM_CHILDREN; idx++) {
        octcell* c = get_child(idx);
        if (c) {
            /* Child exists, remove it */
            remove_child(idx);
        }
    }
    make_leaf();

    /*
     * This cell is no longer a leaf cell, update other end of the connections to
     * the neighbor cells at the same level and at the higher level
     */

    nlset same_level_set;
    same_level_set.add_neighbor_list(&neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_LEAF]);
    same_level_set.add_neighbor_list(&neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_NON_LEAF]);
    for (nlnode* node = same_level_set.get_first_node(); node; node = same_level_set.get_next_node()) {
        node->v.n->move_neighbor_connection_to_other_list(node->v.cnle, NL_SAME_LEVEL_OF_DETAIL_LEAF);
    }

    nlnode* node = neighbor_lists[NL_HIGHER_LEVEL_OF_DETAIL].get_first_node();
    nlnode* next_node;
    for (; node; node = next_node) {
        next_node = node->get_next_node();
        node->v.n->move_neighbor_connection_to_other_list(node->v.cnle, NL_LOWER_LEVEL_OF_DETAIL_LEAF);
    }
}

#if 0
void octcell::coarsen2()
{
#if  DEBUG
    static int count = 0;
    /* This function will make the cell a leaf cell */
    if (is_leaf()) {
        throw logic_error("Trying to coarsen a leaf cell");
    }
#endif


    nlnode* cn = get_first_coarse_neighbor_list_node();
    nlnode* next_cn;
    for (; cn; cn = next_cn) {
        next_cn = cn->get_next_node();
        octcell* neighbor = cn->v.n;
        if (neighbor->is_leaf()) {
            /* This coarse neighbor connection will become a leaf neighbor connection */
            make_coarse_neighbor_leaf_neighbor(cn);
        }
        else {
            uint dim = cn->v.dim;
            bool pos_dir = cn->v.pos_dir;
            find_and_connect_to_all_leaf_neighbors(neighbor, dim, pos_dir);
        }
    } // cn

    /* All nieghbor connections are now generated. Just remove the children and any neighbor connections they have */

    for (uint idx = 0; idx < MAX_NUM_CHILDREN; idx++) {
        octcell* c = get_child(idx);
        if (c) {
            /* Child exists, remove it */
            if (c->has_child_array()) {
                /* Child has a child array, coarsen it first */
                c->coarsen();
            }
            c->remove_all_leaf_neighbor_connections();
            c->remove_all_coarse_neighbor_connections();
            remove_child(idx);
        }
    }
    make_leaf();

    return;

    for (uint idx = 0; idx < MAX_NUM_CHILDREN; idx++) {
        //TODO: Remove these test variables
#if  DEBUG
        count++;
        uint this_count = count;
        this_count = this_count;
        if (count == 3) {
            count = count;
        }
#endif
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
        nlnode* node = c->get_first_leaf_neighbor_list_node();
        nlnode* next_node;
        for (; node; node = next_node) {
            next_node = node->get_next_node();
            if (node->v.low_lvl > lvl) {
                /* The neighbor connection is internal within this cell, remove connection */
                c->un_leaf_neighbor(node);
                continue; /* Continue with next neighbor connection */
            }
            octcell* neighbor = node->v.n;
            if (neighbor->lvl > lvl) {
                /*
                 * This neighbor is smaller than this cell and therefore only has one
                 * neighbor connection to this cell, keep connection
                 */
                steal_child_leaf_neighbor_connection(node);
            }
            else {
                /*
                 * This neighbor is at least as large as this cell and can therefore
                 * have several connections with this cell.
                 */
                uint dir = direction(node->v.dim, node->v.pos_dir, true);
                if (connected_with_neighbor[dir]) {
                    /* This cell is already connected with the neighbor, remove connection */
                    c->un_leaf_neighbor(node);
                }
                else {
                    /* This cell has not yet connected to neighbor, so keep connection */
                    steal_child_leaf_neighbor_connection(node);
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
#endif

void octcell::move_neighbor_connection_to_other_list(nlnode* node, uint new_list_index)
{
#if  DEBUG
    if (new_list_index >= NUM_NEIGHBOR_LISTS) {
        throw out_of_range("Trying to move node to list with invalid index");
    }
#endif
    node->remove_from_list_and_keep();
    neighbor_lists[new_list_index].add_existing_node(node);
}

void octcell::break_all_neighbor_connections()
{
    for (uint i = 0; i < NUM_NEIGHBOR_LISTS; i++) {
        nlnode* current_node;
        nlnode* next_node;
        for (current_node = neighbor_lists[i].get_first_node(); current_node; current_node = next_node) {
            next_node = current_node->get_next_node();
            _un_neighbor(current_node);
        }
    }
}

#if 0
void octcell::find_and_connect_to_all_leaf_neighbors(octcell* neighbor, uint dim, bool pos_dir)
{
#if DEBUG
    if (is_leaf()) {
        throw logic_error("Trying to find neighbor sub cells in a leaf");
    }
#endif
    for (uint idx = 0; idx < MAX_NUM_CHILDREN; idx++) {
        if (positive_direction_of_child(idx, dim) == pos_dir) {
            /* Child lies in wrong half of cell, continue with next child */
            continue;
        }
        octcell* c = neighbor->get_child(idx);
        if (c) {
            if (c->has_child_array()) {
                find_and_connect_to_all_leaf_neighbors(c, dim, pos_dir);
            }
            else {
                /* Child is a leaf cell, make leaf neighbors */
                if (pos_dir) {
                    make_leaf_neighbors(this, c, dim, c->lvl);
                }
                else {
                    make_leaf_neighbors(c, this, dim, c->lvl);
                }
            }
        }
    } // idx
}
#endif

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
void octcell::generate_all_internal_leaf_neighbors()
{
    if (is_leaf()) {
        return;
    }
    uint child_level = lvl + 1;
    //TODO: Optimize:
    for (uint i1 = 0; i1 < MAX_NUM_CHILDREN; i1++) {
        if (get_child(i1)) {
            /* Child exists */
            /* Generate all internal neighbors in the child cell */
            get_child(i1)->generate_all_internal_leaf_neighbors();
            /* Generate all cross-child cell neighbors from this child to children with larger indexes */
            for (uint dim = 0; dim < NUM_DIMENSIONS; dim++) {
                uint i2 = i1 | child_index_offset(dim);
                if (i2 > i1 && get_child(i2)) {
                    /* Second child also exists, generate neighbors in interface */
                    generate_all_cross_cell_leaf_neighbors(get_child(i1), get_child(i2), dim, child_level);
                }
            }
        }
    }
}
#endif

////////////////////////////////////////////////////////////////
// PUBLIC STATIC METHODS
////////////////////////////////////////////////////////////////

void octcell::make_neighbors(octcell* cell1, octcell* cell2, uint cell1_neighbor_list_idx, uint cell2_neighbor_list_idx, uint dimension, bool pos_dir)
{
#if  DEBUG
    if (!(cell1 && cell2)) {
        throw logic_error("Trying to make a cell neighbor using a null pointer");
    }
    if (cell1 == cell2) {
        throw logic_error("Trying to make a cell neighbor with itself");
    }
    if (cell1_neighbor_list_idx >= NUM_NEIGHBOR_LISTS || cell2_neighbor_list_idx >= NUM_NEIGHBOR_LISTS) {
        throw out_of_range("Neighbor list index doesn't exist");
    }
    if ((cell1_neighbor_list_idx == NL_HIGHER_LEVEL_OF_DETAIL) && (cell2_neighbor_list_idx == NL_HIGHER_LEVEL_OF_DETAIL)) {
        throw logic_error("Error when making neighbors: Both cells are claimed to be on the higher level");
    }
    if (((cell1_neighbor_list_idx == NL_SAME_LEVEL_OF_DETAIL_LEAF) ||
         (cell1_neighbor_list_idx == NL_SAME_LEVEL_OF_DETAIL_NON_LEAF)) !=
        ((cell2_neighbor_list_idx == NL_SAME_LEVEL_OF_DETAIL_LEAF) ||
         (cell2_neighbor_list_idx == NL_SAME_LEVEL_OF_DETAIL_NON_LEAF)) ) {
        throw logic_error("Error when making neighbors: One cell is at the same level as the other cell but the other cell is not at the same level as the first cell");
    }
    if (((cell1_neighbor_list_idx == NL_LOWER_LEVEL_OF_DETAIL_LEAF) ||
         (cell1_neighbor_list_idx == NL_LOWER_LEVEL_OF_DETAIL_NON_LEAF)) &&
        ((cell2_neighbor_list_idx == NL_LOWER_LEVEL_OF_DETAIL_LEAF) ||
         (cell2_neighbor_list_idx == NL_LOWER_LEVEL_OF_DETAIL_NON_LEAF)) ) {
         throw logic_error("Error when making neighbors: Both cells are claimed to be on the lower level");
    }
    cell1_neighbor_list_idx = cell1_neighbor_list_idx;
#endif
    /* Create elements to work with */
    nlnode* node1 = cell1->neighbor_lists[cell1_neighbor_list_idx].add_new_element();
    nlnode* node2 = cell2->neighbor_lists[cell2_neighbor_list_idx].add_new_element();
    /* Calculate properties */
    pfvec dist = cell2->cell_center() - cell1->cell_center();
    pftype dist_abs = dist.length();
    pftype min_s = MIN(cell1->s, cell2->s);
    pftype area = min_s*min_s;
    /* Set properties */
    node1->v.set(cell2, node2,  dist, dist_abs, area, dimension,  pos_dir);
    node2->v.set(cell1, node1, -dist, dist_abs, area, dimension, !pos_dir);
}

#if 0
void octcell::make_leaf_neighbors(octcell* c1, octcell* c2, uint dimension, uint lowest_level)
{
#if  DEBUG
    if (!(c1 && c2)) {
        throw logic_error("Trying to make a cell neighbor using a null pointer");
    }
    if (c1 == c2) {
        throw logic_error("Trying to make a cell neighbor with itself");
    }
#endif
    /* Create elements to work with */
    nlnode* node1 = c1->leaf_neighbor_list.add_new_element();
    nlnode* node2 = c2->leaf_neighbor_list.add_new_element();
    /* Calculate properties */
    pfvec dist = c2->cell_center() - c1->cell_center();
    pftype dist_abs = dist.length();
    pftype min_s = MIN(c1->s, c2->s);
    pftype area = min_s*min_s;
    /* Set properties */
    //node1->v.set(c2, node2,  dist, dist_abs, area, dimension, true , lowest_level);
    //node2->v.set(c1, node1, -dist, dist_abs, area, dimension, false, lowest_level);
    node1->v.set(c2, node2,  dist, dist_abs, area, dimension, true );
    node2->v.set(c1, node1, -dist, dist_abs, area, dimension, false);
}

void octcell::make_coarse_neighbors(octcell* c1, octcell* c2, uint dimension, uint lowest_level)
{
#if  DEBUG
    if (!(c1 && c2)) {
        throw logic_error("Trying to make a cell neighbor using a null pointer");
    }
    if (c1 == c2) {
        throw logic_error("Trying to make a cell neighbor with itself");
    }
    if (c1->lvl != c2->lvl) {
        throw logic_error("Trying to make two cells on different levels coarse neighbors");
    }
    if (c1->is_leaf() && c2->is_leaf()) {
        throw logic_error("Trying to make two leaf cells coarse neighbors");
    }
#endif
    /* Create elements to work with */
    nlnode* node1 = c1->coarse_neighbor_list.add_new_element();
    nlnode* node2 = c2->coarse_neighbor_list.add_new_element();
    /* Calculate properties */
    pfvec dist = c2->cell_center() - c1->cell_center();
    pftype dist_abs = dist.length();
    pftype min_s = MIN(c1->s, c2->s);
    pftype area = min_s*min_s;
    /* Set properties */
    //node1->v.set(c2, node2,  dist, dist_abs, area, dimension, true , lowest_level);
    //node2->v.set(c1, node1, -dist, dist_abs, area, dimension, false, lowest_level);
    node1->v.set(c2, node2,  dist, dist_abs, area, dimension, true );
    node2->v.set(c1, node1, -dist, dist_abs, area, dimension, false);
}
#endif

#if  GENERATE_NEIGHBORS_STATICALLY
void octcell::generate_all_cross_cell_leaf_neighbors(octcell* c1, octcell* c2, uint normal_dimension, uint lowest_level)
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
#endif // DEBUG

    if (c1->is_leaf() && c2->is_leaf()) { /* CASE 1 */
        // Both nodes are leaf nodes, add them to each others neighbor lists
        make_leaf_neighbors(c1, c2, normal_dimension, lowest_level);
    }
    else if (c1->is_leaf() && !(c2->is_leaf())) { /* CASE 2 */
        /* c1 is a leaf cell but c2 has child cells */
        // TODO: Optimize
        for (uint i = 0; i < MAX_NUM_CHILDREN; i++) {
            if (!positive_direction_of_child(i, normal_dimension)) {
                /* Potential child cell with this index in c2 borders to c1 */
                if (c2->get_child(i)) {
                    /* Child exists */
                    if (c2->get_child(i)->has_child_array()) {
                        generate_all_cross_cell_leaf_neighbors(c1, c2->get_child(i), normal_dimension, lowest_level);
                    }
                    else {
                        make_leaf_neighbors(c1, c2->get_child(i), normal_dimension, lowest_level);
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
                        generate_all_cross_cell_leaf_neighbors(c1->get_child(i), c2, normal_dimension, lowest_level);
                    }
                    else {
                        make_leaf_neighbors(c1->get_child(i), c2, normal_dimension, lowest_level);
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
                    generate_all_cross_cell_leaf_neighbors(c1->get_child(i), c2->get_child(i-dim_offset), normal_dimension, lowest_level);
                }
            }
        }
    }
}
#endif // GENERATE_NEIGHBORS_STATICALLY

////////////////////////////////////////////////////////////////
// PRIVATE NON-STATIC METHODS
////////////////////////////////////////////////////////////////

#if 0
void octcell::steal_child_leaf_neighbor_connection(nlnode* child_node)
{
#if RUN_SAFE
    /* Make this sell a neighbor */
    uint dim = child_node->v.dim;
    bool pos_dir = child_node->v.pos_dir;
    uint low_lvl = child_node->v.low_lvl;
    if (pos_dir) {
        make_leaf_neighbors(this, child_node->v.n, dim, low_lvl);
    }
    else {
        make_leaf_neighbors(child_node->v.n, this, dim, low_lvl);
    }
    /* And remove the child cell as a neighbor */
    child_node->v.cnle->v.n->un_leaf_neighbor(child_node);
#elif 1
    /* Create and extract elements to work with */
    octcell* neighbor = child_node->v.n;
    nlnode* new_node = leaf_neighbor_list.add_new_element();
    nlnode* corresponding_node = child_node->v.cnle;
    /* Calculate properties */
    pfvec dist = neighbor->cell_center() - cell_center();
    pftype dist_abs = dist.length();
    pftype min_s = MIN(s, neighbor->s);
    pftype area = min_s*min_s;
    uint dim = child_node->v.dim;
    bool pos_dir = child_node->v.pos_dir;
    uint low_lvl = child_node->v.low_lvl;
    /* Set properties */
    new_node          ->v.set(neighbor, corresponding_node,  dist, dist_abs, area, dim,  pos_dir, low_lvl);
    corresponding_node->v.set(this    , new_node          , -dist, dist_abs, area, dim, !pos_dir, low_lvl);
    /* Remove neighbor connection for the child */
    child_node->remove_from_list_and_delete();
#else
    //TODO: Write code (use remove_from_list_and_keep() instead and make something of it)
#endif
}
#endif

#if 0
void octcell::make_leaf_neighbor_coarse_neighbor_or_throw_away(nlnode* list_entry)
{
    //TODO: Optimize:
    octcell* neighbor = list_entry->v.n;
    if (lvl == neighbor->lvl) {
    //if (false) {
        make_leaf_neighbor_coarse_neighbor(list_entry);
    }
    else {
        un_leaf_neighbor(list_entry);
    }
}

void octcell::make_coarse_neighbor_leaf_neighbor(nlnode* list_entry)
{
    /* Move to leaf neighbor list in this cell */
    list_entry->remove_from_list_and_keep();
    leaf_neighbor_list.add_existing_node(list_entry);

    /* Move to leaf neighbor list in neighbor cell */
    octcell* neighbor = list_entry->v.n;
    nlnode* matching_list_entry = list_entry->v.cnle;
    matching_list_entry->remove_from_list_and_keep();
    neighbor->leaf_neighbor_list.add_existing_node(matching_list_entry);
}

void octcell::make_leaf_neighbor_coarse_neighbor(nlnode* list_entry)
{
    /* Move to coarse neighbor list in this cell */
    list_entry->remove_from_list_and_keep();
    coarse_neighbor_list.add_existing_node(list_entry);

    /* Move to coarse neighbor list in neighbor cell */
    octcell* neighbor = list_entry->v.n;
    nlnode* matching_list_entry = list_entry->v.cnle;
    matching_list_entry->remove_from_list_and_keep();
    neighbor->coarse_neighbor_list.add_existing_node(matching_list_entry);
}
#endif
