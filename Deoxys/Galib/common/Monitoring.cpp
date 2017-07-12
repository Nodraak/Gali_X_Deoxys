
#include "mbed.h"
#include "common/Debug.h"
#include "config.h"
#include "common/Monitoring.h"


Monitoring *g_mon = NULL;


Duration::Duration(void) {
    this->reset_stats();
}

void Duration::start_new(void) {
    t_.start();
    t_.reset();
}

void Duration::stop_and_save(void) {
    uint32_t val = 0;
    t_.stop();
    val = t_.read_us();
    if (val < min_) min_ = val;
    if (val > max_) max_ = val;
    sum_ += val;
    count_ ++;
}

void Duration::get_stats(uint32_t *min, uint32_t *max, uint32_t *sum, uint32_t *count, float *avg) {
    *min = min_;
    *max = max_;
    *sum = sum_;
    *count = count_;
    if (count_)
        *avg = 1.0*sum_/count_;
    else
        *avg = -1;
}

void Duration::reset_stats(void) {
    min_ = 1 << 31;
    max_ = 0;
    sum_ = 0;
    count_ = 0;
}


Counter::Counter(void) {
    this->reset_stats();
}

void Counter::inc(void) {
    count_ ++;
}

void Counter::reset_stats(void) {
    count_ = 0;
}

void Counter::get_stats(uint32_t *count) {
    *count = count_;
}


Monitoring::Monitoring(void) {
    this->reset();
}

void Monitoring::reset(void) {
#ifdef PRINT_MONITORING_RESET
    this->debug();
#endif

    qei_interrupt.reset_stats();
    asserv_pl.reset_stats();
    asserv_mc.reset_stats();
    main_loop.reset_stats();
    can_usage.reset_stats();
    can_send_errors.reset_stats();
}

void Monitoring::debug(void) {
    uint32_t min = 0, max = 0, sum = 0, count = 0;
    float avg = 0;

    if (g_debug == NULL)
        return;

    g_debug->printf("[Monitoring] (min max sum count avg | count)\n");

#ifdef IAM_QBOUGE
    qei_interrupt.get_stats(&min, &max, &sum, &count, &avg);
    g_debug->printf("\tqei          %3u %5u %6u %3u %5.1f\n", min, max, sum, count, avg);

    asserv_pl.get_stats(&min, &max, &sum, &count, &avg);
    g_debug->printf("\tasserv_pl    %3u %5u %6u %3u %5.1f\n", min, max, sum, count, avg);

    asserv_mc.get_stats(&min, &max, &sum, &count, &avg);
    g_debug->printf("\tasserv_mc    %3u %5u %6u %3u %5.1f\n", min, max, sum, count, avg);
#endif

    main_loop.get_stats(&min, &max, &sum, &count, &avg);
    g_debug->printf("\tmain_loop    %3u %5u %6u %3u %5.1f\n", min, max, sum, count, avg);

    can_usage.get_stats(&count);
    g_debug->printf("\tcan_usage        %3u\n", count);

    can_send_errors.get_stats(&count);
    g_debug->printf("\tcan_send_errors  %3u\n", count);
}
