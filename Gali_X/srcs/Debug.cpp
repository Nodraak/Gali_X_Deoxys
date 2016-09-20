
#include <cstdarg>

#include "mbed.h"

#include "Debug.h"


Debug::Debug(PinName pcTx, PinName pcRx, PinName xbeeTx, PinName xbeeRx) : pc_(pcTx, pcRx), xbee_(xbeeTx, xbeeRx) {
    pc_.baud(115200);
    // pc_.format(8, Serial::None, 1);  // usefull?
    pc_.printf("Hello, world! (pc)\n");

    xbee_.baud(115200);
    // xbee_.format(8, Serial::None, 1);  // usefull?
    xbee_.printf("Hello, world! (xbee)\n");
}

void Debug::printf(const char* format, ...) {
    pc_.printf(format);
    xbee_.printf(format);
}
