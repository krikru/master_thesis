#ifndef WATERSYSTEM_H
#define WATERSYSTEM_H

////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

// Own includes
#include "fvoctree.h"
#include "callback.h"

////////////////////////////////////////////////////////////////
// ENUMS
////////////////////////////////////////////////////////////////

enum SIMULATION_RESULT {
    SR_FINISHED,
    SR_PAUSED,
    SR_ABORTED,
    NUM_SIMULATION_RESULTS
};

////////////////////////////////////////////////////////////////
// CLASS DEFINITION
////////////////////////////////////////////////////////////////

class watersystem
{
public:
    /* Constructors and destructor */
    watersystem();

public:
    /* Water */
    bool      is_water_defined() const;
    void      define_water(fvoctree* water, pftype start_time = 0, bool time_staggered = false, pftype time_step = 0);
    void      redefine_water(fvoctree* water, pftype start_time = 0, bool time_staggered = false, pftype time_step = 0);
    void      undefine_water();
    const fvoctree* get_water() const;
    /* Time */
    pftype    get_time() const;
    void      set_time(pftype time);
    pftype    get_time_step() const;
    void      set_time_step(pftype time_step);
    void      set_number_of_time_steps_before_resting(uint number_of_time_steps);
    /* Control */
    void      evolve();
    int       run_simulation(pftype time_step);
    bool      is_started() const;
    bool      is_paused() const;
    bool      is_operating() const;
    void      pause_simulation(bool jump_out_of_main_loop = true);
    void      continue_simulation();
    void      abort_ongoing_operation();
    /* Callback */
    void set_state_updated_callback(callback<void (*)(void*)> callback);
    void set_state_updated_callback(void (*function)(void*), void* parameter);

private:
    /* Private member variables */

    /* Simulation */
    fvoctree* w ; // Water
    pftype    t ; // Time
    pftype    dt; // Time step
    uint      num_time_steps_before_resting; // Ths number of time steps before calling the state_updated_callback function

    /* Control */
    bool      started; // If the simulation is running or not
    bool      operating; // If some operation is already using some of the class members
    bool      paused; // If the simulation should stop or not
    bool      break_main_loop_when_pausing; // If the main loop should break or not when paused
    bool      abort; // If the main loop should quit or not

    /* Callback */
    //callback<void (*)(void*, string)> output_callback;
    callback<void (*)(void*)> state_updated_callback;

private:
    /* Private methods */

    /* Control */
    void _evolve();

    /* Simulation */
    void transport_fluids_and_update_pressure();
    void calculate_cell_face_properties_recursivelly(octcell* cell);
    void calculate_delta_alpha_recursively(octcell* cell);
    void clamp_advect_alpha_recursively(octcell* cell);
    void calculate_alpha_gradient_recursively(octcell* cell);
    void advect_cell_properties_recursivelly(octcell* cell);
    //bool advect_and_update_pressure_recursively(octcell* cell);
    void update_velocities_by_the_pressure_gradients_recursively(octcell* cell);
    void update_velocities_by_advection();

    /* Thread safety */
    void start_operation();
    void finish_operation();

    /* Miscellaneous */
    void process_events();
};

////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////

inline
bool watersystem::is_water_defined() const
{
    return w;
}

inline
void watersystem::define_water(fvoctree* water, pftype start_time, bool time_staggered, pftype time_step)
{
#if  DEBUG
    if (!water) {
        throw logic_error("Trying to define water using a NULL pointer");
    }
    if (w) {
        throw logic_error("Trying to define water when it is already defined");
    }
#endif

    w = water;
    t = start_time;
    if (time_staggered) {
        dt = time_step;
    }
    else {
        dt = 0;
        set_time_step(time_step);
    }
    started = false;
    paused = false;
    abort = false;
}

inline
void watersystem::redefine_water(fvoctree* water, pftype start_time, bool time_staggered, pftype time_step)
{
    undefine_water();
    define_water(water, start_time, time_staggered, time_step);
}

inline
void watersystem::undefine_water()
{
#if  DEBUG
    if (!w) {
        throw logic_error("Trying to undefine water but it is not currently defined");
    }
#endif
    delete w;
    w = 0;
}

inline
const fvoctree *watersystem::get_water() const
{
    return w;
}

inline
pftype watersystem::get_time() const
{
#if  DEBUG
    if (!is_water_defined()) {
        throw logic_error("Trying to get time while no water is defined");
    }
#endif
    return t;
}

inline
void watersystem::set_time(pftype time)
{
#if  DEBUG
    if (!is_water_defined()) {
        throw logic_error("Trying to set time while no water is defined");
    }
#endif
    t = time;
}

inline
pftype watersystem::get_time_step() const
{
#if  DEBUG
    if (!is_water_defined()) {
        throw logic_error("Trying to get time step while no water is defined");
    }
#endif
    return dt;
}

inline
void watersystem::set_number_of_time_steps_before_resting(uint number_of_time_steps)
{
#if  DEBUG
    if (number_of_time_steps <= 0) {
        throw logic_error("Trying to set an invalid number of time steps before resting");
    }
#endif
    num_time_steps_before_resting = number_of_time_steps;
}

inline
void watersystem::set_time_step(pftype time_step)
{
#if  DEBUG
    if (!is_water_defined()) {
        throw logic_error("Trying to set time step while no water is defined");
    }
#endif
    if (dt != time_step) {
        // TODO: Update time-staggered parameters (velocities)
        dt = 0.5 * (time_step - dt);
        update_velocities_by_the_pressure_gradients_recursively(w->root);
        dt = time_step;
    }
}

inline
bool watersystem::is_started() const
{
#if  DEBUG
    if (!is_water_defined()) {
        throw logic_error("Asking if simulation is started while no water is defined");
    }
#endif
    return started;
}

inline
bool watersystem::is_paused() const
{
#if  DEBUG
    if (!is_water_defined()) {
        throw logic_error("Asking if simulation is paused while no water is defined");
    }
#endif
    return paused;
}

inline
bool watersystem::is_operating() const
{
    return operating;
}

inline
void watersystem::pause_simulation(bool jump_out_of_main_loop)
{
#if  DEBUG
    if (!is_water_defined()) {
        throw logic_error("Trying to pause simulation while no water is defined");
    }
    if (!started) {
        throw logic_error("Trying to pause simulation while not started any");
    }
    if (paused) {
        throw logic_error("Trying to pause simulation while already paused");
    }
#endif
    paused = true;
    break_main_loop_when_pausing = jump_out_of_main_loop;
}

inline
void watersystem::continue_simulation()
{
#if  DEBUG
    if (!is_water_defined()) {
        throw logic_error("Trying to continue simulation while no water is defined");
    }
    if (!started) {
        throw logic_error("Trying to continue simulation while not started any");
    }if (!paused) {
        throw logic_error("Trying to continue simulation while not paused");
    }
#endif
    if (break_main_loop_when_pausing) {
        paused = false;
    }
    else {
        run_simulation(dt);
    }
}

inline
void watersystem::abort_ongoing_operation()
{
#if  DEBUG
    if (!operating) {
        throw logic_error("Trying to abort operation while not doing any");
    }
#endif
    abort = true;
}

inline
void watersystem::set_state_updated_callback(callback<void (*)(void*)> callback)
{
    state_updated_callback = callback;
}

inline
void watersystem::set_state_updated_callback(void (*function)(void*), void* parameter)
{
    state_updated_callback.func  = function ;
    state_updated_callback.param = parameter;
}

/* Miscellaneous */

inline
void watersystem::process_events()
{
    // Let the application process its events
    if (state_updated_callback.func) {
        state_updated_callback.func(state_updated_callback.param);
    }
}

#endif // WATERSYSTEM_H
