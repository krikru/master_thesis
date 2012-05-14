////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

// Own includes
#include "octcell.h"

////////////////////////////////////////////////////////////////
// CONSTRUCTORS AND DESTRUCTOR
////////////////////////////////////////////////////////////////

octcell::octcell(octcell *parent, pftype size, pfvec pos, uint level, uint internal_layer_advancement)
{
    _par = parent;
    s = size;
    r = pos;
    lvl = level;
    //ila = internal_layer_advancement;
    internal_layer_advancement = internal_layer_advancement;
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

/**************
 * Simulation *
 **************/

void octcell::set_volume_coefficients(pftype water_volume_coefficient, pftype total_volume_coefficient)
{
#if  DEBUG
#if  0
    cout << endl;
    cout << "Cell: Old water volume coefficient: " << water_vol_coeff << endl;
    cout << "Cell: Old total volume coefficient: " << total_vol_coeff << endl;
    cout << "Cell: Additional water volume coefficient: " << water_volume_coefficient-water_vol_coeff << endl;
    cout << "Cell: Additional total volume coefficient: " << total_volume_coefficient-total_vol_coeff << endl;
    cout << "Cell: New water volume coefficient: " << water_volume_coefficient << endl;
    cout << "Cell: New total volume coefficient: " << total_volume_coefficient << endl;
#endif
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

/*******************
 * Level of detail *
 *******************/

/**********
 * Family *
 **********/

void octcell::make_parent()
{
    create_new_random_child_array();
    for (uint idx = 0; idx < MAX_NUM_CHILDREN; idx++) {
        set_child(idx, 0);
    }

    /*
     * This cell is no longer a parent cell, update other end of the connections to
     * the neighbor cells at the same level and at the higher level
     */

    /* Update the neighbor cells' connections on the same level */
    nlset same_level_set;
    same_level_set.add_neighbor_list(&neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_LEAF]);
    same_level_set.add_neighbor_list(&neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_NON_LEAF]);
    for (nlnode* node = same_level_set.get_first_node(); node; node = same_level_set.get_next_node()) {
        node->v.n->move_neighbor_connection_to_other_list(node->v.cnle, NL_SAME_LEVEL_OF_DETAIL_NON_LEAF);
    }

    /* Update the neighbor cells' connections on the higher level */
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
}

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

void octcell::refine()
{
    create_new_random_child_array();

    // Create new values for children
    pftype s_2 = 0.5*s;
    pfvec corners[2];
    corners[0] = r;
    corners[1] = get_cell_center();
    uint   new_level = lvl + 1;

    // Create children with new values
    pfvec new_r;
    for (uint i = 0; i < MAX_NUM_CHILDREN; i++) {
        for (uint dim = 0; dim < NUM_DIMENSIONS; dim++) {
            new_r[dim] = corners[(i >> dim) & 1][dim];
        }
        set_child(i, new octcell(this, s_2, new_r, new_level));
    }

    /*****************************
     * +-----------------------+ *
     * |                       | *
     * |  AHEAD: MOUNT CODE 1  | *
     * |                       | *
     * +-----------------------+ *
     *****************************/

    /* Find neighbors on this and the children's and the children's children's level */
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
        } // cidx
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
     * The higher level neighbors have already been updated in the code mountain
     */

    /*
     * The lower level neighbors do not need to be updated, because they have only
     * one list for cells at this level anyway
     */

    // Create all neighbor connections internally between the child cells
    // Optimize
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

    water_vol_coeff = 0;
    total_vol_coeff = 0;
    for (uint idx = 0; idx < MAX_NUM_CHILDREN; idx++) {
        octcell* c = get_child(idx);
        if (c) {
            /* Child exists, remove it */
            if (c->has_child_array()) {
                /* Coarsen it to get updated properties */
                c->coarsen();
            }
            water_vol_coeff += c->water_vol_coeff;
            total_vol_coeff += c->total_vol_coeff;
            remove_child(idx);
        }
    }
    water_vol_coeff *= (pftype(1)/MAX_NUM_CHILDREN);
    total_vol_coeff *= (pftype(1)/MAX_NUM_CHILDREN);
    make_leaf();
}

octcell* octcell::create_new_air_child(uint child_idx)
{
#if  DEBUG
    if (!has_child_array()) {
        throw logic_error("Trying to create a new air child in a cell without a child array");
    }
    if (child_idx < 0 || child_idx >= MAX_NUM_CHILDREN) {
        throw out_of_range("Trying to create an air child with index out of bound");
    }
    if (get_child(child_idx)) {
        throw logic_error("Trying to create a new air child that already exists");
    }
#endif
    // Create new values for child
    pftype s_2 = 0.5*s;
    pfvec new_r;
    for (uint dim = 0; dim < NUM_DIMENSIONS; dim++) {
        new_r[dim] = r[dim] + ((child_idx >> dim) & 1) * s_2;
    }
    // Create child with new values
    octcell *child = new octcell(this, s_2, new_r, lvl + 1);
    child->water_vol_coeff = 0;
    child->total_vol_coeff = 1;
    set_child(child_idx, child);

    /* Create neighbor connections for new child */

    /*****************************
     * +-----------------------+ *
     * |                       | *
     * |  AHEAD: MOUNT CODE 2  | *
     * |                       | *
     * +-----------------------+ *
     *****************************/

    /* Find neighbors on this and the child's and the child's children's level */
    nlset same_level_set;
    same_level_set.add_neighbor_list(&neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_LEAF]);
    same_level_set.add_neighbor_list(&neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_NON_LEAF]);
    for (nlnode* node = same_level_set.get_first_node(); node; node = same_level_set.get_next_node()) {
        octcell* n = node->v.n; // Neighbor
        uint dim = node->v.dim;
        bool pos_dir = node->v.pos_dir;
        if (positive_direction_of_child(child_idx, dim) == pos_dir) {
            /* Child index is in the right half */
            if (n->has_child_array()) {
                make_neighbors(child, n, NL_LOWER_LEVEL_OF_DETAIL_NON_LEAF, NL_HIGHER_LEVEL_OF_DETAIL, dim, pos_dir);
                /* Cell has children, see if there is a neighbor at the child level */
                uint ncidx = child_index_flip_direction(child_idx, dim); // Neighbor child index
                octcell* nc = n->get_child(ncidx); // Neighbor child
                if (nc) {
                    /* Neighbor cell on child level exists too, make neighbors */
                    if (nc->has_child_array()) {
                        make_neighbors(child, nc, NL_SAME_LEVEL_OF_DETAIL_NON_LEAF, NL_SAME_LEVEL_OF_DETAIL_LEAF, dim, pos_dir);
                        /* Child cell has children too, see if any of these are neighbors */
                        for (uint nccidx = 0; nccidx < MAX_NUM_CHILDREN; nccidx++) { // Neighbor child child index
                            if (positive_direction_of_child(nccidx, dim) != pos_dir) {
                                // Potential neighbor child child is in righ half, make neighbor
                                octcell* ncc = nc->get_child(nccidx);
                                if (ncc) {
                                    // Cell exists, make neighbor
                                    make_neighbors(child, ncc, NL_HIGHER_LEVEL_OF_DETAIL, NL_LOWER_LEVEL_OF_DETAIL_LEAF, dim, pos_dir);
                                }
                            }
                        } // for nccidx
                    } // if nc->has_child_array()
                    else {
                        make_neighbors(child, nc, NL_SAME_LEVEL_OF_DETAIL_LEAF, NL_SAME_LEVEL_OF_DETAIL_LEAF, dim, pos_dir);
                    }
                }

            } // if n->has_child_array()
            else {
                make_neighbors(child, n, NL_LOWER_LEVEL_OF_DETAIL_LEAF, NL_HIGHER_LEVEL_OF_DETAIL, dim, pos_dir);
            }
        } // if child index is in right half
        /* This cell is no longer a leaf cell, update other end of neighbor connection */
        n->move_neighbor_connection_to_other_list(node->v.cnle, NL_SAME_LEVEL_OF_DETAIL_NON_LEAF);
    } // node

    // Create all neighbor connections internally between this child cell and other child cells that may exist
    for (uint dim = 0; dim < NUM_DIMENSIONS; dim++) {
        /* Get other child in this dimension */
        uint other_child_idx = child_idx ^ child_index_offset(dim);
        octcell *other_child = get_child(other_child_idx);
        if (other_child) {
            /* Child exists, make neighbors */
            make_neighbors(child, other_child,
                           other_child->has_child_array() ? NL_SAME_LEVEL_OF_DETAIL_NON_LEAF : NL_SAME_LEVEL_OF_DETAIL_LEAF,
                           NL_SAME_LEVEL_OF_DETAIL_LEAF,
                           dim, other_child_idx > child_idx);
        }
    }

    return child;
}

void octcell::move_neighbor_connection_to_other_list(nlnode *node, uint new_list_index)
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

/* Flow */

pftype octcell::get_velocity_divergence() const
{
    pftype div = 0;
    /* Loop through neighbors */
    nlset lists;
    lists.add_neighbor_list(&neighbor_lists[NL_HIGHER_LEVEL_OF_DETAIL]);
    lists.add_neighbor_list(&neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_LEAF]);
    lists.add_neighbor_list(&neighbor_lists[NL_LOWER_LEVEL_OF_DETAIL_LEAF]);
    for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
        div += node->v.vel_out * node->v.cf_area;
    }
    div /= get_cube_volume();
    return div;
}

pftype octcell::get_water_flow_divergence() const
{
    pftype div = 0;
    /* Loop through neighbors */
    nlset lists;
    lists.add_neighbor_list(&neighbor_lists[NL_HIGHER_LEVEL_OF_DETAIL]);
    lists.add_neighbor_list(&neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_LEAF]);
    lists.add_neighbor_list(&neighbor_lists[NL_LOWER_LEVEL_OF_DETAIL_LEAF]);
    for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
        div += node->v.vel_out * node->v.water_vol_coeff * node->v.cf_area;
    }
    div /= get_cube_volume();
    return div;
}

void octcell::prepare_for_water()
{
    /*
     * This function takes care of:
     *   1. Walls to cells with no water in them
     *   2. Missing neighbors (create new ones)
     */

    /* Calculate average velocity vector */
    pfvec mean_vel;
    pfvec area[2];
    /* Loop though neighbors */
    nlset lists;
    lists.add_neighbor_list(&neighbor_lists[NL_HIGHER_LEVEL_OF_DETAIL]);
    lists.add_neighbor_list(&neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_LEAF]);
    lists.add_neighbor_list(&neighbor_lists[NL_LOWER_LEVEL_OF_DETAIL_LEAF]);
    for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
        if (node->v.n->has_water()) {
            /* This velocity is relevant, use it to calculate mean velocity vector */
            area[node->v.pos_dir].e[node->v.dim] += node->v.cf_area;
            mean_vel.e[node->v.dim] += node->v.cf_area * node->v.get_signed_dir() * node->v.vel_out;
        }
    }
    for (uint dim = 0; dim < NUM_DIMENSIONS; dim++) {
        pftype dim_area = area[0].e[dim] + area[1].e[dim];
        if (dim_area) {
            mean_vel.e[dim] /= dim_area;
        }
        else {
            /* Don't know the velocity in this direction */
            // TODO: Find out the velocity in some other way
            //mean_vel[dim] = 0; This componentis already 0
        }
        for (uint pos_dir = 0; pos_dir < 2; pos_dir++) {
            if (area[pos_dir].e[dim] < (7.0/8) * get_side_area()) {
                /* Needs new neighbors at this side */
                /* Not guaranteed though that neighbors will be created; this may be a wall */
                pfvec neighbor_center = get_cell_center();
                neighbor_center.e[dim] += (2*int(pos_dir) - 1) * get_edge_length();
                create_new_air_neighbors(neighbor_center, dim, pos_dir, lvl);
                //TODO:: Set velocities in faces to newly created cells
            }
        }
    }

    /* Set velocities on faces to empty cells */
    /* Loop though neighbors */
    lists.add_neighbor_list(&neighbor_lists[NL_HIGHER_LEVEL_OF_DETAIL]);
    lists.add_neighbor_list(&neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_LEAF]);
    lists.add_neighbor_list(&neighbor_lists[NL_LOWER_LEVEL_OF_DETAIL_LEAF]);
    for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
        if (node->v.n->has_no_water()) {
            /* The velocity out for this neighbor connection currently contains humbug, initialize it */
            node->v.set_velocity_out(mean_vel[node->v.dim]*node->v.get_signed_dir());
        }
    }
}

void octcell::create_new_air_neighbors(pfvec neighbor_center, uint dim, bool pos_dir, uint source_level)
{
#if 0
    static int count = 0;
    count++;
    if (count == 16200) {
        NO_OP();
    }
    cout << count << endl;
#endif
    if (inside_of_cell(neighbor_center)) {
        /* Neighbor is in this cell */
        if (is_fine_enough()) {
            // All neighbors created
            return;
        }
        /* Cell is not fine enough */
        if (is_leaf()) {
            make_parent();
        }
        if (lvl < source_level) {
            /* The cell who wants the nieghbors is at a higher level, find the one neighboring child */
            uint child_idx = get_child_index_from_position(neighbor_center);
            if (!get_child(child_idx)) {
                create_new_air_child(child_idx);
            }
            get_child(child_idx)->create_new_air_neighbors(neighbor_center, dim, pos_dir, source_level);
        }
        else {
            /* The cell who wants the neighbors is not at a higher level and therefore neighbor to half of this cell's children */
            for (uint child_idx = 0; child_idx < MAX_NUM_CHILDREN; child_idx++) {
                if (positive_direction_of_child(child_idx, dim) != pos_dir) {
                    /* Child cell is in the right half */
                    octcell *child = get_child(child_idx);
                    if (!child) {
                        child = create_new_air_child(child_idx);
                    }
                    //child->create_new_air_neighbors(child->get_cell_center(), dim, pos_dir, source_level);
                }
            }
        }
    }
    else if (has_parent()) {
        /* Neighbor is outside of this cell */
        /* Must step up one level to get to position */
        get_parent()->create_new_air_neighbors(neighbor_center, dim, pos_dir, source_level);
    }
    else {
        /* Neighbor is outside of root cell (this cell) */
        // TODO: Create water outside of root cell
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
    pfvec dist = cell2->get_cell_center() - cell1->get_cell_center();
    pftype dist_abs = dist.length();
    pftype min_s = MIN(cell1->s, cell2->s);
    pftype area = cube_side_area(min_s);
    /* Set properties */
    node1->v.set(cell2, node2, dimension,  pos_dir, 0, 0, 0,  dist, dist_abs, area);
    node2->v.set(cell1, node1, dimension, !pos_dir, 0, 0, 0, -dist, dist_abs, area);
}

////////////////////////////////////////////////////////////////
// PRIVATE STATIC METHODS
////////////////////////////////////////////////////////////////

/* The size of a leaf cell should be at the maximum the value of this function applied to its cell center */
pftype octcell::size_accuracy(pfvec r)
{
    if (r.e[VERTICAL_DIMENSION] < SURFACE_HEIGHT) {
        /* Cell is under the surface */
        return SURFACE_ACCURACY + (SURFACE_HEIGHT - r.e[VERTICAL_DIMENSION])
                * (1/(MIN_LOD_LAYER_THICKNESS + 0.5));
    }
    else {
        return SURFACE_ACCURACY;
    }
}
