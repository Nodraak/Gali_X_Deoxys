#ifndef DEBUG_H_INLCUDED
#define DEBUG_H_INLCUDED

#include "mbed.h"


class Debug {

public:
    Debug(PinName pcTx, PinName pcRx, PinName xbeeTx, PinName xbeeRx);
    void printf(const char* format, ...);

protected:
    Serial pc_;
    Serial xbee_;
};

#endif
