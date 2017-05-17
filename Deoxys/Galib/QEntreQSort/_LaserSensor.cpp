#ifdef IAM_QENTRESORT

#include "mbed.h"
#include "QEntreQSort/_LaserSensor.h"

LaserSensor::LaserSensor(PinName pin, EventQueue *queue) : pin_(pin) {
    val_ = 0;
    queue->call_every(LASER_UPDATE_PERIOD, callback(this, &LaserSensor::update));
}

void LaserSensor::update(void) {
    val_ = pin_.read();
}

bool LaserSensor::get_val(void) {
    return val_;
}

#endif // #ifdef IAM_QENTRESORT
