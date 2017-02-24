
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
