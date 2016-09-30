
#include <cstdarg>

#include "mbed.h"
#include "BufferedSerial.h"

#include "Debug.h"


Debug::Debug(PinName pcTx, PinName pcRx, PinName xbeeTx, PinName xbeeRx) :
    pc_(pcTx, pcRx, DEBUG_BUFFER_SIZE),
    xbee_(xbeeTx, xbeeRx, DEBUG_BUFFER_SIZE)
{
    pc_.baud(115200);
    // pc_.format(8, Serial::None, 1);  // usefull?

    xbee_.baud(115200);
    // xbee_.format(8, Serial::None, 1);  // usefull?

    printf("\n\n========================================\n\n");
    printf("Hello world ! (all)\n");
    pc_.printf("Hello, world! (pc)\n");
    xbee_.printf("Hello, world! (xbee)\n");
}

void Debug::printf(const char* format, ...) {
    pc_.printf(format);
    xbee_.printf(format);
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
    int ret = 0;

    if ((interface == DEBUG_PC) || (interface == DEBUG_ALL))
    {
        ret = _get_line(&pc_, buffer, buffer_length);
        if (ret != 0)
            return ret;
    }
    if ((interface == DEBUG_XBEE) || (interface == DEBUG_ALL))
    {
        ret = _get_line(&xbee_, buffer, buffer_length);
        if (ret != 0)
            return ret;
    }

    return -1;  // should not happen
}
