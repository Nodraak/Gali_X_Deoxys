
#include "utils.h"

/*
    Re-maps a number from one range to another.
    Notes:
        Does not constrain values to within the range
        The "lower bounds" of either range may be larger or smaller than the "upper bounds"

    Cf. https://www.arduino.cc/en/Reference/Map
*/
double map(double x, double in_min, double in_max, double out_min, double out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

double constrain(double val, double min, double max)
{
    if (val < min)
        return min;
    else if (val > max)
        return max;
    else
        return val;
}


#ifdef TARGET_NUCLEO_L432KC
const char *pin2str(PinName pin)
{
    static const char *_pin2str[] = {
        "PA_0  A0",     // 0x00
        "PA_1  A1",     // 0x01
        "PA_2  A7",     // 0x02
        "PA_3  A2",     // 0x03
        "PA_4  A3",     // 0x04
        "PA_5  A4",     // 0x05
        "PA_6  A5",     // 0x06
        "PA_7  A6",     // 0x07
        "PA_8  D9",     // 0x08
        "PA_9  D1",     // 0x09
        "PA_10 D0",     // 0x0A
        "PA_11 D10",    // 0x0B
        "PA_12 D2",     // 0x0C
        "PA_13 ",       // 0x0D
        "PA_14 ",       // 0x0E
        "PA_15 ",       // 0x0F
        "PB_0  D3",     // 0x10
        "PB_1  D6",     // 0x11
        "PB_2 ",        // 0x12
        "PB_3  D13",    // 0x13
        "PB_4  D12",    // 0x14
        "PB_5  D11",    // 0x15
        "PB_6  D5",     // 0x16
        "PB_7  D4",     // 0x17
    };
    static const char *_nc = "NC";

    if (pin == NC)
        return _nc;
    return _pin2str[pin];
}
#endif
