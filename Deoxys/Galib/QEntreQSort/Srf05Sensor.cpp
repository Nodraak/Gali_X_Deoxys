#ifdef IAM_QENTRESORT

#include "mbed.h"
#include "QEntreQSort/Srf05Sensor.h"


Srf05Sensor::Srf05Sensor(PinName trigger, PinName echo) : trigger_(trigger), echo_(echo)
{
    val_ = -1;
    trigger_.write(0);
    t_.start();
    echo_.fall(callback(this, &Srf05Sensor::echo_callback));
}

void Srf05Sensor::update(void) {
    trigger_.write(1);
    wait_us(15); // todo datasheet states 10 us min.
    trigger_.write(0);
    t_.reset();
}

void Srf05Sensor::echo_callback(void) {
    uint32_t t_val = t_.read_us() - 700;  // TODO define 700 and explain why

    // valid distance if 0.1 to 25 ms. Timeout at 30
    if (t_val > 25*1000)
        val_ = -1;
    else
        val_ = t_val / 5.8;  // TODO define - state that it is from datasheet
}

int16_t Srf05Sensor::get_val(void) {
    return val_;
}

#endif // #ifdef IAM_QENTRESORT
