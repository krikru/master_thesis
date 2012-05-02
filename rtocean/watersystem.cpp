////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

// Own includes
#include "watersystem.h"

////////////////////////////////////////////////////////////////
// CONSTRUCTORS AND DESTRUCTOR
////////////////////////////////////////////////////////////////

watersystem::watersystem()
{
    w = 0;
    t = 0;
    operating = false;

}

////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////

/* Control */

void watersystem::evolve()
{
    start_operation();
#if  DEBUG
    if (!dt) {
        throw logic_error("Trying to evolve system but time step is not defined");
    }
    if (!water_defined()) {
        throw logic_error("Trying to evolve system while water is not defined");
    }
#endif
    _evolve();
    finish_operation();
}

int watersystem::run_simulation(pftype time_step)
{
    start_operation();
    int ret = 0;
    set_time_step(time_step);

    pause = false;
    do {
        /* Evolve the system */
        _evolve();
        /* Process everything that needs to be processed */
        process_events();
    } while (!abort && !pause);

    if (abort) {
        ret = 1;
    }
    else if (pause) {
        ret = 0;
    }

    finish_operation();
    return ret;
}

////////////////////////////////////////////////////////////////
// PRIVATE METHODS
////////////////////////////////////////////////////////////////

void watersystem::_evolve()
{
    t += dt;
    if (0) {
        //advect_and_update_pressure();
    } else {
        advect_and_update_pressure_recursively(w->root);
    }
    update_velocities_recursively(w->root);
}

#if 0
void watersystem::advect_and_update_pressure()
{
    //TODO: Ensure mass conservation (current alpha advection do not conserve the mass)
    /* Advect alpha */
    calculate_alpha_gradient_recursively(w->root);
    advect_alpha_recursively(w->root);

    /* Sharpen alpha */
    calculate_alpha_gradient_recursively(w->root);

    /* Update pressure */
    update_pressure_recursively(w->root);
}

void watersystem::calculate_alpha_gradient_recursively(octcell* cell)
{
    if (cell->has_child_array()) {
        for (uint idx = 0; idx < octcell::MAX_NUM_CHILDREN; idx++) {
            octcell* c = cell->get_child(idx);
            if (c) {
                /* Child exists */
                calculate_alpha_gradient_recursively(c);
            }
        }
        return;
    }

    /* Calculate alpha gradient */
    cell->alpha_grad = pfvec(); // Reset alpha gradient
    nlset lists;
    lists.add_neighbor_list(&cell->neighbor_lists[NL_HIGHER_LEVEL_OF_DETAIL]);
    lists.add_neighbor_list(&cell->neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_LEAF]);
    lists.add_neighbor_list(&cell->neighbor_lists[NL_LOWER_LEVEL_OF_DETAIL_LEAF]);
    for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
        cell->alpha_grad.e[node->v.dim] += (node->v.n->alpha - cell->alpha) * (node->v.pos_dir ? node->v.cf_area : -node->v.cf_area);
    }
    for (uint dim = 0; dim < NUM_DIMENSIONS; dim++) {
        cell->alpha_grad.e[dim] /= 2 * cell->get_total_volume();
    }
}

void watersystem::advect_alpha_recursively(octcell* cell)
{
    if (cell->has_child_array()) {
        for (uint idx = 0; idx < octcell::MAX_NUM_CHILDREN; idx++) {
            octcell* c = cell->get_child(idx);
            if (c) {
                /* Child exists */
                advect_alpha_recursively(c);
            }
        }
        return;
    }

    /* Calculate mean velocity */
    pfvec v = pfvec(); // Velocity
    nlset lists;
    lists.add_neighbor_list(&cell->neighbor_lists[NL_HIGHER_LEVEL_OF_DETAIL]);
    lists.add_neighbor_list(&cell->neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_LEAF]);
    lists.add_neighbor_list(&cell->neighbor_lists[NL_LOWER_LEVEL_OF_DETAIL_LEAF]);
    for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
        v.e[node->v.dim] += node->v.vel * node->v.cf_area;
    }
    for (uint dim = 0; dim < NUM_DIMENSIONS; dim++) {
        v.e[dim] /= 2 * cell->get_side_area();
    }

    /* Update alpha */
    cell->alpha -= cell->alpha_grad * v * dt;
}

void watersystem::update_pressure_recursively(octcell* cell)
{
    if (cell->has_child_array()) {
        /* */
        for (uint idx = 0; idx < octcell::MAX_NUM_CHILDREN; idx++) {
            octcell* c = cell->get_child(idx);
            if (c) {
                /* Child exists */
                calculate_alpha_gradient_recursively(c);
            }
        }
        return;
    }

    /* TODO: Update the code for updating pressure */
    /* Update pressure */
    pftype in_flux = 0;
    nlset lists;
    lists.add_neighbor_list(&cell->neighbor_lists[NL_HIGHER_LEVEL_OF_DETAIL]);
    lists.add_neighbor_list(&cell->neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_LEAF]);
    lists.add_neighbor_list(&cell->neighbor_lists[NL_LOWER_LEVEL_OF_DETAIL_LEAF]);
    for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
        in_flux += (node->v.pos_dir ? -1 : +1) * node->v.vel * node->v.cf_area;
    }

#if  USE_ARTIFICIAL_COMPRESSIBILITY
    /* Move volume into or from cell */
    pftype in_volume = in_flux * dt;
    if (cell->is_surface_cell()) {
        cell->rp += in_volume / cell->get_side_area() * P_G;
    }
    else {
        cell->rp += in_volume / cell->get_total_volume() * ARTIFICIAL_COMPRESSIBILITY_FACTOR;
    }
#else
    "don't know what to do now"
#endif
}
#endif

//TODO: Remove this function once replaced
/* Returns true if the cell should be removed from the simulation, false otherwise*/
bool watersystem::advect_and_update_pressure_recursively(octcell* cell)
{
    if (cell->has_child_array()) {
        for (uint idx = 0; idx < octcell::MAX_NUM_CHILDREN; idx++) {
            octcell* c = cell->get_child(idx);
            if (c) {
                /* Child exists */
                if (advect_and_update_pressure_recursively(c)) {
                    cell->remove_child(idx);
                    if (!cell->get_number_of_children()) {
                        /* No children left, remove this cell as well */
                        return true;
                    }
                }
            }
        }
        return false;
    }

    /* Cell is a leaf cell */
    /* Calculate in flux */
    pftype in_flux = 0;
    nlset lists;
    //nlnode* nn_up   = 0;
    //nlnode* nn_down = 0;
    lists.add_neighbor_list(&cell->neighbor_lists[NL_HIGHER_LEVEL_OF_DETAIL]);
    lists.add_neighbor_list(&cell->neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_LEAF]);
    lists.add_neighbor_list(&cell->neighbor_lists[NL_LOWER_LEVEL_OF_DETAIL_LEAF]);
    for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
        if (node->v.dim == VERTICAL_DIMENSION) {
            /*
            if (node->v.pos_dir) {
                nn_up = node;
            }
            else {
                nn_down = node;
            }
            */
        }
        in_flux += (node->v.pos_dir ? -1 : +1) * node->v.vel * node->v.cf_area;
    }

    /* Move volume into or from cell */
    pftype in_volume = in_flux * dt;
    if (cell->is_surface_cell()) {
        cell->alpha += in_volume/cell->get_total_volume();
#if  USE_ARTIFICIAL_COMPRESSIBILITY
        cell->rp += in_volume / cell->get_side_area() * P_G;
#endif
#if 0
        if (cell->vof > cell->get_total_volume()) {
            pftype excess_volume = cell->vof - cell->get_total_volume();
            cell->un_surface_cell();
            /* Move up water to cell above */
            if (nn_up) {
                // TODO: Add code
            }
            else {
                // TODO: Add code
            }
        }
        else if (cell->vof <= 0) {
            /* Cancel out with water from cell beneath */
            if (nn_down) {
                if (!nn_down.v.n->is_surface_cell()) {
                    nn_down.v.n->make_surface_cell();
                    nn_down.remove_water(-cell->vof);
                }
                else {
                    /* Remove from all neighbors */
                }
            }
            return true;
            // TODO: Add code
        }
        else {
            /* Assume a pressure that is increasing linearly from the surface and down; allow negative pressures for points above the surface */
            //cell->rp = (cell->vof/cell->get_side_area() - 0.5*cell->s) * P_G;
        }
#endif
    }
    else {
#if  USE_ARTIFICIAL_COMPRESSIBILITY
        cell->rp += in_volume / cell->get_total_volume() * ARTIFICIAL_COMPRESSIBILITY_FACTOR;
#endif
    }

    return false;
}

void watersystem::update_velocities_recursively(octcell* cell)
{
    if (cell->has_child_array()) {
        for (uint idx = 0; idx < octcell::MAX_NUM_CHILDREN; idx++) {
            octcell* c = cell->get_child(idx);
            if (c) {
                /* Child exists */
                update_velocities_recursively(c);
            }
        }
        return;
    }

    /* Cell is a leaf cell */
    nlset lists;
    lists.add_neighbor_list(&cell->neighbor_lists[NL_HIGHER_LEVEL_OF_DETAIL]);
    lists.add_neighbor_list(&cell->neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_LEAF]);
    lists.add_neighbor_list(&cell->neighbor_lists[NL_LOWER_LEVEL_OF_DETAIL_LEAF]);
    for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
        if (node->v.should_calculate_new_velocity()) {
            node->v.update_velocity(cell, node->v.n, dt);
        }
    }
}

/* Thread safety */

void watersystem::start_operation()
{
    while (operating) {
        // Wait for the other operation to finish
        process_events();
    }
    operating = true;
    abort = false;
}

void watersystem::finish_operation()
{
    if (!operating) {
        throw logic_error("Tried to finish operation while not operating");
    }
    operating = false;
}