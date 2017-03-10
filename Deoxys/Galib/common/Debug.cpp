
#include <cstdarg>

#include "mbed.h"
#include "BufferedSerial.h"

#include "common/sys.h"
#include "pinout.h"

#include "common/Debug.h"


Debug::Debug(void) :
interfaces_{
    BufferedSerial(USBTX, USBRX, INTERFACE_BUFFER_SIZE),
#ifndef IAM_QENTRESORT
    BufferedSerial(XBEE_TX, XBEE_RX, INTERFACE_BUFFER_SIZE),
#endif
},
interfaces_str_{
    "pc",
#ifndef IAM_QENTRESORT
    "xbee",
#endif
}
{
    int i = 0;

    print_level_ = DEBUG_INITIALISATION;
    current_level_ = DEBUG_INITIALISATION;

    wait_ms(100);  // wait for the various UART buffers to clean up (xbee)

    for (i = 0; i < DEBUG_LAST; ++i)
        interfaces_[i].baud(DEBUG_SPEED);

    this->printf("\n\n========================================\n\n");
    this->printf("Hello, world! (all)\n");

    for (i = 0; i < DEBUG_LAST; ++i)
        interfaces_[i].printf("Hello, world! (%s)\n", interfaces_str_[i]);

    this->printf("\n");
}

void Debug::printf(const char* format, ...) {
    char buffer[BUFFER_SIZE] = "";
    va_list args;

    va_start(args, format);
    vsnprintf(buffer, BUFFER_SIZE, format, args);
    this->printf(current_level_, "%s", buffer);
    va_end(args);
}

void Debug::printf(Level level, const char* format, ...) {
    int i = 0;
    char buffer[BUFFER_SIZE] = "";
    va_list args;

    if (level < print_level_)
        return;

    va_start(args, format);
    vsnprintf(buffer, BUFFER_SIZE, format, args);
    va_end(args);

    for (i = 0; i < DEBUG_LAST; ++i)
        interfaces_[i].printf("%s", buffer);

    if (level == DEBUG_INITIALISATION)
        wait_ms(5);
}

int _get_line(BufferedSerial *interface, char *buffer, int buffer_length) {
    int i = 0;

    for (i = 0; (i < buffer_length-1) && interface->readable(); ++i)
    {
        buffer[i] = interface->getc();
        if ((buffer[i] == '\r') || (buffer[i] == '\n'))
            break;
    }

    buffer[i] = '\0';
    return i;
}

int Debug::get_line(char *buffer, int buffer_length, Interface interface) {
    int ret = 0, i = 0;

    for (i = 0; i < DEBUG_LAST; ++i)
    {
        if ((interface == i) || (interface == DEBUG_ALL))
        {
            ret = _get_line(&interfaces_[i], buffer, buffer_length);
            if (ret != 0)
                return ret;
        }
    }

    return -1;  // nothing to read
}

void Debug::set_print_level(Level level) {
    print_level_ = level;
}

void Debug::set_current_level(Level level) {
    current_level_ = level;
}
