#ifdef IAM_QENTRESORT

#include <string.h>
#include <stdlib.h>

#include "common/Debug.h"
#include "QEntreQSort/ColorSensor.h"


ColorSensor::ColorSensor(PinName g, PinName b) : g_(g), b_(b) {
    int i = 0;

    for (i = 0; i < RAW2AVG_WINDOW; ++i)
    {
        raw_g_[i] = g_.read();
        raw_b_[i] = b_.read();
    }

    tim_.start();

    val_ = COLOR_UNKNOWN;

    updating_ = false;
    tick_.attach(callback(this, &ColorSensor::update), 1.000/FREQ_RAW);
}

void ColorSensor::update(void) {
    float tmp_g = 0, tmp_b = 0;
    int i = 0;

    if (updating_)
        return;
    updating_ = true;

    // read new val
    memmove(&raw_g_[0], &raw_g_[1], (RAW2AVG_WINDOW-1)*sizeof(float));
    memmove(&raw_b_[0], &raw_b_[1], (RAW2AVG_WINDOW-1)*sizeof(float));
    raw_g_[RAW2AVG_WINDOW-1] = g_.read();
    raw_b_[RAW2AVG_WINDOW-1] = b_.read();

    // avg
    for (i = 0; i < RAW2AVG_WINDOW; ++i)
    {
        tmp_g += raw_g_[i];
        tmp_b += raw_b_[i];
    }
    last_avg_g_ = tmp_g / RAW2AVG_WINDOW;
    last_avg_b_ = tmp_b / RAW2AVG_WINDOW;

    updating_ = false;
}

e_color ColorSensor::get_val(void) {
    e_color new_color;

    if ((last_avg_g_ > 0.058) && (last_avg_b_ > 0.058))
        new_color = COLOR_WHITE;
    else if ((last_avg_g_ > 0.058) && (last_avg_b_ < 0.058))
        new_color = COLOR_YELLOW;
    else if ((last_avg_g_ < 0.058) && (last_avg_b_ < 0.058))
        new_color = COLOR_BLUE;
    else
        new_color = COLOR_UNKNOWN;

    if ((val_ != new_color) || (new_color == COLOR_UNKNOWN))
    {
        val_ = new_color;
        tim_.reset();
        return COLOR_NOT_CONFIDENT;
    }
    else
    {
        if (tim_.read() < CONFIDENCE_DELAY)
            return COLOR_NOT_CONFIDENT;
        else
            return val_;
    }
}

#endif // #ifdef IAM_QENTRESORT
