////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

// Own includes
#include "watersystem.h"

////////////////////////////////////////////////////////////////
// MACROS
////////////////////////////////////////////////////////////////

#define  DECLARE_RECURSIVE_LEAF_CELL_FUNCTION(function, cell)        \
    if ((cell)->has_child_array()) {                                 \
        for (uint idx = 0; idx < octcell::MAX_NUM_CHILDREN; idx++) { \
            octcell* c = (cell)->get_child(idx);                     \
            if (c) {                                                 \
                /* Child exists */                                   \
                (function)(c);                                       \
            }                                                        \
        }                                                            \
        return;                                                      \
    }

#define  LOOP_THROUGH_ALL_LEAF_NEIGHBORS(cell, node)                                  \
    for (nlset lists,                                                                 \
    lists.add_neighbor_list(&(cell)->neighbor_lists[NL_HIGHER_LEVEL_OF_DETAIL]),      \
    lists.add_neighbor_list(&(cell)->neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_LEAF]),   \
    lists.add_neighbor_list(&(cell)->neighbor_lists[NL_LOWER_LEVEL_OF_DETAIL_LEAF]),  \
    nlnode* node = lists.get_first_node(); node; node = lists.get_next_node())

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
    advect_and_update_pressure();
    update_velocities_recursively(w->root);
}

#if 1
void watersystem::advect_and_update_pressure()
{
    //TODO: Ensure mass conservation (current alpha advection do not conserve the mass)
    /* Advect alpha */
    //calculate_delta_alpha_recursively(w->root);
    calculate_cell_face_properties_recursivelly(w->root);
    //clamp_advect_alpha_recursively(w->root);

#if 0
    /* Sharpen alpha */
    calculate_alpha_gradient_recursively(w->root);
    sharpen_alpha_recursively(w->root);
#endif

    /* Update pressure */
    advect_cell_properties_recursivelly(w->root);
    //update_pressure_recursively(w->root);
}

void watersystem::calculate_cell_face_properties_recursivelly(octcell* cell)
{
    DECLARE_RECURSIVE_LEAF_CELL_FUNCTION(calculate_cell_face_properties_recursivelly, cell);

    /* Loop through neighbors */
    nlset lists;
    lists.add_neighbor_list(&cell->neighbor_lists[NL_HIGHER_LEVEL_OF_DETAIL]);
    lists.add_neighbor_list(&cell->neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_LEAF]);
    lists.add_neighbor_list(&cell->neighbor_lists[NL_LOWER_LEVEL_OF_DETAIL_LEAF]);
    for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
        if (node->v.vel_out > 0) {
            // UPWIND Scheme (smearing) //TODO: Change to better scheme!!!
            node->v.set_densities(cell->water_density, cell->total_density);
        }
    }
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
#elif 0
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
#elif 0
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
    cell->dalpha = 0; // Reset alpha gradient
    /* Loop though neighbors */
    nlset lists;
    lists.add_neighbor_list(&cell->neighbor_lists[NL_HIGHER_LEVEL_OF_DETAIL]);
    lists.add_neighbor_list(&cell->neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_LEAF]);
    lists.add_neighbor_list(&cell->neighbor_lists[NL_LOWER_LEVEL_OF_DETAIL_LEAF]);
    for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
        if (node->v.vel_out < 0) {
            cell->dalpha -= node->v.vel_out * (node->v.n->alpha - cell->alpha)*node->v.cf_area;
        }
    }
    cell->dalpha /= cell->get_side_area();
    cell->dalpha *= dt;
}
#endif

#if 0
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

    /* Calculate d-alpha */
    pftype dwater_density;
    pftype dtotal_density;
    // TODO: Add code for calculating dwater_vol_coeff and dtotal_vol_coeff

    /* Check if cell goes from air cell to non-air cell */
    if (cell->is_air_cell() && dwater_density) {
        /* Set velocities on faces to empty cells */
        pfvec mean_vel;
        pfvec area;
        /* Loop though neighbors */
        nlset lists;
        lists.add_neighbor_list(&cell->neighbor_lists[NL_HIGHER_LEVEL_OF_DETAIL]);
        lists.add_neighbor_list(&cell->neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_LEAF]);
        lists.add_neighbor_list(&cell->neighbor_lists[NL_LOWER_LEVEL_OF_DETAIL_LEAF]);
        for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
            if (node->v.n->is_non_air_cell()) {
                /* This velocity is relevant, use it to calculate mean velocity vector */
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
            if (node->v.n->is_air_cell()) {
                /* The velocity out for this neighbor connection currently contains humbug, initialize it */
                node->v.set_velocity_out(mean_vel[node->v.dim]*node->v.get_signed_dir());
            }
        }
    }

    /* Update volumes */

    cell->water_density += dwater_density;
    cell->total_density += dtotal_density;
    if (cell->water_density < 0) {
#if  DEBUG
        throw logic_error("Water density becaue negative");
#endif
        //cell->water_density = 0;
    }
    else if (cell->water_density > cell->total_density) {
#if  DEBUG
        throw logic_error("Water density becaue larger than total density");
#endif
        //cell->water_density = cell->total_density;
    }
}
#endif

#if 0
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
#endif

void watersystem::advect_cell_properties_recursivelly(octcell* cell)
{
    if (cell->has_child_array()) {
        /* */
        for (uint idx = 0; idx < octcell::MAX_NUM_CHILDREN; idx++) {
            octcell* c = cell->get_child(idx);
            if (c) {
                /* Child exists */
                advect_cell_properties_recursivelly(c);
            }
        }
        return;
    }

    /* TODO: Update the code for updating pressure */
    /* Update pressure */
    pftype in_water_flux = 0; // [kg/s]
    pftype in_total_flux = 0; // [kg/s]
    /* Loop through neighbors */
    nlset lists;
    lists.add_neighbor_list(&cell->neighbor_lists[NL_HIGHER_LEVEL_OF_DETAIL]);
    lists.add_neighbor_list(&cell->neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_LEAF]);
    lists.add_neighbor_list(&cell->neighbor_lists[NL_LOWER_LEVEL_OF_DETAIL_LEAF]);
    for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
        pftype volume_flux_out = node->v.vel_out * node->v.cf_area; // [m^3/s]
        in_water_flux -= node->v.water_density * volume_flux_out;
        in_total_flux -= node->v.total_density * volume_flux_out;
    }

#if 0
    pftype mass_flux_to_density_factor = dt/cell->get_cube_volume();
    pftype in_water_density = in_water_flux * mass_flux_to_density_factor;
    pftype in_total_density = in_total_flux * mass_flux_to_density_factor;

    if (dt) {
        throw logic_error("dt is non-zero");
    }
    if (mass_flux_to_density_factor) {
        throw logic_error("mass_flux_to_density_factor is non-zero");
    }
    if (in_water_density) {
        throw logic_error("in_water_density is non-zero");
    }
    if (in_total_density) {
        throw logic_error("in_total_density is non-zero");
    }
#else

    float C = in_total_flux;


    float A = 0;
    float B = C * A;
    cout << "C = " << C << endl;
    if (A) {
        cout << "A = " << A << endl;
        throw logic_error("A is non-zero");
    }
    if (B) {
        cout << "B = " << B << endl;
        throw logic_error("B is non-zero");
    }

    pftype in_water_density = 0;
    pftype in_total_density = 0;
#endif

#if 1
    cell->water_density += in_water_density;
    cell->total_density += in_total_density;
#endif

#if  USE_ARTIFICIAL_COMPRESSIBILITY
    /* Update pressure */
    if (cell->is_water_cell()) {
        cell->p = (cell->water_density - P_WATER_DENSITY) * ARTIFICIAL_COMPRESSIBILITY_FACTOR;
        if (cell->p < 0) {
            cell->p = 0; // Vacuum partly fills the cell
        }
    }
    else if (cell->is_air_cell()) {
        cell->p = cell->total_density * (NORMAL_PRESSURE / P_NORMAL_AIR_DENSITY);
        //cell->p = 0; // This is for zero atmospheric pressure
    }
    else {
        /* Calculate pressure for a mixed cell */
        //TODO: Optimize
        pftype k = ARTIFICIAL_COMPRESSIBILITY_FACTOR;
        pftype q = NORMAL_PRESSURE;
        pftype a = cell->get_air_density()/P_NORMAL_AIR_DENSITY;
        pftype w = cell->water_density/P_WATER_DENSITY;
        pftype d = q*a/k;
        cell->p = k/2*(sqrt(SQUARE(d + 1 - w) + 4*d*w) + d + w + 1);
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
