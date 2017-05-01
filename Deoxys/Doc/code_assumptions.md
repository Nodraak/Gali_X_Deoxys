
# Code Assumptions

## Task duration

asserv fps
main loop fps
EventQueue task delays

## Global objects

*Note: check for NULL!*

Debug *g_debug; -> Monitoring::reset() which calls Monitoring::print()
Monitoring *g_mon; -> qei + asserv which are interrupts

## Interrupts Priorities

Note: is this really the best order ??

First, dont forget to NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);  // 4 bits for pre-emption priority, 0 bit for subpriority
Second, set default priority for all interrupts. That can't hurt.

1: EXTI* (GPIO interrupt in)
3: TIM2 (mbed Ticker -> asserv)
5: TIM1 (mbed PwmOut)
10: CAN
11: U(S)ART

---

int     freq            period          duration
exti    max 66k (16k)   15 us (62 us)   3.5 us
asserv  200-500         2-5 ms          ??
pwmout  50k ?           20 us           very short ? 1-10 us ?
can     ?               ?               ?               per frame ?
uart    ?               ?               ?               per char ?

=> The duration of an interrupt must not be longuer that the period of a lower priority interrupt


## CAN Bus Usage

500k bauds => 3500 (5000) frames/sec max with 70% usage

* QBouge
    * ping/pong             1 *   1 Hz  =   1
    * I_am_finished         1 *   1 Hz  =   1 spike
    * next_order_request    1 *  20 Hz  =  20 spike
    * I_am_doing            1 * 100 Hz  = 100 optionnal
    * MC debug              2 * 200 Hz  = 400 optionnal
* QReflechi
    * ping/pong             1 *   1 Hz  =   1
    * orders                1 *  20 Hz  =  20 spike
    * reset+we_are_at+...   5 *   1 Hz  =   5 spike
* QEntreSort
    * ping/pong             1 *   1 Hz  =   1
    * I_am_finished         1 *   1 Hz  =   1 spike
    * next_order_request    1 *  20 Hz  =  20 spike
    * I_am_doing            1 * 100 Hz  = 100 optionnal

Sums
    *   3
    *  67 spike
    * 600 optionnal

=> 700 (1000) messages / sec with 70% usage.
=> ~100k bauds
