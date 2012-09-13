////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

//#define WIN32_LEAN_AND_MEAN

/* Standard includes */
#include <typeinfo>

/* Qt includes */
#include <QDebug>
#include <QString>

/* Own includes */
#include "message_handler.h"

////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////

void message_handler::display_message_box(QString message, QString caption, QMessageBox::Icon icon)
{
    QMessageBox mbox;
    mbox.setText(message);
    mbox.setWindowTitle(caption);
    mbox.setIcon(icon);
    mbox.exec();
}

void message_handler::display_information_message(QString message, QString caption)
{
    display_message_box(message, caption, QMessageBox::Information);
}

void message_handler::display_warning_message(QString message, QString caption)
{
    display_message_box(message, caption, QMessageBox::Warning);
}

void message_handler::display_error_message(QString message, QString caption)
{
    display_message_box(message, caption, QMessageBox::Critical);
}

void message_handler::inform_about_exception(QString caller, std::exception &error, bool terminate)
{
    qCritical() << "Unexpected error occured in" << caller;
    qCritical() << "\tCaught:" << error.what();
    qCritical() << "\tType:" << typeid(error).name();

    //std::cerr << "Unexpected error occured in " << caller.toStdString() << std::endl;
    //std::cerr << "\tCaught: " << error.what() << std::endl;
    //std::cerr << "\tType: " << typeid(error).name() << std::endl;
#if  0
    display_error_message("Unexpected error occured in " + caller + "\nType: " + typeid(error).name() + "\n\n\"" + error.what() + "\"");
#endif
    if (terminate) exit(1);
}

