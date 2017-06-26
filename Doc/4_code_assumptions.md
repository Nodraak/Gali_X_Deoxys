WIP

# Code Assumptions

## Task duration

* asserv fps
* main loop fps
* EventQueue task delays

## Global objects

*Note: check for NULL!*

Type And Name       | Methods Called Globally                               | Explanation
--------------------|-------------------------------------------------------|-------------
Debug *g_debug      | Monitoring::reset() which calls Monitoring::print()   | todo
Monitoring *g_mon   | qei + asserv which are interrupts                     | todo

## Interrupts Priorities

Note: is this really the best order ??

First, dont forget to NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);  // 4 bits for pre-emption priority, 0 bit for subpriority
Second, set default priority for all interrupts. That can't hurt.

* 1: EXTI* (GPIO interrupt in)
* 3: TIM2 (mbed Ticker -> asserv)
* 5: TIM1 (mbed PwmOut)
* 10: CAN
* 11: U(S)ART


Task        | Freq          | Period            | Duration
------------|---------------|-------------------|------------
exti        | max 66k (16k) | 15 us (62)        | 4 us
pwmout      | 50k ?         | 20 us             | very short ? 1-10 us ?
asserv      | max 500 (200) | 2000 us (5000)    | 600-1000 us
can         | ?             | ?                 | ?               per frame ?
uart        | ?             | 50 ?              | ?               per char ?
main loop   | 100           | 10k us            | 200-4000 us

**The duration of an interrupt must not be longuer that the period of a lower priority interrupt**

Board   | Task      | Duration
--------|-----------|----------
CQR     | main loop | 40 to 3500 us - avg 300 us - sum 25k us
CQR     | can usage | 22 avg - max 80
CQES    | main loop | 50 to 6000 us - avg 350 us - sum 20k us
CQB     | qei       | avg 4, sum 40k, count 10k
CQB     | asserv    | avg 600, max 1k, sum 100k,
CQB     | main loop | max 4k, avg 200, sum 10k

---

main loop |  .      |   10 ms   (50 * 0.2)
sleep     |  .      | 990 ms
 .        | asserv  |  100 ms  (200 * 0.5)
 .        | qei     |   40 ms  (10k * 4*10-3)


* qei max 200 ms
* asserv 100-250 ms (200-500 Hz)
* ml 50 ms (250 Hz)


## CAN Bus Usage

500k bauds => 3500 (up to 5000) frames/sec max with 70% usage

Board   | Task                  | Duration
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
