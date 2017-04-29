#ifndef STATUSLEDS_H_INLCUDED
#define STATUSLEDS_H_INLCUDED

#include "mbed.h"

/*
    alive (green)
        init basic : debug (uart + leds) + com (canMessenger) -> on
        init full : eveything. board is ready -> off
        running -> blinking
    ping cqb (blue)
    ping cqr (blue)
    ping cqes (blue)
*/
class StatusLeds {
public:
    StatusLeds(PinName alive_pin, PinName pingCQB_pin, PinName pingCQR_pin, PinName pingCQES_pin);
    ~StatusLeds(void);

    void init_half(void);
    void running(void);
    void on_CQB_pong(void *arg);
    void on_CQR_pong(void *arg);
    void on_CQES_pong(void *arg);

private:
    void led_set(DigitalOut *led, bool val);
    void led_toggle(DigitalOut *led);

private:
    DigitalOut *alive_;
    DigitalOut *pingCQB_;
    DigitalOut *pingCQR_;
    DigitalOut *pingCQES_;
};

#endif // #ifndef STATUSLEDS_H_INLCUDED
