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
#if  RUN_SAFE
    rp = 0;
#endif
    _c = 0;
}

octcell::~octcell()
{
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
#if  DEBUG
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

    // Create children with new values
    pfvec new_r;
    for (uint i = 0; i < MAX_NUM_CHILDREN; i++) {
        for (uint dim = 0; dim < NUM_DIMENSIONS; dim++) {
            new_r[dim] = corners[(i >> dim) & 1][dim];
        }
        set_child(i, new octcell(s_2, new_r, new_level));
    }

    /***************************
     * +---------------------+ *
     * |                     | *
     * |  AHEAD: MOUNT CODE  | *
     * |                     | *
     * +---------------------+ *
     ***************************/

    /* Find neighbors on this and the childrens' and the childrens' childrens' level */
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

    /*
     * This cell is no longer a leaf cell, update other end of the connections to
     * the higher level
     */
    nlnode* node = neighbor_lists[NL_HIGHER_LEVEL_OF_DETAIL].get_first_node();
    nlnode* next_node;
    for (; node; node = next_node) {
        next_node = node->get_next_node();
        node->v.n->move_neighbor_connection_to_other_list(node->v.cnle, NL_LOWER_LEVEL_OF_DETAIL_NON_LEAF);
    }

    /*
     * The lower level neighbors do not need to be updated, because they have only
     * one list for cells at this level anyway
     */

    // Create all neighbor connections internally between the child cells
    // TODO: Optimize
    for (uint idx1 = 0; idx1 < MAX_NUM_CHILDREN; idx1++) {
        for (uint dim = 0; dim < NUM_DIMENSIONS; dim++) {
            uint idx2 = idx1 | child_index_offset(dim);
            if (idx2 > idx1) {
                make_neighbors(get_child(idx1), get_child(idx2), NL_SAME_LEVEL_OF_DETAIL_LEAF, NL_SAME_LEVEL_OF_DETAIL_LEAF, dim, true);
            }
        }
    }

}

void octcell::coarsen()
{
#if  DEBUG
    /* This function will make the cell a leaf cell */
    if (is_leaf()) {
        throw logic_error("Trying to coarsen a leaf cell");
    }
#endif

    surface_cell = false;
    vof = 0;
    rp = 0;
    for (uint idx = 0; idx < MAX_NUM_CHILDREN; idx++) {
        octcell* c = get_child(idx);
        if (c) {
            /* Child exists, remove it */
            if (c->has_child_array()) {
                /* Coarsen it to get updated properties */
                c->coarsen();
            }
            if (c->surface_cell) {
                surface_cell = true;
            }
            pftype child_water_volume = c->get_volume_of_fluid();
            vof += child_water_volume;
            rp += child_water_volume * c->rp;
            remove_child(idx);
        }
    }
    rp /= vof;
    make_leaf();

    /*
     * This cell is no longer a leaf cell, update other end of the connections to
     * the neighbor cells at the same level and at the higher level
     */

    /* Update the neighbor cells' connections on the same level */
    nlset same_level_set;
    same_level_set.add_neighbor_list(&neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_LEAF]);
    same_level_set.add_neighbor_list(&neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_NON_LEAF]);
    for (nlnode* node = same_level_set.get_first_node(); node; node = same_level_set.get_next_node()) {
        node->v.n->move_neighbor_connection_to_other_list(node->v.cnle, NL_SAME_LEVEL_OF_DETAIL_LEAF);
    }

    /* Update the neighbor cells' connections on the higher level */
    nlnode* node = neighbor_lists[NL_HIGHER_LEVEL_OF_DETAIL].get_first_node();
    nlnode* next_node;
    for (; node; node = next_node) {
        next_node = node->get_next_node();
        node->v.n->move_neighbor_connection_to_other_list(node->v.cnle, NL_LOWER_LEVEL_OF_DETAIL_LEAF);
    }

    /*
     * The lower level neighbors do not need to be updated, because they have only
     * one list for cells at this level anyway
     */
}

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
            un_neighbor(current_node);
        }
    }
}

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
#endif
    /* Create elements to work with */
    nlnode* node1 = cell1->neighbor_lists[cell1_neighbor_list_idx].add_new_element();
    nlnode* node2 = cell2->neighbor_lists[cell2_neighbor_list_idx].add_new_element();
    /* Calculate properties */
    pfvec dist = cell2->cell_center() - cell1->cell_center();
    pftype dist_abs = dist.length();
    pftype min_s = MIN(cell1->s, cell2->s);
    pftype area = cube_side_area(min_s);
    /* Set properties */
    node1->v.set(cell2, node2, dimension,  pos_dir, 0,  dist, dist_abs, area);
    node2->v.set(cell1, node1, dimension, !pos_dir, 0, -dist, dist_abs, area);
}
