#ifndef DEBUG_H_INLCUDED
#define DEBUG_H_INLCUDED

#include "mbed.h"
#include "BufferedSerial.h"


#define DEBUG_BUFFER_SIZE 512  // library default is 256


class Debug {

public:
    typedef enum _Interface {
        DEBUG_PC,
        DEBUG_XBEE,
        DEBUG_ALL
    } Interface;

    Debug(PinName pcTx, PinName pcRx, PinName xbeeTx, PinName xbeeRx);
    void printf(const char* format, ...);
    int get_line(char *buffer, int buffer_length, Interface interface = DEBUG_ALL);

protected:
    BufferedSerial pc_;
    BufferedSerial xbee_;
};

#endif
