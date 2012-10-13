#ifndef  MESSAGE_HANDLER_H
#define  MESSAGE_HANDLER_H

////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

/* Standard includes */
#include <string>
#include <stdexcept>

class message_handler {
public:
    static void display_message_box(std::string message, std::string caption = "rtocean",
        UINT message_type = MB_ICONINFORMATION);
    static void display_warning(std::string message, std::string caption = "rtocean");
    static void display_error_message(std::string message, std::string caption = "rtocean");
    static void inform_about_exception(std::string caller, std::exception &error, bool terminate = true);
};

#endif  /* MESSAGE_HANDLER_H */
