#include <QtGui/QApplication>
#include "mainwin.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    mainwin w;
    w.show();

    return a.exec();
}
