#include "mainwin.h"
#include "ui_mainwin.h"

mainwin::mainwin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mainwin)
{
    ui->setupUi(this);
}

mainwin::~mainwin()
{
    delete ui;
}
