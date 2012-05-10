#ifndef MAINWIN_H
#define MAINWIN_H

////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

// Standard includes
#include <time.h>

// Qt includes
#include <QMainWindow>

// Own widget includes
#include "viswidget.h"

////////////////////////////////////////////////////////////////
// CLASS DEFINITION
////////////////////////////////////////////////////////////////

namespace Ui {
class mainwin;
}

class mainwin : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit mainwin(QWidget *parent = 0);
    ~mainwin();

private slots:
    // Overload the close event slot
    void closeEvent(QCloseEvent *event);

    void start_simulation();
    void toggle_pause_simulation();

    // Slots connected to UI items
    void on_actionAbout_rtocean_triggered();

private:
    /* Private methods */
    void do_events();
    static void do_events(void* mainwin_object);

private:
    Ui::mainwin *ui;
    watersystem system;
    clock_t     last_time_letting_system_evolve;
};

#endif // MAINWIN_H
