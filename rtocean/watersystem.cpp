////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

// Own includes
#include "watersystem.h"
#include "physics.h"

#if  DEBUG
#include <iostream>
#include <iomanip>
using std::cout;
using std::endl;
using std::setprecision;
#endif

////////////////////////////////////////////////////////////////
// MACROS
////////////////////////////////////////////////////////////////

#define  DECLARE_RECURSIVE_LEAF_CELL_FUNCTION(function, cell)        \
    if ((cell)->has_child_array()) {                                 \
        for (uint idx = 0; idx < octcell::MAX_NUM_CHILDREN; idx++) { \
            if ((cell)->has_child(idx)) {                            \
                /* Child exists */                                   \
                octcell* c = (cell)->get_child(idx);                 \
                (function)(c);                                       \
            }                                                        \
        }                                                            \
        return;                                                      \
    }

////////////////////////////////////////////////////////////////
// CONSTRUCTORS AND DESTRUCTOR
////////////////////////////////////////////////////////////////

watersystem::watersystem()
{
    w = 0;
    max_v = 0;
    operating = false;
    num_time_steps_before_resting = 1;
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
    if (!is_water_defined()) {
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

    started = true;
    paused = false;
    uint num_time_steps_until_resting = num_time_steps_before_resting;
    while (!abort && !(paused && break_main_loop_when_pausing)) {
        if (!num_time_steps_until_resting) {
            /* Process everything that needs to be processed */
            process_events();
            num_time_steps_until_resting = num_time_steps_before_resting - 1;
        }
        else {
            num_time_steps_until_resting--;
        }

        /* Evolve the system */
        if (!paused) {
            _evolve();
        }
    }

    if (abort) {
        ret = SR_ABORTED;
    }
    else if (paused) {
        ret = SR_PAUSED;
    }

    finish_operation();
    return ret;
}

////////////////////////////////////////////////////////////////
// PRIVATE METHODS
////////////////////////////////////////////////////////////////

void watersystem::_evolve()
{
    static size_t current_print_screen_index = 0;
    /* Check if dt needs to be decreased */
    if (max_v) {
        dt *= MAX_RECOMMENDED_V / max_v;
        if (dt > max_dt) {
            dt = max_dt;
        }
    }
    else {
        dt = max_dt;
    }
    max_v = 0;
    /* Check if next_print_screen is going to be taken after this time step */
    bool take_printscreen_after_time_step = false;
    if (current_print_screen_index < NUM_PRINTSCREEN_TIMES &&
            t + dt >= PRINTSCREEN_TIMES[current_print_screen_index]) {
        take_printscreen_after_time_step = true;
        dt = PRINTSCREEN_TIMES[current_print_screen_index] - t;
#if  DEBUG
        if (dt < 0) {
            throw logic_error("Want's to step back in time because if printscreen");
        }
#endif
    }
    /* Update the time */
    if (take_printscreen_after_time_step) {
        t = PRINTSCREEN_TIMES[current_print_screen_index];
        current_print_screen_index++;
    }
    else {
        t += dt;
    }
    /* Calculate cell-center velocity vectors */
    calculate_cell_center_properties_recursively(w->root);
    /*
     * Calculate cell-face alpha
     * Calculate cell-face quasi-momentum vectors using the cell-face alpha and an UPWIND scheme.
     */
    calculate_cell_face_properties_recursively(w->root);
    /*
     * Advect mass
     * Calculate the net quasi-momentum inflow in each cell
     * Calculate the quasi-momentum increase in cell-faces due to increase of density in cells and remove that value from the net quasi-momentum increase
     */
    advect_cell_properties_recursivelly(w->root);

    /* Convert cell-face velocity out to quasi-momentum out */
    //convert_cell_face_vel_out_to_quasi_momentum_out_recursively(w->root);
    //TODO: Distribute the remainding net quasi-momentum in the cells on the cell faces equaly per unit area
    distribute_ceLl_quasi_momentum_on_cell_faces_recursively(w->root);
    /* Convert cell-face quasi-momentum out to velocity out */
    //convert_cell_face_quasi_momentum_out_to_vel_out_recursively(w->root);

    update_velocities_by_the_pressure_gradients_recursively(w->root);

    if (take_printscreen_after_time_step && take_printscreen_callback.is_defined()) {
        take_printscreen_callback.func(take_printscreen_callback.param);
    }
}

/*
 * Calculates cell center velocity vector
 */
void watersystem::calculate_cell_center_properties_recursively(octcell* cell)
{
    DECLARE_RECURSIVE_LEAF_CELL_FUNCTION(calculate_cell_face_properties_recursively, cell);

    /* Reset cell-center velocity */
    cell->ccv = pfvec();

    /* Calculate new cell-center velocity averaged from the cell faces */
    if (cell->has_fluid()) {
        pfvec weights; /* The weights in all three directins */
        pftype own_cell_density = cell->get_density();
        nlset lists;
        cell->add_leaf_neighbor_lists_to_list_set(lists);
        for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
            pftype weight = node->v.cf_area*(own_cell_density + node->v.n->get_density());
            cell->ccv[node->v.dim] += weight * node->v.get_vel_in_pos_dir();
            weights[node->v.dim] += weight;
        }
        for (uint dim = 0; dim < NUM_DIMENSIONS; dim++) {
            if (weights[dim]) {
                cell->ccv[dim] /= weights[dim];
            }
            else {
                //TODO: Handle this case (it can occur for empty surface cells when the velocity shound't be non-zero)
            }
        }
    }
}

/*
 * Calculates cell face alpha
 */
void watersystem::calculate_cell_face_properties_recursively(octcell* cell)
{
    DECLARE_RECURSIVE_LEAF_CELL_FUNCTION(calculate_cell_face_properties_recursively, cell);

    /* Calculate cell-face alpha */
    nlset lists;
    cell->add_leaf_neighbor_lists_to_list_set(lists);
#if    ALPHA_ADVECTION_SCHEME == UPWIND
    /* UPWIND gives smearing!!! */
    for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
        if (node->v.vel_out > 0) {
            node->v.set_volume_coefficients(cell->water_vol_coeff, cell->total_vol_coeff);
        }
    }
#elif  ALPHA_ADVECTION_SCHEME == HRIC || ALPHA_ADVECTION_SCHEME == HYPER_C || ALPHA_ADVECTION_SCHEME == HIGH_CONTRAST_SCHEME
    /* Find average donor neighbor alpha */
    pftype v = 0; // [1] Courant number
    pftype average_donor_neighbor_alpha; // [1]
    pftype cell_alpha = cell->total_vol_coeff ? cell->get_alpha() : pftype(0); // [1]
    pftype guessed_water_in_volume_flux = 0; // [m^3/s]
    pftype guessed_total_in_volume_flux = 0; // [m^3/s]
    for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
        if (node->v.vel_out < 0) {
            guessed_water_in_volume_flux -= node->v.n->water_vol_coeff * node->v.vel_out * node->v.cf_area;
            guessed_total_in_volume_flux -= node->v.n->total_vol_coeff * node->v.vel_out * node->v.cf_area;
        }
        else {
            pftype face_total_vol_coeff = cell->total_vol_coeff; // [1] Will depend on which scheme that is used to advect total volume (currently UPWIND)
            pftype face_total_vol_fluxed = face_total_vol_coeff * node->v.vel_out * node->v.cf_area * dt; // [m^3]
            v += face_total_vol_fluxed/cell->get_total_fluid_volume();
        }
    }
#if COURANT_NUMBER_LIMITATION
    if (v > max_v) {
        max_v = v;
    }
#endif
#if  DEBUG
    if (v > 1) {
        std::cerr << "Warning! Courant number > 1 ( = " << v << endl;
        if (v > MAX_ALLOWED_V) {
            throw logic_error("Courant number larger than MAX_ALLOWED_V");
        }
    }
#endif
    if (guessed_total_in_volume_flux) {
        average_donor_neighbor_alpha = guessed_water_in_volume_flux/guessed_total_in_volume_flux;
    }
    else {
        average_donor_neighbor_alpha = cell_alpha;
    }
    //v = guessed_water_in_volume_flux * dt / cell->get_cube_volume();

    /* Set out volume coefficients to acceptor neighbors */
    cell->add_leaf_neighbor_lists_to_list_set(lists);
    for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
        if (node->v.vel_out > 0) {
            pftype acceptor_neighbor_alpha = node->v.n->total_vol_coeff ? node->v.n->get_alpha() : pftype(0); // [1]
#if  ALPHA_ADVECTION_SCHEME == HIGH_CONTRAST_SCHEME
            if (acceptor_neighbor_alpha > cell_alpha) {
                acceptor_neighbor_alpha = 1;
            }
            else if (acceptor_neighbor_alpha < cell_alpha) {
                acceptor_neighbor_alpha = 0;
            }
#endif
            pftype face_alpha;
            pftype face_total_vol_coeff = cell->total_vol_coeff; // [1]
            if (acceptor_neighbor_alpha == average_donor_neighbor_alpha) {
#if  ALPHA_ADVECTION_SCHEME == HRIC || ALPHA_ADVECTION_SCHEME == HYPER_C || ALPHA_ADVECTION_SCHEME == HIGH_CONTRAST_SCHEME
                face_alpha = cell_alpha;
#endif
            }
            else {
                pftype cell_normalized_variable =
                        (cell_alpha - average_donor_neighbor_alpha) /
                        (acceptor_neighbor_alpha - average_donor_neighbor_alpha); // [1]
                pftype face_normalized_variable; // [1];
                if (cell_normalized_variable > 1 || cell_normalized_variable < 0) {
                    /* Outer domain */
                    face_normalized_variable = cell_normalized_variable;
                }
#if  ALPHA_ADVECTION_SCHEME == HRIC
                else if (cell_normalized_variable < 0.5) {
                    /* Inner domain */
                    face_normalized_variable = 2 * cell_normalized_variable;
                }
#elif  ALPHA_ADVECTION_SCHEME == HYPER_C || ALPHA_ADVECTION_SCHEME == HIGH_CONTRAST_SCHEME
                else if (cell_normalized_variable < v) {
                    /* Inner domain */
                    face_normalized_variable = cell_normalized_variable / v;
                }
#endif
                else {
                    face_normalized_variable = 1;
                }

                if (face_normalized_variable == 1) {
                    face_alpha = acceptor_neighbor_alpha;
                }
                else if (face_normalized_variable == cell_normalized_variable) {
                    face_alpha = cell_alpha;
                }
                else {
                    face_alpha = average_donor_neighbor_alpha + face_normalized_variable * (acceptor_neighbor_alpha - average_donor_neighbor_alpha); // [1]
                }
#if  DEBUG
                if (average_donor_neighbor_alpha < 0 || average_donor_neighbor_alpha > 1) {
                    cout << "average_donor_neighbor_alpha: " << average_donor_neighbor_alpha << endl;
                    throw logic_error("Strange value for average_donor_neighbor_alpha");
                }
                if (acceptor_neighbor_alpha < 0 || acceptor_neighbor_alpha > 1) {
                    cout << "acceptor_neighbor_alpha: " << acceptor_neighbor_alpha << endl;
                    throw logic_error("Strange value for acceptor_neighbor_alpha");
                }
                if (cell_alpha < 0 || cell_alpha > 1) {
                    cout << "cell_alpha: " << cell_alpha << endl;
                    throw logic_error("Strange value for cell_alpha");
                }
                if (face_alpha < 0 || face_alpha > 1) {
                    cout << "face_alpha: " << face_alpha << endl;
                    throw logic_error("Strange value for face_alpha");
                }
#endif
            }
            pftype face_water_vol_coeff = face_alpha * face_total_vol_coeff; // [1]
            node->v.set_volume_coefficients(face_water_vol_coeff, face_total_vol_coeff);
        }
    }
#else
    "Don't know how to advect water"
#endif // ALPHA_ADVECTION_SCHEME

    //TODO: Calculate cell-face quasi-momentum scalars

    /* Calculate cell-face quasi-momentum vectors using the UPWIND scheme */
    cell->add_leaf_neighbor_lists_to_list_set(lists);
    for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
        if (node->v.vel_out >= 0) {
            node->v.quasi_momentum_vector = cell->ccv * node->v.get_cell_face_density();
        }
    }
}

void watersystem::advect_cell_properties_recursivelly(octcell* cell)
{
    DECLARE_RECURSIVE_LEAF_CELL_FUNCTION(advect_cell_properties_recursivelly, cell);

    /*
     * Update volume coefficients
     * Calculate in quasi-momentum flux
     */
    pftype in_water_vol_flux = 0; // [m^3/s]
    pftype in_total_vol_flux = 0; // [m^3/s]
    pfvec  in_momentum_flux; // [kg*m/s^2] in momentum flux
    pfvec  total_cell_face_area_velocity; // [m^3/s]

    /* Loop through neighbors */
    nlset lists;
    cell->add_leaf_neighbor_lists_to_list_set(lists);
    for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
        pftype volume_flux_out = node->v.vel_out * node->v.cf_area; // [m^3/s]
        in_water_vol_flux -= node->v.water_vol_coeff * volume_flux_out;
        in_total_vol_flux -= node->v.total_vol_coeff * volume_flux_out;
        in_momentum_flux -= node->v.quasi_momentum_vector * volume_flux_out;
        total_cell_face_area_velocity[node->v.dim] += node->v.get_vel_in_pos_dir() * node->v.cf_area;
    }

    pftype volume_flux_to_volume_coefficient_factor = dt/cell->get_cube_volume(); /* [s/m^3] */
    pftype d_water_vol_coeff = in_water_vol_flux * volume_flux_to_volume_coefficient_factor; /* [1] */
    pftype d_total_vol_coeff = in_total_vol_flux * volume_flux_to_volume_coefficient_factor; /* [1] */
    pftype d_density = physics::vol_coeffs_to_density(d_water_vol_coeff, d_total_vol_coeff); /* [kg/m^3] */
    pfvec net_momentum_in_flow = dt * in_momentum_flux; // [kg*m/s] Net in momentum
    cell->momentum_to_distribute = net_momentum_in_flow - d_density * total_cell_face_area_velocity * (0.5 * cell->s);

    const pftype LIMIT = 4.0e-16 * (USE_DOUBLE_PRECISION_FOR_PHYSICS ? 1 : 1 << (52 - 23));
    bool okay_to_decrease_water = false;
    bool okay_to_increase_water = false;
    bool no_fluid_left = false;
    if (cell->total_vol_coeff + d_total_vol_coeff < 0 &&
            cell->total_vol_coeff + d_total_vol_coeff > -LIMIT) {
        no_fluid_left = true;
    }
    if (((cell->water_vol_coeff + d_water_vol_coeff) > (cell->total_vol_coeff + d_total_vol_coeff) &&
         (cell->water_vol_coeff + d_water_vol_coeff) - (cell->total_vol_coeff + d_total_vol_coeff) < LIMIT) ||
            ((cell->water_vol_coeff + d_water_vol_coeff) < (cell->total_vol_coeff + d_total_vol_coeff) &&
             (cell->water_vol_coeff + d_water_vol_coeff) - (cell->total_vol_coeff + d_total_vol_coeff) > -LIMIT)) {
        okay_to_decrease_water = true;
    }
    if ((cell->water_vol_coeff + d_water_vol_coeff < 0 &&
         cell->water_vol_coeff + d_water_vol_coeff > -LIMIT) ||
            (cell->water_vol_coeff + d_water_vol_coeff > 0 &&
             cell->water_vol_coeff + d_water_vol_coeff < LIMIT)) {
        okay_to_increase_water = true;
    }
#if  DEBUG
    if (cell->total_vol_coeff + d_total_vol_coeff < 0 &&
            !no_fluid_left) {
        cout << endl;
        cout << "Advection: Old water volume coefficient: " << cell->water_vol_coeff << endl;
        cout << "Advection: Old total volume coefficient: " << cell->total_vol_coeff << endl;
        cout << "Advection: Additional water volume coefficient: " << d_water_vol_coeff << endl;
        cout << "Advection: Additional total volume coefficient: " << d_total_vol_coeff << endl;
        cout << "Advectoin: New water volume coefficient: " << cell->water_vol_coeff + d_water_vol_coeff << endl;
        cout << "Advection: New total volume coefficient: " << cell->total_vol_coeff + d_total_vol_coeff << endl;
        cout << (okay_to_decrease_water ? "Okay" : "Not okay") << " to decrease water" << endl;
        cout << (okay_to_increase_water ? "Okay" : "Not okay") << " to increase water" << endl;
        throw logic_error("New total volume coefficient less than zero");
    }
    if (cell->water_vol_coeff + d_water_vol_coeff > cell->total_vol_coeff + d_total_vol_coeff &&
            !okay_to_decrease_water) {
        cout << endl;
        cout << "Advection: Old water volume coefficient: " << cell->water_vol_coeff << endl;
        cout << "Advection: Old total volume coefficient: " << cell->total_vol_coeff << endl;
        cout << "Advection: Additional water volume coefficient: " << d_water_vol_coeff << endl;
        cout << "Advection: Additional total volume coefficient: " << d_total_vol_coeff << endl;
        cout << "Advectoin: New water volume coefficient: " << cell->water_vol_coeff + d_water_vol_coeff << endl;
        cout << "Advection: New total volume coefficient: " << cell->total_vol_coeff + d_total_vol_coeff << endl;
        cout << "Old water volume coefficient is " << cell->total_vol_coeff - cell->water_vol_coeff <<
                " less than old total volume coefficient" << endl;
        cout << "New water volume coefficient is " << (cell->water_vol_coeff + d_water_vol_coeff)-(cell->total_vol_coeff + d_total_vol_coeff) <<
                " more than new total volume coefficient" << endl;
        throw logic_error("New water volume coefficient more than new total volume coefficient in cell");
    }
    if (cell->water_vol_coeff + d_water_vol_coeff < 0 &&
            !okay_to_increase_water) {
        cout << endl;
        cout << "Advection: Old water volume coefficient: " << cell->water_vol_coeff << endl;
        cout << "Advection: Old total volume coefficient: " << cell->total_vol_coeff << endl;
        cout << "Advection: Additional water volume coefficient: " << d_water_vol_coeff << endl;
        cout << "Advection: Additional total volume coefficient: " << d_total_vol_coeff << endl;
        cout << "Advectoin: New water volume coefficient: " << cell->water_vol_coeff + d_water_vol_coeff << endl;
        cout << "Advection: New total volume coefficient: " << cell->total_vol_coeff + d_total_vol_coeff << endl;
        throw logic_error("New water volume coefficient less than zero");
    }
#endif
    if (!cell->water_vol_coeff && d_water_vol_coeff) {
        cell->prepare_for_water();
    }

#if  NO_ATMOSPHERE
#define TRY_TO_MAINTAIN_FULL_AIR_CELLS  0 // Experimental
#else
#define TRY_TO_MAINTAIN_FULL_AIR_CELLS  0 // Don't change this value
#endif
    if (no_fluid_left) {
#if TRY_TO_MAINTAIN_FULL_AIR_CELLS
        cell->set_volume_coefficients(0,
                                      1);
#else
        cell->set_volume_coefficients(0,
                                      0);
#endif
    }
    else if (okay_to_decrease_water) {
        cell->set_volume_coefficients(cell->total_vol_coeff + d_total_vol_coeff,
                                      cell->total_vol_coeff + d_total_vol_coeff);
    }
    else if (okay_to_increase_water) {
#if TRY_TO_MAINTAIN_FULL_AIR_CELLS
        cell->set_volume_coefficients(0,
                                      1);
#else
        cell->set_volume_coefficients(0,
                                      cell->total_vol_coeff + d_total_vol_coeff);
#endif
    }
    else {
#if  TRY_TO_MAINTAIN_FULL_AIR_CELLS
        if ((cell->total_vol_coeff + d_total_vol_coeff) - (cell->water_vol_coeff + d_water_vol_coeff) > 0.2) {
            cell->set_volume_coefficients(cell->water_vol_coeff + d_water_vol_coeff,
                                          1);
        }
        else {
            cell->set_volume_coefficients(cell->water_vol_coeff + d_water_vol_coeff,
                                          cell->total_vol_coeff + d_total_vol_coeff);
        }
#else
        cell->set_volume_coefficients(cell->water_vol_coeff + d_water_vol_coeff,
                                      cell->total_vol_coeff + d_total_vol_coeff);
#endif
    }

#if  DEBUG
    if (cell->water_vol_coeff < 0) {
        throw logic_error("Water volume coefficient became negative");
    }
    if (cell->total_vol_coeff < 0) {
        throw logic_error("Total volume coefficient became negative");
    }
    if (cell->water_vol_coeff > cell->total_vol_coeff) {
        throw logic_error("Water volume coefficient became larger than total volume coefficient");
    }
#endif
}

#if 0
void watersystem::convert_cell_face_vel_out_to_quasi_momentum_out_recursively(octcell* cell)
{
    DECLARE_RECURSIVE_LEAF_CELL_FUNCTION(convert_cell_face_vel_out_to_quasi_momentum_out_recursively, cell);

    /* Loop through neighbors */
    nlset lists;
    cell->add_leaf_neighbor_lists_to_list_set(lists);
    for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
        node->v.quasi_momentum_out = node->v.vel_out * node->v.get_average_cell_density();
    }
}

void watersystem::convert_cell_face_quasi_momentum_out_to_vel_out_recursively(octcell* cell)
{
    DECLARE_RECURSIVE_LEAF_CELL_FUNCTION(convert_cell_face_quasi_momentum_out_to_vel_out_recursively, cell);

    /* Loop through neighbors */
    nlset lists;
    cell->add_leaf_neighbor_lists_to_list_set(lists);
    for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
        if (node->v.quasi_momentum_out) {
            node->v.vel_out = node->v.quasi_momentum_out / node->v.get_average_cell_density();
#if  DEBUG
            if (!node->v.get_average_cell_density()) {
                throw logic_error("Cell face has quasi momentum out but no density");
            }
#endif
        }
    }
}
#endif

void watersystem::distribute_ceLl_quasi_momentum_on_cell_faces_recursively(octcell* cell)
{
    DECLARE_RECURSIVE_LEAF_CELL_FUNCTION(distribute_ceLl_quasi_momentum_on_cell_faces_recursively, cell);

    /* Measure areas */
    pfvec areas;
    nlset lists;
    cell->add_leaf_neighbor_lists_to_list_set(lists);
    for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
        areas[node->v.dim] += node->v.cf_area;
    }
    /* Distribute quasi momentum */
    cell->add_leaf_neighbor_lists_to_list_set(lists);
    for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
        node->v.vel_out += node->v.get_signed_dir() *
                cell->momentum_to_distribute[node->v.dim]/(node->v.get_associated_mass_per_unit_area() * areas[node->v.dim]);
    }
}

void watersystem::update_velocities_by_the_pressure_gradients_recursively(octcell* cell)
{
    DECLARE_RECURSIVE_LEAF_CELL_FUNCTION(update_velocities_by_the_pressure_gradients_recursively, cell);

    /* Cell is a leaf cell */
    /* Loop through neighbors */
    nlset lists;
    cell->add_leaf_neighbor_lists_to_list_set(lists);
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
