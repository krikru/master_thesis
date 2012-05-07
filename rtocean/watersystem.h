#ifndef WATERSYSTEM_H
#define WATERSYSTEM_H

////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

// Own includes
#include "fvoctree.h"
#include "callback.h"

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
    bool      water_defined();
    void      define_water(fvoctree* water, bool time_staggered = false, pftype time_step = 0);
    void      redefine_water(fvoctree* water, bool time_staggered = false, pftype time_step = 0);
    void      undefine_water();
    fvoctree* get_water();
    /* Time */
    pftype    get_time();
    void      set_time(pftype time);
    pftype    get_time_step();
    void      set_time_step(pftype time_step);
    /* Control */
    void      evolve();
    int       run_simulation(pftype time_step);
    bool      is_running();
    bool      is_operating();
    void      pause_simulation();
    void      abort_ongoing_operation();
    /* Callback */
    void set_state_updated_callback(callback<void (*)(void*)> callback);
    void set_state_updated_callback(void (*function)(void*), void* parameter);

private:
    /* Private member variables */

    /* Simulation */
    fvoctree* w; // Water
    pftype    t; // Time
    pftype   dt; // Time step

    /* Control */
    bool      running; // If the simulation is running or not
    bool      operating; // If some operation is already using some of the class members
    bool      pause; // If the simulation should stop or not
    bool      abort; // If the main loop should quit or not

    /* Callback */
    //callback<void (*)(void*, string)> output_callback;
    callback<void (*)(void*)> state_updated_callback;

private:
    /* Private methods */

    /* Control */
    void _evolve();

    /* Simulation */
    void advect_and_update_pressure();
    void calculate_cell_face_properties_recursivelly(octcell* cell);
    void calculate_delta_alpha_recursively(octcell* cell);
    void clamp_advect_alpha_recursively(octcell* cell);
    void calculate_alpha_gradient_recursively(octcell* cell);
    void advect_cell_properties_recursivelly(octcell* cell);
    //bool advect_and_update_pressure_recursively(octcell* cell);
    void update_velocities_recursively(octcell* cell);

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
bool watersystem::water_defined()
{
    return w;
}

inline
void watersystem::define_water(fvoctree* water, bool time_staggered, pftype time_step)
{
#if  DEBUG
    if (!water) {
        throw logic_error("Trying to define water using a NULL pointer");
    }
    if (w) {
        throw logic_error("Trying to define water when it is already defined");
    }
#endif

    if (time_staggered) {
        dt = time_step;
    }
    else {
        dt = 0;
        set_time_step(time_step);
    }
    w = water;
}

inline
void watersystem::redefine_water(fvoctree* water, bool time_staggered, pftype time_step)
{
    undefine_water();
    define_water(water, time_staggered, time_step);
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
fvoctree* watersystem::get_water()
{
    return w;
}


inline
pftype watersystem::get_time()
{
    return t;
}

inline
void watersystem::set_time(pftype time)
{
    t = time;
}

inline
pftype watersystem::get_time_step()
{
    return dt;
}

inline
void watersystem::set_time_step(pftype time_step)
{
    if (dt != time_step) {
        // TODO: Update time-staggered parameters (velocities)
        dt = time_step;
    }
}

inline
bool watersystem::is_running()
{
    return running;
}

inline
bool watersystem::is_operating()
{
    return operating;
}

inline
void watersystem::pause_simulation()
{
#if  DEBUG
    if (!running) {
        throw logic_error("Trying to pause simulation while not running any");
    }
#endif
    pause = true;
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
