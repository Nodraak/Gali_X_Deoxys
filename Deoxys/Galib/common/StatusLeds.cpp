
#include "mbed.h"
#include "common/Messenger.h"
#include "common/StatusLeds.h"


StatusLeds::StatusLeds(
    PinName alive_pin, PinName pingCQB_pin, PinName pingCQR_pin, PinName pingCQES_pin, PinName busOff_pin
) {
    alive_    = alive_pin != NC    ? new DigitalOut(alive_pin)    : NULL;
    pingCQB_  = pingCQB_pin != NC  ? new DigitalOut(pingCQB_pin)  : NULL;
    pingCQR_  = pingCQR_pin != NC  ? new DigitalOut(pingCQR_pin)  : NULL;
    pingCQES_ = pingCQES_pin != NC ? new DigitalOut(pingCQES_pin) : NULL;
    busOff_   = busOff_pin != NC   ? new DigitalOut(busOff_pin)   : NULL;

    led_set(alive_, 1);
    led_set(pingCQB_, 0);
    led_set(pingCQR_, 0);
    led_set(pingCQES_, 0);
    led_set(busOff_, 0);
}

void StatusLeds::init_half(void) {
    led_set(alive_, 0);
}

void StatusLeds::running(void) {
    led_toggle(alive_);
}

void StatusLeds::updateBO(void) {
    #undef BASE_ADDR_CAN1
    #undef CAN_ESR
    #undef CAN_ESR_BOFF
    #define BASE_ADDR_CAN1 0x40006400
    #define CAN_ESR        (BASE_ADDR_CAN1+0x18)
    #define CAN_ESR_BOFF   (((*(uint32_t*)CAN_ESR) >> 2) & 0b1)         // Bit 2 BOFF : Bus-off flag
    led_set(busOff_, CAN_ESR_BOFF);
}

void StatusLeds::on_CQB_pong(void *arg) {
    (void)arg;
    led_toggle(pingCQB_);
}

void StatusLeds::on_CQR_pong(void *arg) {
    (void)arg;
    led_toggle(pingCQR_);
}

void StatusLeds::on_CQES_pong(void *arg) {
    (void)arg;
    led_toggle(pingCQES_);
}

void StatusLeds::led_set(DigitalOut *led, bool val) {
    if (led)
        *led = val;
}

void StatusLeds::led_toggle(DigitalOut *led) {
    if (led)
        *led = !(*led);
}
