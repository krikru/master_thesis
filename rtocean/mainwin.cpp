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

    // Set up user interface components
    ui->statusBar->setFont(QFont("Courier", 8));

    // Show ui
    this->show();

    /* Connect menu items with functions */
    QObject::connect(ui->actionPause, SIGNAL(triggered(bool)),
                     this, SLOT(toggle_pause_simulation()));
    QObject::connect(ui->actionSave_screen_as_TikZ_picture, SIGNAL(triggered(bool)),
                     this, SLOT(save_screen_as_tikz_picture()));

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
            /* Select scalar property to visualize */
            int scalar_property_number = e->key() - Qt::Key_0;
            if (scalar_property_number >= NUM_SCALAR_PROPERTIES) {
                scalar_property_number = 0;
            }
            ui->visualization_vw->set_scalar_property_to_visualize(scalar_property_number);
            ui->statusBar->showMessage(QString("Visualizing ") + scalar_propery_names[scalar_property_number]);
            ui->visualization_vw->updateGL();
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
        system.set_state_updated_callback(do_events_callback, this);
        system.set_take_printscreen_callback(take_printscreens_callback, this);
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
        scalar_propery_names[i] = "";
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
        if (scalar_propery_names[i].isEmpty()) {
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
        ui->statusBar->showMessage("Simulation finished at time = " + QString::number(system.get_time(), 'f', 6) + " s");
        break;
    case (SR_PAUSED):
        ui->statusBar->showMessage("Paused at time = " + QString::number(system.get_time(), 'f', 6) + " s");
        break;
    case (SR_ABORTED):
        ui->statusBar->showMessage("Simulation aborted at time = " + QString::number(system.get_time(), 'f', 6) + " s");
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
                ui->statusBar->showMessage("Simulating; time = " + QString::number(system.get_time(), 'f', 6) + " s");
                system.continue_simulation();
#endif
                return false;
            }
            else {
#if  BREAK_MAIN_LOOP_WHEN_PAUSING
                system.pause_simulation(true);
#else
                ui->statusBar->showMessage("Paused at time = " + QString::number(system.get_time(), 'f', 6) + " s");
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

void mainwin::save_screen_as_tikz_picture(QString file_name)
{
    try
    {
        cout << "Visualizing system..." << endl;
        ui->visualization_vw->save_screen_as_tikz_picture(file_name);
    }
    catch (std::exception &e) {
        message_handler::inform_about_exception("mainwin::save_screen_as_tikz_picture()", e, true);
    }
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
    ui->statusBar->showMessage("Simulating; time = " + QString::number(system.get_time(), 'f', 6) + " s");
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

void mainwin::take_printscreens()
{
    uint old_property = ui->visualization_vw->get_scalar_property_to_visualize();
    for (size_t i = 0; i < NUM_PRINTSCREEN_SCALAR_PROPERTIES; i++) {
        SCALAR_PROPERTY property = PRINTSCREEN_SCALAR_PROPERTIES[i];
        QString property_name = scalar_propery_names[property];
        QString file_name = "screenshot_" + property_name.replace(' ', '_') + "_at_" + QString::number(system.get_time()) + ".tex";
        ui->visualization_vw->set_scalar_property_to_visualize(property);
        save_screen_as_tikz_picture(file_name);
    }
    ui->visualization_vw->set_scalar_property_to_visualize(old_property);
}

void mainwin::do_events_callback(void* mainwin_object)
{
#if  DEBUG
    if (!mainwin_object) {
        throw logic_error("do_events callback function was called with a NULL pointer");
    }
#endif
    static_cast<mainwin*>(mainwin_object)->do_events();
}

void mainwin::take_printscreens_callback(void* mainwin_object)
{
#if  DEBUG
    if (!mainwin_object) {
        throw logic_error("take_printscreen_callback was called with a NULL pointer");
    }
#endif
    static_cast<mainwin*>(mainwin_object)->take_printscreens();
}
