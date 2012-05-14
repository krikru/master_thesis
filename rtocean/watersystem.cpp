////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

// Own includes
#include "watersystem.h"

////////////////////////////////////////////////////////////////
// MACROS
////////////////////////////////////////////////////////////////

#if 0
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
#endif

////////////////////////////////////////////////////////////////
// CONSTRUCTORS AND DESTRUCTOR
////////////////////////////////////////////////////////////////

watersystem::watersystem()
{
    w = 0;
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
    pause = false;
    while (!abort && !pause) {
        /* Evolve the system */
        _evolve();
        /* Process everything that needs to be processed */
        process_events();
    }

    if (abort) {
        ret = SR_ABORTED;
    }
    else if (pause) {
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
    t += dt;
    advect_and_update_pressure();
    update_velocities_recursively(w->root);
}

#if 1
void watersystem::advect_and_update_pressure()
{
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
    if (cell->has_child_array()) {
        for (uint idx = 0; idx < octcell::MAX_NUM_CHILDREN; idx++) {
            octcell* c = cell->get_child(idx);
            if (c) {
                /* Child exists */
                calculate_cell_face_properties_recursivelly(c);
            }
        }
        return;
    }

    /* Loop through neighbors */
    nlset lists;
    lists.add_neighbor_list(&cell->neighbor_lists[NL_HIGHER_LEVEL_OF_DETAIL]);
    lists.add_neighbor_list(&cell->neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_LEAF]);
    lists.add_neighbor_list(&cell->neighbor_lists[NL_LOWER_LEVEL_OF_DETAIL_LEAF]);
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
    pftype cell_alpha = cell->total_vol_coeff ? cell->get_alpha() : 0; // [1]
    pftype guessed_water_in_volume_flux = 0; // [m^3/s]
    pftype guessed_total_in_volume_flux = 0; // [m^3/s]
    for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
        if (node->v.vel_out < 0) {
            guessed_water_in_volume_flux -= node->v.n->water_vol_coeff * node->v.vel_out * node->v.cf_area;
            guessed_total_in_volume_flux -= node->v.n->total_vol_coeff * node->v.vel_out * node->v.cf_area;
        }
        else {
            pftype face_total_vol_coeff = cell->total_vol_coeff; // [1]
            pftype face_total_vol_fluxed = face_total_vol_coeff * node->v.vel_out * node->v.cf_area * dt; // [m^3]
            v += face_total_vol_fluxed/cell->get_total_fluid_volume();
        }
    }
    if (guessed_total_in_volume_flux) {
        average_donor_neighbor_alpha = guessed_water_in_volume_flux/guessed_total_in_volume_flux;
    }
    else {
        average_donor_neighbor_alpha = cell_alpha;
    }
    //v = guessed_water_in_volume_flux * dt / cell->get_cube_volume();

    /* Set out volume coefficients to acceptor neighbors */
    lists.add_neighbor_list(&cell->neighbor_lists[NL_HIGHER_LEVEL_OF_DETAIL]);
    lists.add_neighbor_list(&cell->neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_LEAF]);
    lists.add_neighbor_list(&cell->neighbor_lists[NL_LOWER_LEVEL_OF_DETAIL_LEAF]);
    for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
        if (node->v.vel_out > 0) {
            pftype acceptor_neighbor_alpha = node->v.n->total_vol_coeff ? node->v.n->get_alpha() : 0; // [1]
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
#endif

}

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

    /* Update volume coefficients */
    pftype in_water_vol_flux = 0; // [m^3/s]
    pftype in_total_vol_flux = 0; // [m^3/s]
    /* Loop through neighbors */
    nlset lists;
    lists.add_neighbor_list(&cell->neighbor_lists[NL_HIGHER_LEVEL_OF_DETAIL]);
    lists.add_neighbor_list(&cell->neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_LEAF]);
    lists.add_neighbor_list(&cell->neighbor_lists[NL_LOWER_LEVEL_OF_DETAIL_LEAF]);
    for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
        pftype volume_flux_out = node->v.vel_out * node->v.cf_area; // [m^3/s]
        in_water_vol_flux -= node->v.water_vol_coeff * volume_flux_out;
        in_total_vol_flux -= node->v.total_vol_coeff * volume_flux_out;
    }

    pftype volume_flux_to_volume_coefficient_factor = dt/cell->get_cube_volume();
    pftype d_water_vol_coeff = in_water_vol_flux * volume_flux_to_volume_coefficient_factor;
    pftype d_total_vol_coeff = in_total_vol_flux * volume_flux_to_volume_coefficient_factor;

#if  USE_DOUBLE_PRECISION_FOR_PHYSICS
    const pftype LIMIT = 1.0e-16;
#else
    const pftype LIMIT = 0.536870912e-7; /* Based on the number above */
#endif
    bool okay_to_decrease_water = false;
    bool okay_to_increase_water = false;
    bool no_fluid_left = false;
    if (cell->total_vol_coeff + d_total_vol_coeff < 0 &&
            cell->total_vol_coeff + d_total_vol_coeff > -LIMIT) {
        no_fluid_left = true;
    }
    if ((cell->water_vol_coeff + d_water_vol_coeff > cell->total_vol_coeff + d_total_vol_coeff &&
         (cell->water_vol_coeff + d_water_vol_coeff) - (cell->total_vol_coeff + d_total_vol_coeff) < LIMIT) ||
            (cell->water_vol_coeff + d_water_vol_coeff < cell->total_vol_coeff + d_total_vol_coeff &&
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
    if (okay_to_decrease_water) {
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

    /* Update pressure */
#if  USE_ARTIFICIAL_COMPRESSIBILITY
#if  NO_ATMOSPHERE
#if  VACUUM_HAS_PRESSURE
    cell->p = (cell->total_vol_coeff - 1) * ARTIFICIAL_COMPRESSIBILITY_FACTOR;
#else
    cell->p = (cell->water_vol_coeff - 1) * ARTIFICIAL_COMPRESSIBILITY_FACTOR;
    if (cell->p < 0) {
#if  ALLOW_NEGATIVE_PRESSURES
#if INTERPOLATE_SURFACE_PRESSURE
        /* Loop through neighbors */
        nlset lists;
        lists.add_neighbor_list(&cell->neighbor_lists[NL_HIGHER_LEVEL_OF_DETAIL]);
        lists.add_neighbor_list(&cell->neighbor_lists[NL_SAME_LEVEL_OF_DETAIL_LEAF]);
        lists.add_neighbor_list(&cell->neighbor_lists[NL_LOWER_LEVEL_OF_DETAIL_LEAF]);
        for (nlnode* node = lists.get_first_node(); node; node = lists.get_next_node()) {
            pftype num_terms = 0;
            pftype average_pressure_below = 0;
            pftype average_distance_down = 0;
            if (node->v.dim == VERTICAL_DIMENSION && !node->v.pos_dir) {
                /* Have found a node beneath, get pressure */
                average_pressure_below += node->v.n->p;
                average_distance_down -= node->v.n->get_edge_length()/2;
                num_terms++;
            }
            pftype pressure_gradient;
            if (num_terms) {
                average_pressure_below /= num_terms;
                average_distance_down /= num_terms;
                average_distance_down += cell->get_alpha()*cell->get_edge_length();
                pressure_gradient = (average_pressure_below - NORMAL_AIR_PRESSURE)/average_distance_down;
            }
            else {
                pressure_gradient = P_G * P_WATER_DENSITY;
            }
            cell->p = NORMAL_AIR_PRESSURE + (cell->get_alpha()-0.5)*cell->get_edge_length()*pressure_gradient;
        }
#else
        // Prevent too low pressures at the boundary
        cell->p = (cell->total_vol_coeff - 1) * ARTIFICIAL_COMPRESSIBILITY_FACTOR;
        if (cell->p > 0) {
            cell->p = 0; // Vacuum partly fills the cell
        }
#endif
#else
        cell->p = 0;
#endif // ALLOW_NEGATIVE_PRESSURES
    }
#endif // VACUUM_HAS_PRESSURE
#else // NO_ATMOSPHERE
    if (cell->has_no_air()) {
        /* Cell consists only of water */
        cell->p = (cell->water_vol_coeff - 1) * ARTIFICIAL_COMPRESSIBILITY_FACTOR;
        if (cell->p < 0) {
#if  ALLOW_NEGATIVE_PRESSURES
            // Prevent too low pressures at the boundary
            cell->p = (cell->total_vol_coeff - 1) * ARTIFICIAL_COMPRESSIBILITY_FACTOR;
            if (cell->p > 0) {
                cell->p = 0; // Vacuum partly fills the cell
            }
#else
            cell->p = 0;
#endif // ALLOW_NEGATIVE_PRESSURES
        }
    }
    else if (cell->has_no_water()) {
        /* Cell consists only of air */
        cell->p = cell->total_vol_coeff * NORMAL_AIR_PRESSURE;
    }
    else {
        /* Calculate pressure for a mixed cell */
        // Optimize
        pftype k = ARTIFICIAL_COMPRESSIBILITY_FACTOR;
        pftype q = NORMAL_AIR_PRESSURE;
        pftype a = cell->get_air_volume_coefficient();
        pftype w = cell->water_vol_coeff;
        pftype d = q*a/k;
        //"Don't know if this formula is correct"
        cell->p = k/2*(sqrt(SQUARE(d + 1 - w) + 4*d*w) + d + w - 1);
#if  DEBUG
        if (IS_NAN(cell->p)) {
            throw logic_error("Pressure became NaN");
        }
#endif
    }
#endif //NO_ATMOSPHERE
#else  //USE_ARTIFICIAL_COMPRESSIBILITY
    "don't know what to do now"
#endif  //USE_ARTIFICIAL_COMPRESSIBILITY
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
