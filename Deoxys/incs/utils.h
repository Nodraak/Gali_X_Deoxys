#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#define BUFFER_SIZE 256

typedef struct _s_vector_uint16 {
    int16_t x, y;
} s_vector_int16;

typedef struct _s_vector_float {
    float x, y;
} s_vector_float;

#define RAD2DEG(val)        ((val)*180/M_PI)
#define DEG2RAD(val)        ((val)*M_PI/180)

#define ABS(val)            (((val) < 0) ? (-(val)) : (val))
#define DIST(x, y)          sqrt((x)*(x) + (y)*(y))
#define MAX(a, b)           ((a) > (b) ? (a) : (b))

// Return the angle between -pi and +pi, modulo pi.
#define std_rad_angle(val)  (fmod((val)+M_PI, 2*M_PI)-M_PI)

double map(double x, double in_min, double in_max, double out_min, double out_max);
double constrain(double val, double min, double max);

#endif
