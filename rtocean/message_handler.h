#ifndef  MESSAGE_HANDLER_H
#define  MESSAGE_HANDLER_H

////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

/* Standard includes */
#include <string>
#include <stdexcept>

/* Qt includes */
#include <QString>
#include <QMessageBox>

/* Own includes */
#include "definitions.h"

////////////////////////////////////////////////////////////////
// CLASS DEFINITION
////////////////////////////////////////////////////////////////

class message_handler {
public:
    static void display_message_box(std::string message, std::string caption = "rtocean",
        QMessageBox::Icon icon = QMessageBox::Information);
    static void display_warning(std::string message, std::string caption = "rtocean");
    static void display_error_message(std::string message, std::string caption = "rtocean");
    static void inform_about_exception(std::string caller, std::exception &error, bool terminate = true);
};

#endif  /* MESSAGE_HANDLER_H */
