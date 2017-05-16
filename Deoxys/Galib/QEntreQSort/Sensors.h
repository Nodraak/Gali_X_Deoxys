#ifndef SENSORS_H_INCLUDED
#define SENSORS_H_INCLUDED

/*
    Note:
        Seules les LED sont alimentés -> du coup 5V
        Les photodiodes sont auto alimentées
        Apparement, la rouge ne marche pas
    20 Hz ok -> 50 / sec
    avg (raw val + dist) sur 1 sec
*/

#include "QEntreQSort/Ax12Driver.h"
#include "common/Debug.h"

#define DIR                     AX12_DIR_CW     // 1 bit
#define SPEED                   (0x3FF)         // 10 bits  // should be 114 RPM ie 2 turns per sec ie 1 sec for a full cylinder turn, but it is 2 sec :/

#define ONE_ROTATION_DURATION   2.0             // sec
#define FREQ_RAW                500
#define RAW2AVG_WINDOW          50
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

    void read_data(void);

    e_color val(void);

private:
    AnalogIn g_;
    AnalogIn b_;

    Ticker tick_;
    float raw_g_[RAW2AVG_WINDOW];
    float raw_b_[RAW2AVG_WINDOW];
    float last_avg_g_;
    float last_avg_b_;

    Timer tim_;
    e_color val_;
};

#endif // #ifndef SENSORS_H_INCLUDED
