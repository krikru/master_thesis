
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
        pftype a = 1;
        a = 2;
        a + 3;
        /* Init glut */
        //glutInit(&argc, argv);

        // Init application
        QApplication app(argc, argv);
        mainwin win;
        win.showMaximized();
        win.show();

#if  0
        {
            dllist<int> l;
            dllnode<int>* e1 = l.add_existing_element(1);
            dllnode<int>* e2 = l.add_new_element();
            e2->v = 2;
            dllnode<int>* e3 = l.add_existing_element(3);
            l.add_existing_element(4);
            l.add_existing_element(5);
            dllnode<int>* e6 = l.add_existing_element(6);
            e6->remove_from_list_and_delete();
            l.add_existing_element(7);
            dllnode<int>* e8 = l.add_existing_element(8);
            dllnode<int>* e9 = l.add_existing_element(9);
            dllnode<int>* e10 = l.add_existing_element(10);

            e2->remove_from_list_and_delete();
            e1->remove_from_list_and_delete();
            e3->remove_from_list_and_delete();
            e9->remove_from_list_and_delete();
            e10->remove_from_list_and_delete();
            e8->remove_from_list_and_delete();

            for (dllnode<int>* e = l.get_first_element(); e; e = e->get_next_node()) {
                cout << e->v << endl;
            }
        }
        return 0;
#endif

        // "Start" application
        return app.exec();
    }
    catch (std::exception &e) {
        message_handler::inform_about_exception("main()", e, true);
    }

    // Will never reach this line
    return 1;
}
