
#include <cstdarg>

#include "mbed.h"
#include "BufferedSerial.h"

#include "pinout.h"
#include "Debug.h"


Debug::Debug(void) :
interfaces_{
    BufferedSerial(USBTX, USBRX, INTERFACE_BUFFER_SIZE),
    BufferedSerial(XBEE_TX, XBEE_RX, INTERFACE_BUFFER_SIZE),
    BufferedSerial(SCREEN_TX, SCREEN_RX, INTERFACE_BUFFER_SIZE)
},
interfaces_str_{
    "pc",
    "xbee",
    "screen"
}
{
    int i = 0;

    level_ = DEBUG_DEBUG;

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
    this->printf(DEBUG_DEBUG, "%s", buffer);
    va_end(args);
}

void Debug::printf(Level level, const char* format, ...) {
    int i = 0;
    char buffer[BUFFER_SIZE] = "";
    va_list args;

    if (level < level_)
        return;

    va_start(args, format);
    vsnprintf(buffer, BUFFER_SIZE, format, args);
    va_end(args);

    for (i = 0; i < DEBUG_LAST; ++i)
        interfaces_[i].printf("%s", buffer);
}

int _get_line(BufferedSerial *interface, char *buffer, int buffer_length) {
    int i = 0;

    for (i = 0; (i < buffer_length-1) && interface->readable(); ++i)
    {
        buffer[i] = interface->getc();
        if (buffer[i] == '\n')
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

    return -1;  // should not happen
}

void Debug::set_level(Level level) {
    level_ = level;
}
