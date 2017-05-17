#ifdef IAM_QENTRESORT

#include "mbed.h"
#include "QEntreQSort/SharpSensor.h"

SharpSensor::SharpSensor(PinName pin, EventQueue *queue) : pin_(pin) {
    val_ = -1;
    queue->call_every(SHARP_UPDATE_PERIOD, callback(this, &SharpSensor::update));
}

void SharpSensor::update(void) {
    val_ = 75 * 1.0/pin_.read();  // TODO define - state that it is empirical
    // todo: detect errors due to too close / too far ?
}

int16_t SharpSensor::get_val(void) {
    return val_;
}

#endif // #ifdef IAM_QENTRESORT
