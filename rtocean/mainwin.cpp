////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

// Qt includes
#include <QMessageBox>
#include <QCloseEvent>
#include <QTimer>

// Widgets
#include "mainwin.h"
#include "ui_mainwin.h"

// Own includes
#include "message_handler.h"
#include "base_float_vec2.h"

////////////////////////////////////////////////////////////////
// CONSTRUCTORS AND DESTRUCTOR
////////////////////////////////////////////////////////////////

mainwin::mainwin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mainwin)
{
    /* Initialize program parameters */
    initialize_scalar_propery_names();

    // Set up user interface
    ui->setupUi(this);

    // Set up main window
    this->show();
    // Set central widget

    QObject::connect(ui->actionPause, SIGNAL(triggered(bool)),
                     this, SLOT(toggle_pause_simulation()));

    // Start simulation directly when application has finished loading
    QTimer::singleShot(100, this, SLOT(start_simulation()));
}

mainwin::~mainwin()
{
    try
    {
        delete ui;
    }
    catch (std::exception &e) {
        message_handler::inform_about_exception("mainwin::~mainwin()", e, true);
    }
}

////////////////////////////////////////////////////////////////
// PRIVATE SLOTS
////////////////////////////////////////////////////////////////

void mainwin::closeEvent(QCloseEvent *event)
{
    try
    {
        if (system.is_operating()) {
#if 0
            // Ask the user whether to quit the application or not
            QMessageBox msg_box;
            msg_box.setText("Operation in process");
            msg_box.setInformativeText("An operation is currently performed by the simulation. Do you want to abort the operation?");
            msg_box.setStandardButtons(QMessageBox::Abort | QMessageBox::Cancel);
            msg_box.setDefaultButton(QMessageBox::Cancel);
            int result = msg_box.exec();
            if (result != QMessageBox::Abort) {
                event->ignore();
                return;
            }
#endif
            system.abort_ongoing_operation();
            ui->statusBar->showMessage("Aborting operation...");
        }
    }
    catch (std::exception &e) {
        message_handler::inform_about_exception("mainwin::closeEvent()", e, true);
    }
}

void mainwin::keyPressEvent(QKeyEvent *e)
{
    try
    {
        if (e->key() >= Qt::Key_0 && e->key() <= Qt::Key_9) {
            int scalar_property_number = e->key() - Qt::Key_0;
            if (scalar_property_number >= NUM_SCALAR_PROPERTIES) {
                scalar_property_number = 0;
            }
            ui->visualization_vw->set_scalar_property_to_visualize(scalar_property_number);
            ui->statusBar->showMessage(QString("Visualizing ") + scalar_propery_names[scalar_property_number]);
            //ui->statusBar->showMessage(scalar_propery_names[scalar_property_number]);
            //ui->statusBar->showMessage("Pressure deviation");
            //cout << "Address to statusBar: " << ui->statusBar << endl;
            //exit(0);
            return;
        }

        /* No action is implicitly defined for this key, pass the event to QMainWindow */
        QMainWindow::keyPressEvent(e);
    }
    catch (std::exception &e) {
        message_handler::inform_about_exception("mainwin::keyPressEvent()", e, true);
    }
}

void mainwin::start_simulation()
{
    try
    {
        system.define_water(new fvoctree(0, 0));
        system.set_state_updated_callback(do_events, this);
        system.set_number_of_time_steps_before_resting(NUM_TIME_STEPS_PER_FRAME);
        ui->visualization_vw->set_system_to_visualize(&system);
        ui->visualization_vw->set_scalar_property_to_visualize(DEFAULT_SCALAR_PROPERTY_TO_VISUALIZE);
        run_simulation();
    }
    catch (std::exception &e) {
        message_handler::inform_about_exception("mainwin::start_simulation()", e, true);
    }
}

////////////////////////////////////////////////////////////////
// PRIVATE METHODS
////////////////////////////////////////////////////////////////

void mainwin::initialize_scalar_propery_names()
{
#if  DEBUG
    for (uint i = 0; i < NUM_SCALAR_PROPERTIES; i++) {
        scalar_propery_names[i] = 0;
    }
#endif

    scalar_propery_names[SP_NO_SCALAR_PROPERTY      ] = "no scalar property"      ;
    scalar_propery_names[SP_ALPHA                   ] = "alpha"                   ;
    scalar_propery_names[SP_WATER_VOLUME_COEFFICIENT] = "water volume coefficient";
    scalar_propery_names[SP_AIR_VOLUME_COEFFICIENT  ] = "air volume coefficient"  ;
    scalar_propery_names[SP_TOTAL_VOLUME_COEFFICIENT] = "total volume coefficient";
    scalar_propery_names[SP_PRESSURE                ] = "pressure"                ;
    scalar_propery_names[SP_PRESSURE_DEVIATION      ] = "pressure deviation"      ;
    scalar_propery_names[SP_VELOCITY_DIVERGENCE     ] = "velocity divergence"     ;
    scalar_propery_names[SP_FLOW_DIVERGENCE         ] = "flow divergence"         ;

#if  DEBUG
    for (uint i = 0; i < NUM_SCALAR_PROPERTIES; i++) {
        if (!scalar_propery_names[i]) {
            throw logic_error("Not all scalar property names have been initialized");
        }
    }
#endif
}

void mainwin::run_simulation()
{
    ui->statusBar->showMessage("Simulating");
    int result = system.run_simulation(SIMULATION_TIME_STEP);
    switch (result) {
    case (SR_FINISHED):
        ui->statusBar->showMessage("Simulation finished.");
        break;
    case (SR_PAUSED):
        ui->statusBar->showMessage("Paused");
        break;
    case (SR_ABORTED):
        ui->statusBar->showMessage("Simulation aborted.");
        break;
    default:
        throw logic_error("Unknown simulation result");
    }
}

/* Returns true if the simulation was pauseed, false otherwise */
bool mainwin::toggle_pause_simulation()
{
#define  BREAK_MAIN_LOOP_WHEN_PAUSING  1
    try
    {
        if (system.is_water_defined()) {
            if (system.is_paused()) {
#if  BREAK_MAIN_LOOP_WHEN_PAUSING
                run_simulation();
#else
                ui->statusBar->showMessage("Simulating");
                system.continue_simulation();
#endif
                return false;
            }
            else {
#if  BREAK_MAIN_LOOP_WHEN_PAUSING
                system.pause_simulation(true);
#else
                ui->statusBar->showMessage("Paused");
                system.pause_simulation(false);
#endif
                return true;
            }
        }
    }
    catch (std::exception &e) {
        message_handler::inform_about_exception("mainwin::start_simulation()", e, true);
    }
#if  DEBUG
    LINE_UNREACHABLE();
#endif
    return false; // Only to prevent warning
}

void mainwin::on_actionAbout_rtocean_triggered()
{
    try
    {
    }
    catch (std::exception &e) {
        message_handler::inform_about_exception("mainwin::on_actionAbout_rtocean_triggered()", e, true);
    }
}

void mainwin::do_events()
{
    static bool has_been_called_before = false;
    static pftype time_starting_to_evolve_system = clock();
    pftype time_starting_to_render = clock();
    if (has_been_called_before) {
        /* Finished evolving system*/
        cout << "Took " << (time_starting_to_render - time_starting_to_evolve_system)/1000.0 << " seconds." << endl << endl;
    }
    else {
        has_been_called_before = true;
    }

    /* Visualize */
    cout << "Visualizing system..." << endl;
    ui->visualization_vw->updateGL();
    cout << "Took " << (clock() - time_starting_to_render)/1000.0 << " seconds." << endl << endl;

    /* Let Qt do its stuff */
    QApplication::processEvents(QEventLoop::AllEvents);

    /* Wait until frame has finished */
    pftype time_loop_finished;
    do {
        time_loop_finished = clock();
    } while (time_loop_finished - time_starting_to_evolve_system < FRAME_MS);

    /* Evolve system again (it will evolve when this function returns) */
    time_starting_to_evolve_system = time_loop_finished;
    cout << "Evolving system..." << endl;
}

void mainwin::do_events(void* mainwin_object)
{
#if  DEBUG
    if (!mainwin_object) {
        throw logic_error("Callback function was called with a NULL pointer");
    }
#endif
    static_cast<mainwin*>(mainwin_object)->do_events();
}
