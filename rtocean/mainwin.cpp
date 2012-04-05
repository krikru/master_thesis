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
    // Set central widget

    // Start simulation directly when application has finished loading
    QTimer::singleShot(0, this, SLOT(start_simulation()));
}

mainwin::~mainwin()
{
    delete ui;
}

////////////////////////////////////////////////////////////////
// PRIVATE SLOTS
////////////////////////////////////////////////////////////////

void mainwin::closeEvent(QCloseEvent *event)
{
    return;

    // Ask the user whether to quit the application or not
    QMessageBox msg_box;
    msg_box.setText("Question to user:");
    msg_box.setInformativeText("Do you really want to quit the application?");
    msg_box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msg_box.setDefaultButton(QMessageBox::No);
    int result = msg_box.exec();
    if (result != QMessageBox::Yes) {
        event->ignore();
        return;
    }

    // TODO: abort activities if necessary
}

void mainwin::start_simulation()
{

}

void mainwin::on_actionAbout_rtocean_triggered()
{

}
