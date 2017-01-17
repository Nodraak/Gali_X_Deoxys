#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include "mbed.h"

typedef struct _s_vector_float {
    float x, y;
} s_vector_float;

typedef struct _s_vector_int16 {
    int16_t x, y;
} s_vector_int16;

#define RAD2DEG(val)        ((val)*180/M_PI)
#define DEG2RAD(val)        ((val)*M_PI/180)

#define ABS(val)            (((val) < 0) ? (-(val)) : (val))
#define DIST(x, y)          sqrt((x)*(x) + (y)*(y))
#define MAX(a, b)           ((a) > (b) ? (a) : (b))

#define FMOD_2PI(angle)       fmod((angle)+M_PI, 2*M_PI)

// Return the angle between -pi and +pi, modulo 2*pi.
#define std_rad_angle(angle)  (     \
    FMOD_2PI(angle) < 0             \
    ? FMOD_2PI(angle)+2*M_PI-M_PI   \
    : FMOD_2PI(angle)-M_PI          \
)

double map(double x, double in_min, double in_max, double out_min, double out_max);
double constrain(double val, double min, double max);

// check we know who we are
#ifndef IAM_QBOUGE
    #ifndef IAM_QREFLECHI
        #error I do not know who I am
    #endif
#endif

#endif
