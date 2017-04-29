#ifndef MAIN_SLEEP_H_INCLUDED
#define MAIN_SLEEP_H_INCLUDED

void main_print_time(Debug *debug, Timer *main_timer);

/*
    Sleeps the needed amount of time the ensure the main loop last `MAIN_LOOP_DELAY`.
*/
void main_sleep(Debug *debug, Timer *loop);

#endif  // MAIN_SLEEP_H_INCLUDED
