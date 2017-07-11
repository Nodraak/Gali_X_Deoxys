#ifdef IAM_QENTRESORT

#ifndef SENSORS_H_INCLUDED
#define SENSORS_H_INCLUDED

/*
    Note:
        Seules les LED sont alimentés -> du coup 5V
        Les photodiodes sont auto alimentées
        Apparement, la rouge ne marche pas
    todo doc in english
    The sensor is shit, so we sample at FREQ_RAW (500) Hz, moving avg on
    last RAW2AVG_WINDOW (50) raw values. we work only on these averaged values.
    fancy algorithms with a calibraton phase has been tried, but the dumb
    version actually works better.

    Blue is not recognized !!
*/


#include "common/Debug.h"
#include "QEntreQSort/Ax12Driver.h"


#define ONE_ROTATION_DURATION   2.0             // sec
#define FREQ_RAW                200
#define RAW2AVG_WINDOW          40
#define CONFIDENCE_DELAY        (0.050)         // default value is (ONE_ROTATION_DURATION/8) (the middle of the color)

#define SIZE_AVG                ((int)(FREQ_RAW*ONE_ROTATION_DURATION))


typedef enum _e_color {
    COLOR_UNKNOWN,
    COLOR_NOT_CONFIDENT,
    COLOR_BLUE,
    COLOR_YELLOW,
    COLOR_WHITE
} e_color;


class ColorSensor {
public:
    ColorSensor(PinName g, PinName b);

private:
    void update(void);

public:
    e_color get_val(void);

private:
    AnalogIn g_;
    AnalogIn b_;

    Ticker tick_;
    float raw_g_[RAW2AVG_WINDOW];
    float raw_b_[RAW2AVG_WINDOW];
    float last_avg_g_;
    float last_avg_b_;
    bool updating_;

    Timer tim_;
    e_color val_;
};


#endif // #ifndef SENSORS_H_INCLUDED
#endif // #ifdef IAM_QENTRESORT

/*
    const char *color2str[] = {
        "COLOR_UNKNOWN",
        "COLOR_NOT_CONFIDENT",
        "COLOR_BLUE",
        "COLOR_YELLOW",
        "COLOR_WHITE"
    };
*/
