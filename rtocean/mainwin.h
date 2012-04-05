#ifndef MAINWIN_H
#define MAINWIN_H

////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

// Standard includes

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

    // Create a slot for starting the simulation
    void start_simulation();
    
    void on_actionAbout_rtocean_triggered();

private:
    Ui::mainwin *ui;
};

#endif // MAINWIN_H
