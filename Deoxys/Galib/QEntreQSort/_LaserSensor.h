#ifdef IAM_QENTRESORT

#ifndef LASERSENSOR_H_INCLUDED
#define LASERSENSOR_H_INCLUDED

#include "mbed.h"

#define LASER_UPDATE_PERIOD 0.010

/*
    TODO
        define
        doc
        Option to disable US (define / switch)
        Test laser
*/

class LaserSensor {
public:
    LaserSensor(PinName pin, EventQueue *queue);

private:
    void update(void);

public:
    bool get_val(void);

private:
    DigitalIn pin_;
    bool val_;
};

#endif // #ifndef LASERSENSOR_H_INCLUDED
#endif // #ifdef IAM_QENTRESORT
