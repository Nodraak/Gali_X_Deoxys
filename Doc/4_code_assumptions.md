
# Code Assumptions

## Global objects

*Note: check for NULL before use!!*

Type And Name       | Methods Called Globally                               | Explanation
--------------------|-------------------------------------------------------|-------------
Debug *g_debug      | Monitoring::reset() which calls Monitoring::print()   | todo
Monitoring *g_mon   | qei + asserv which are interrupts                     | todo


## Task duration

Task           | Freq          | Period            | Duration
---------------|---------------|-------------------|------------
GPIO int (QEI) | max 66k (16k) | 15 us (62)        | 4 us
PwmOut         | 50k ?         | 20 us             | Shorter than GPIO ? 1-10 us ?
Ticker (Asserv)| max 500 (200) | 2000 us (5000)    | 600-1000 us
CAN            | ?             | ?                 | ?               per frame ?
UART           | ?             | ?                 | ?               per char ?
main loop      | 100           | 10k us            | 200-4000 us

**The duration of an interrupt must not be longuer that the period of a lower priority interrupt.**

Board   | Task      | Duration: avg (min-max)   | Sum Of Duration Over 1 Sec
--------|-----------|---------------------------|----------------------------
CQB     | qei       | 4 us (2-5 us ?)           | 50 ms (max 300 ms)
CQB     | asserv    | 600 us (?-1000 us)        | 100 ms (max 250 ms)
CQB     | main loop | 200 us (?-4000 us)        | 10 ms (max 50 ms ?)
CQR     | main loop | 300 us (40-3500 us)       | 25 ms (max 50 ms ?)
CQR     | can usage | 22 us (?-80)              | ?
CQES    | main loop | 350 us (50-6000 us)       | 20 ms (max 50 ms ?)


## Interrupts Priorities

**The duration of an interrupt must not be longuer that the period of a lower priority interrupt.**

* First, don't forget to call `NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);` to set 4 bits for pre-emption priority and 0 bit for subpriority.
* Second, set default priority for all interrupts. That can't hurt.
* Third, explicitly set needed priorities.

Priority | Periph  | Used for
---------|---------|----------
1        | EXTI*   | GPIO interrupt in (QEI)
2        | TIM2    | mbed Ticker (MC::asserv)
3        | TIM1    | mbed PwmOut (Motors)
4        | CAN     | communication (high priority)
5        | U(S)ART | debug


## CAN Bus Usage

500k bauds => 3500 (up to 5000) frames/sec max with 70% usage

Board   | Task                  | Count
--------|-----------------------|----------
CQB     | ping/pong             | 1 *   1 Hz  =   1
 "      | I_am_finished         | 1 *   1 Hz  =   1 spike
 "      | next_order_request    | 1 *  20 Hz  =  20 spike
 "      | I_am_doing            | 1 * 100 Hz  = 100 optionnal
 "      | MC debug              | 2 * 200 Hz  = 400 optionnal
CQR     | ping/pong             | 1 *   1 Hz  =   1
 "      | orders                | 1 *  20 Hz  =  20 spike
 "      | reset+we_are_at+...   | 5 *   1 Hz  =   5 spike
CQES    | ping/pong             | 1 *   1 Hz  =   1
 "      | I_am_finished         | 1 *   1 Hz  =   1 spike
 "      | next_order_request    | 1 *  20 Hz  =  20 spike
 "      | I_am_doing            | 1 * 100 Hz  = 100 optionnal

Current usage: 700 frames/sec => 20%
