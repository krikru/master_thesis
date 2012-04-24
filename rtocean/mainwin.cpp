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
    // Set up user interface
    ui->setupUi(this);

    // Set up main window
    this->show();
    // Set central widget

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
            // Ask the user whether to quit the application or not
            QMessageBox msg_box;
            msg_box.setText("Question to user:");
            msg_box.setInformativeText("An operation is currently performed by the simulated system. Do you want to abort the operation and quit the application?");
            msg_box.setStandardButtons(QMessageBox::Abort | QMessageBox::Cancel);
            msg_box.setDefaultButton(QMessageBox::Cancel);
            int result = msg_box.exec();
            if (result != QMessageBox::Abort) {
                event->ignore();
                return;
            }
            else {
                system.abort_ongoing_operation();
                ui->statusBar->showMessage("Aborting operation...");
            }
        }
    }
    catch (std::exception &e) {
        message_handler::inform_about_exception("mainwin::closeEvent()", e, true);
    }
}

void mainwin::start_simulation()
{
    try
    {
        system.define_water(new fvoctree(0, 0));
        system.set_state_updated_callback(do_events, this);
        ui->visualization_vw->set_system_to_visualize(&system);

        /* Start simulation */
        last_time_letting_system_evolve = clock();
        ui->statusBar->showMessage("Starting simulation...");
        system.run_simulation(SIMULATION_TIME_STEP);
        ui->statusBar->showMessage("Simulation finished.");
    }
    catch (std::exception &e) {
        message_handler::inform_about_exception("mainwin::start_simulation()", e, true);
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
    base_float_vec2<float> time_per_frame();

    ui->visualization_vw->paintGL();
    QApplication::processEvents(QEventLoop::AllEvents);
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
