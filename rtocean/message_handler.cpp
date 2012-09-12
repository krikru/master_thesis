////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

//#define WIN32_LEAN_AND_MEAN

/* Standard includes */
#include <iostream>
//#include <windows.h>
#include <typeinfo>

/* Own includes */
#include "message_handler.h"

////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////

void message_handler::display_message_box(std::string message, std::string caption, QMessageBox::Icon icon)
{
    QMessageBox mbox;
    mbox.setText(QString::fromStdString(message));
    mbox.setWindowTitle(QString::fromStdString(caption));
    mbox.setIcon(icon);
    mbox.exec();
    //MessageBoxA(0, message.c_str(), caption.c_str(), message_type);
}

void message_handler::display_warning(std::string message, std::string caption)
{
    display_message_box(message, caption, QMessageBox::Warning);
}

void message_handler::display_error_message(std::string message, std::string caption)
{
    display_message_box(message, caption, QMessageBox::Critical);
}

void message_handler::inform_about_exception(std::string caller, std::exception &error, bool terminate)
{
    std::cerr << "Unexpected error occured in " << caller << std::endl;
    std::cerr << "\tCaught: " << error.what() << std::endl;
    std::cerr << "\tType: " << typeid(error).name() << std::endl;
#if  0
    display_error_message("Unexpected error occured in " + caller + "\nType: " + typeid(error).name() + "\n\n\"" + error.what() + "\"");
#endif
    if (terminate) exit(1);
}

