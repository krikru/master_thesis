#ifndef  MESSAGE_HANDLER_H
#define  MESSAGE_HANDLER_H

////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

/* Standard includes */
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
    static void display_message_box(QString message, QString caption = "rtocean",
        QMessageBox::Icon icon = QMessageBox::Information);
    static void display_information_message(QString message, QString caption = "rtocean");
    static void display_warning_message(QString message, QString caption = "rtocean");
    static void display_error_message(QString message, QString caption = "rtocean");
    static void inform_about_exception(QString caller, std::exception &error, bool terminate = true);
};

#endif  /* MESSAGE_HANDLER_H */
