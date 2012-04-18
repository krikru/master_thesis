
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

        {
            dllist<int> l;
            dllnode<int>* e1 = l.add_existing_element(1);
            dllnode<int>* e2 = l.add_new_element();
            e2->v = 2;
            l.add_existing_element(3);
            l.add_existing_element(4);
            l.add_existing_element(5);
            l.add_existing_element(6);
            l.add_existing_element(7);
            l.add_existing_element(8);
            l.add_existing_element(9);
            l.add_existing_element(10);

            for (dllnode<int>* e = l.get_first_element(); e; e = e->n) {
                cout << e->v << endl;
            }
        }
        return 0;

        // "Start" application
        return app.exec();
    }
    catch (std::exception &e) {
        message_handler::inform_about_exception("main()", e, true);
    }

    // Will never reach this line
    return 1;
}
