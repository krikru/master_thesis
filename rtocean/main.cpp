
////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

// Standard includes
#include <iostream>
using std::cout;
using std::endl;

// OpenGL
//#include <GL/glut.h>

// Qt
#include <QtGui/QApplication>

// Widgets
#include "mainwin.h"

// Own includes
#include "message_handler.h"

////////////////////////////////////////////////////////////////
// MAIN FUNCTION
////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    try
    {
        /* Init glut */
        //glutInit(&argc, argv);

        // Init application
        QApplication app(argc, argv);
        mainwin win;
        win.showMaximized();
        win.show();

        // "Start" application
        return app.exec();
    }
    catch (std::exception &e) {
        message_handler::inform_about_exception("main()", e, true);
    }

    // Will never reach this line
    return 1;
}
