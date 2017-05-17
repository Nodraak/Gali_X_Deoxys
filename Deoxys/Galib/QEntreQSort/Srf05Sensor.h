#ifdef IAM_QENTRESORT

#ifndef SRF05SENSOR_H_INCLUDED
#define SRF05SENSOR_H_INCLUDED

#include "mbed.h"


/*
    TODO
        define
        doc
        Option to disable US (define / switch)
        Test laser
*/

/*
http://www.robot-electronics.co.uk/htm/srf04tech.htm
http://www.robot-electronics.co.uk/htm/sonar_faq.htm


Q. What distance above the floor should the sonar be mounted?
A. If you can mount the SRF04/8 12in/300mm above the floor, that should be OK. If you mount them lower, you may need to point them upwards slightly to avoid reflections from the carpet pile or ridges in a concrete floor.

Q. Can I fire two or more sonar's at the same time?
A. No!  If two or more sonar's are fired together then they could pick up each other "ping" resulting in a false readings. Fire them sequentially 65mS apart

*/



/*
    Pinout (top to bottom)
        Vcc: 5V
        Trig: trigger input
        Echo: echo output
        out: floating
        Gnd: gnd

    Range is 0 to 1500 mm.

    20 Hz update. (need to wait 50 ms between us bursts)
*/
class Srf05Sensor {
public:
    Srf05Sensor(PinName trigger, PinName echo);

    /*
        Need to wait 50 ms between calls.
    */
    void update(void);

private:
    void echo_callback(void);

public:
    int16_t get_val(void);

private:
    DigitalOut trigger_;
    InterruptIn echo_;
    Timer t_;
    int16_t val_;  // -1: timeout ; other: distance in mm
};

#endif // #ifndef SRF05SENSOR_H_INCLUDED
#endif // #ifdef IAM_QENTRESORT
