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
#if 1
    advect_and_update_pressure();
#else
    advect_and_update_pressure_recursively(w->root);
#endif
    update_velocities_recursively(w->root);
}

#if 1
void watersystem::advect_and_update_pressure()
{
    //TODO: Ensure mass conservation (current alpha advection do not conserve the mass)
    /* Advect alpha */
    calculate_delta_alpha_recursively(w->root);
    clamp_advect_alpha_recursively(w->root);

#if 0
    /* Sharpen alpha */
    calculate_alpha_gradient_recursively(w->root);
    sharpen_alpha_recursively(w->root);
#endif

    /* Update pressure */
    update_pressure_recursively(w->root);
}

#if 0
void watersystem::calculate_delta_alpha_recursively(octcell* cell)
{
    if (cell->has_child_array()) {
        for (uint idx = 0; idx < octcell::MAX_NUM_CHILDREN; idx++) {
            octcell* c = cell->get_child(idx);
            if (c) {
                /* Child exists */
                calculate_delta_alpha_recursively(c);
            }
        }
        return;
    }

    /* Calculate delta alpha */
    cell->dalpha = 0; // Reset delta alpha
    /* Loop through neighbors */
    nlset lists;
    lists.add_neighbor_list(&cell->neighbor_lists[NL_HIGHER_LEVEL_OF_DETAIL]);
    lists.add_neighbor_list(&cell->neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_LEAF]);
    lists.add_neighbor_list(&cell->neighbor_lists[NL_LOWER_LEVEL_OF_DETAIL_LEAF]);
    for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
        if (node->v.vel_out < 0) {
            cell->dalpha -= node->v.vel_out * (node->v.n->alpha - cell->alpha) * node->v.cf_area;
        }
#if  DEBUG
#if  !QUICKFIX1
        if (node->v.vel_out * dt * 4 < -cell->s) {
            throw domain_error("Velocity to high for alpha advection");
        }
#endif
#endif
    }
    cell->dalpha *= dt / cell->get_total_volume();
#if  QUICKFIX1
    if (cell->dalpha < -cell->alpha) {
        cell->dalpha = -cell->alpha;
    }
    else if (cell->dalpha > (1-cell->alpha)) {
        cell->dalpha = (1-cell->alpha);
#if  DEBUG
        if (cell->alpha + cell->dalpha > 1) {
            throw logic_error("Damn...");
        }
#endif
    }
#endif
}
#else
void watersystem::calculate_delta_alpha_recursively(octcell* cell)
{
    if (cell->has_child_array()) {
        for (uint idx = 0; idx < octcell::MAX_NUM_CHILDREN; idx++) {
            octcell* c = cell->get_child(idx);
            if (c) {
                /* Child exists */
                calculate_delta_alpha_recursively(c);
            }
        }
        return;
    }

    /* Calculate mean velocity and alpha gradient */
    cell->alpha_grad_coeff = pfvec(); // Reset alpha gradient
    pfvec mean_vel;
    pfvec area;
    /* Loop though neighbors */
    nlset lists;
    lists.add_neighbor_list(&cell->neighbor_lists[NL_HIGHER_LEVEL_OF_DETAIL]);
    lists.add_neighbor_list(&cell->neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_LEAF]);
    lists.add_neighbor_list(&cell->neighbor_lists[NL_LOWER_LEVEL_OF_DETAIL_LEAF]);
    for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
        //cell->alpha_grad.e[node->v.dim] += (node->v.n->alpha - cell->alpha) * (node->v.pos_dir ? node->v.cf_area : -node->v.cf_area);
        cell->alpha_grad_coeff.e[node->v.dim] += (node->v.n->alpha - cell->alpha) * (node->v.pos_dir ? node->v.cf_area : -node->v.cf_area);
        if (node->v.n->is_non_empty()) {
            area.e[node->v.dim] += node->v.cf_area;
            mean_vel.e[node->v.dim] += node->v.cf_area * node->v.get_signed_dir() * node->v.vel_out;
        }
    }
    for (uint dim = 0; dim < NUM_DIMENSIONS; dim++) {
        if (area.e[dim]) {
            mean_vel.e[dim] /= area[dim];
        }
        else {
            /* Don't know the velocity in this direction */
            // TODO: Find out the velocity in some other way
            //mean_vel[dim] = 0; This componentis already 0
        }
    }

    //TODO: Optimize
    pftype ideal_length = 1/(INTERFACE_THICKNESS_IN_CELLS * cell->s); // [1/m]
    pftype square_length = cell->alpha_grad_coeff.sqr_length(); // [1/m]
    if (!square_length) {
        return;
    }
    pftype real_length = sqrt(square_length);
    pftype start_distance_in_cells;
    if (cell->alpha <= 0 || cell->alpha >= 1) {
        /* The cell will need some start distance before getting into the interface */
        pftype cells_from_interface = 1/((ideal_length - 2*real_length)/ideal_length); // [1] How many cells outside the interface
        start_distance_in_cells = MAX(cells_from_interface, 0); // [1] The start distance in cells before reaching the interface
    }
    else {
        /* The cell is already in the interface and needs no start distance */
        start_distance_in_cells = 0;
    }
    /* Give the gradient it's proper magnitude */
    cell->alpha_grad_coeff *= ideal_length / real_length;
    cell->dalpha = -cell->alpha_grad_coeff * mean_vel * dt;
    cell->dalpha -= (cell->dalpha > 0 ? 1 : -1)/INTERFACE_THICKNESS_IN_CELLS * start_distance_in_cells;
}
#endif

void watersystem::clamp_advect_alpha_recursively(octcell* cell)
{
    if (cell->has_child_array()) {
        for (uint idx = 0; idx < octcell::MAX_NUM_CHILDREN; idx++) {
            octcell* c = cell->get_child(idx);
            if (c) {
                /* Child exists */
                clamp_advect_alpha_recursively(c);
            }
        }
        return;
    }

    /* Check if cell goes from empty to non-empty */
    if (cell->is_empty() && cell->dalpha) {
        /* Set velocities on faces to empty cells */
        pfvec mean_vel;
        pfvec area;
        /* Loop though neighbors */
        nlset lists;
        lists.add_neighbor_list(&cell->neighbor_lists[NL_HIGHER_LEVEL_OF_DETAIL]);
        lists.add_neighbor_list(&cell->neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_LEAF]);
        lists.add_neighbor_list(&cell->neighbor_lists[NL_LOWER_LEVEL_OF_DETAIL_LEAF]);
        for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
            if (node->v.n->is_non_empty()) {
                area.e[node->v.dim] += node->v.cf_area;
                mean_vel.e[node->v.dim] += node->v.cf_area * node->v.get_signed_dir() * node->v.vel_out;
            }
        }
        for (uint dim = 0; dim < NUM_DIMENSIONS; dim++) {
            if (area.e[dim]) {
                mean_vel.e[dim] /= area[dim];
            }
            else {
                /* Don't know the velocity in this direction */
                // TODO: Find out the velocity in some other way
                //mean_vel[dim] = 0; This componentis already 0
            }
        }
        /* Loop though neighbors */
        lists.add_neighbor_list(&cell->neighbor_lists[NL_HIGHER_LEVEL_OF_DETAIL]);
        lists.add_neighbor_list(&cell->neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_LEAF]);
        lists.add_neighbor_list(&cell->neighbor_lists[NL_LOWER_LEVEL_OF_DETAIL_LEAF]);
        for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
            if (node->v.n->is_empty()) {
                /* The velocity out for this neighbor connection currently contains humbug, initialize it */
                node->v.set_velocity_out(mean_vel[node->v.dim]*node->v.get_signed_dir());
            }
        }
    }

    /* Update alpha */
    cell->alpha += cell->dalpha;
    if (cell->alpha < 0) {
        cell->alpha = 0;
    }
    else if (cell->alpha > 1) {
        cell->alpha = 1;
    }
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
    cell->alpha_grad_coeff = pfvec(); // Reset alpha gradient
    /* Loop though neighbors */
    nlset lists;
    lists.add_neighbor_list(&cell->neighbor_lists[NL_HIGHER_LEVEL_OF_DETAIL]);
    lists.add_neighbor_list(&cell->neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_LEAF]);
    lists.add_neighbor_list(&cell->neighbor_lists[NL_LOWER_LEVEL_OF_DETAIL_LEAF]);
    for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
        //cell->alpha_grad.e[node->v.dim] += (node->v.n->alpha - cell->alpha) * (node->v.pos_dir ? node->v.cf_area : -node->v.cf_area);
        cell->alpha_grad_coeff.e[node->v.dim] += node->v.n->alpha * (node->v.pos_dir ? node->v.cf_area : -node->v.cf_area);
    }
    for (uint dim = 0; dim < NUM_DIMENSIONS; dim++) {
        cell->alpha_grad_coeff.e[dim] /= 2 * cell->get_side_area();
    }
}

void watersystem::sharpen_alpha_recursively(octcell* cell)
{
    static int count = 0;

    if (cell->has_child_array()) {
        for (uint idx = 0; idx < octcell::MAX_NUM_CHILDREN; idx++) {
            octcell* c = cell->get_child(idx);
            if (c) {
                /* Child exists */
                sharpen_alpha_recursively(c);
            }
        }
        return;
    }

    count++;
    if (count == 273996) {
        count = count;
    }

    /* Sharpen alpha */
    if (cell->alpha <= 0 || cell->alpha >= 1) {
        /* Cell needs no sharpening */
        return;
    }

    /* Determine if the cell should donate or accept and how much */
    pftype abs_deriv_sum = 0;
    for (uint dim = 0; dim < NUM_DIMENSIONS; dim++) {
        abs_deriv_sum += ABS(cell->alpha_grad_coeff[dim]);
    }
    if (!abs_deriv_sum) {
        /* No way to transport alpha */
        return;
    }
#if  DEBUG
    if (abs_deriv_sum < 0) {
        throw logic_error("Derivative sum < 0");
    }
#endif
    pftype donate_factor = 3/abs_deriv_sum;
    pftype accept_factor = donate_factor;
    /* Loop through neighbors */
    nlset lists;
    lists.add_neighbor_list(&cell->neighbor_lists[NL_HIGHER_LEVEL_OF_DETAIL]);
    lists.add_neighbor_list(&cell->neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_LEAF]);
    lists.add_neighbor_list(&cell->neighbor_lists[NL_LOWER_LEVEL_OF_DETAIL_LEAF]);
    for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
        pftype dir_alpha_grad_coeff = cell->alpha_grad_coeff[node->v.dim] * node->v.get_signed_dir();
        if      (dir_alpha_grad_coeff > 0) {
            /* Positive derivative, this cell is a donor */
            pftype real_cap = (1 - node->v.n->alpha) / dir_alpha_grad_coeff;
            if (real_cap < donate_factor) {
                donate_factor = real_cap;
            }
        }
        else if (dir_alpha_grad_coeff < 0) {
            /* Negative derivative, this cell is an acceptor */
            pftype real_cap = node->v.n->alpha / -dir_alpha_grad_coeff;
            if (real_cap < accept_factor) {
                accept_factor = real_cap;
            }
        }
    }
#if  DEBUG
    if (donate_factor < 0) {
        throw logic_error("Donate factor < 0");
    }
    if (accept_factor < 0) {
        throw logic_error("Accept factor < 0");
    }
#endif
    if (donate_factor > accept_factor) {
        donate_factor = MIN(donate_factor, accept_factor + cell->alpha       / abs_deriv_sum);
    }
    else {
        accept_factor = MIN(accept_factor, donate_factor + (1 - cell->alpha) / abs_deriv_sum);
    }
    /* Donate and accept alpha */
    /* Loop through neighbors */
    lists.add_neighbor_list(&cell->neighbor_lists[NL_HIGHER_LEVEL_OF_DETAIL]);
    lists.add_neighbor_list(&cell->neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_LEAF]);
    lists.add_neighbor_list(&cell->neighbor_lists[NL_LOWER_LEVEL_OF_DETAIL_LEAF]);
    for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
        pftype dir_alpha_grad_coeff = cell->alpha_grad_coeff[node->v.dim] * node->v.get_signed_dir();
        /* Positive derivative, this cell is a donor */
        if (dir_alpha_grad_coeff > 0) {
            /* Positive derivative, this cell is a donor */
            pftype transported_alpha = donate_factor * dir_alpha_grad_coeff;
#if  DEBUG
            if (transported_alpha < 0) {
                throw logic_error("Transported alpha has wrong sign 1");
            }
#endif
            cell->alpha -= transported_alpha;
            node->v.n->alpha += transported_alpha;
            if (node->v.n->alpha > 1) {
                node->v.n->alpha = 1;
            }
        }
        else {
            /* Negative derivative, this cell is an acceptor */
            pftype transported_alpha = accept_factor * dir_alpha_grad_coeff;
#if  DEBUG
            if (transported_alpha > 0) {
                throw logic_error("Transported alpha has wrong sign 2");
            }
#endif
            cell->alpha -= transported_alpha;
            node->v.n->alpha += transported_alpha;
            if (node->v.n->alpha < 0) {
                node->v.n->alpha = 0;
            }
        }
    }
    if (cell->alpha < 0) {
        cell->alpha = 0;
    }
    else if (cell->alpha > 1) {
        cell->alpha = 1;
    }
}

void watersystem::update_pressure_recursively(octcell* cell)
{
    if (cell->has_child_array()) {
        /* */
        for (uint idx = 0; idx < octcell::MAX_NUM_CHILDREN; idx++) {
            octcell* c = cell->get_child(idx);
            if (c) {
                /* Child exists */
                update_pressure_recursively(c);
            }
        }
        return;
    }

    /* TODO: Update the code for updating pressure */
    /* Update pressure */
    pftype in_flux = 0;
    /* Loop through neighbors */
    nlset lists;
    lists.add_neighbor_list(&cell->neighbor_lists[NL_HIGHER_LEVEL_OF_DETAIL]);
    lists.add_neighbor_list(&cell->neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_LEAF]);
    lists.add_neighbor_list(&cell->neighbor_lists[NL_LOWER_LEVEL_OF_DETAIL_LEAF]);
    for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
        in_flux -= node->v.vel_out * node->v.cf_area;
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

#if 0
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
    /* Loop through neighbors */
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
        in_flux -= node->v.vel_out * node->v.cf_area;
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
#endif

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
    /* Loop through neighbors */
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
