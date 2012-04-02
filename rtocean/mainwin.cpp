#include "mainwin.h"
#include "ui_mainwin.h"

mainwin::mainwin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mainwin)
{
    ui->setupUi(this);


    // Start simulation directly when application has finished loading
    QTimer::singleShot(0, this, SLOT(start_simulation()));
}

mainwin::~mainwin()
{
    delete ui;
}
