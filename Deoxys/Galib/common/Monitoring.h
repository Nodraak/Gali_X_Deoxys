#ifndef MONITORING_H_INCLUDED
#define MONITORING_H_INCLUDED

/*
Duration
    qei interrupt duration
    asserv interrupt duration
    main loop duration
Counter
    can bus message exchanged (send + received) count
    can bus send errors
*/


#include "mbed.h"


class Duration {
public:
    Duration(void);
    void start_new(void);  // start the timer
    void stop_and_save(void);  // stop the timer and save the value
    void reset_stats(void);
    void get_stats(uint32_t *min, uint32_t *max, uint32_t *sum, uint32_t *count, float *avg);

private:
    uint32_t min_, max_, sum_, count_;
    Timer t_;
};

class Counter {
public:
    Counter(void);
    void inc(void);  // incremente
    void reset_stats(void);
    void get_stats(uint32_t *count);

private:
    uint32_t count_;
};


class Monitoring {
public:
    Monitoring(void);
    void reset(void);
    void debug(void);

public:
    Duration qei_interrupt;
    Duration asserv;
    Duration main_loop;
    Counter can_usage;
    Counter can_send_errors;
};


extern Monitoring *g_mon;


#endif // #ifndef MONITORING_H_INCLUDED
