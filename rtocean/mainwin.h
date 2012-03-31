#ifndef MAINWIN_H
#define MAINWIN_H

#include <QMainWindow>

namespace Ui {
    class mainwin;
}

class mainwin : public QMainWindow
{
    Q_OBJECT

public:
    explicit mainwin(QWidget *parent = 0);
    ~mainwin();

private:
    Ui::mainwin *ui;
};

#endif // MAINWIN_H
