#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#define PID_UPDATE_INTERVAL (1.0/10)  // sec

#define ENC_RADIUS          28                      // one enc radius
#define ENC_PERIMETER       (2*M_PI*ENC_RADIUS)     // one enc perimeter
#define TICKS_PER_MM        16.5
#define PULSES_PER_REV      (ENC_PERIMETER*TICKS_PER_MM)
#define ENC_POS_RADIUS      87                      // distance from one enc to the center of the robot
#define TICKS_2PI           (87*2*M_PI * TICKS_PER_MM * 2)  // how many enc ticks after a 2*M_PI turn

#define MM_TO_TICKS(val)    ((val)*TICKS_PER_MM)
#define TICKS_TO_MM(val)    ((val)/TICKS_PER_MM)


float abs(float f);
double map(double x, double in_min, double in_max, double out_min, double out_max);
double constrain(double val, double min, double max);


#endif
