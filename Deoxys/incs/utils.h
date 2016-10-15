#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#define BUFFER_SIZE 256

typedef struct _s_vector_uint16 {
    int16_t x, y;
} s_vector_int16;

#define RAD2DEG(val)        ((val)*180/M_PI)
#define DEG2RAD(val)        ((val)*M_PI/180)

float abs(float f);
double map(double x, double in_min, double in_max, double out_min, double out_max);
double constrain(double val, double min, double max);
double std_rad_angle(double val);

#endif
