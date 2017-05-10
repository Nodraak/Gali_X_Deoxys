
# System Design

## Overview

Distributed architecture with several microcontrollers:

* QReflechi: high level computations, in charge of the startegy and decide what to do
* QBouge: in charge of the motion of the robot and keep track of its position
* QEntreSort: in charge of the actuators and sensors
* Lidar ? TODO

Communication:

* Each microcontroller is connected to a CAN bus.
* QReflechi is connected to an Xbee for wireless communication to a computer or another robot.

## Boards

### QBouge (Board Which Moves)

Tasks:

* QEI: raw position -> GPIO input interrupt
* Asserv: compute position and control the movement -> mbed Ticker (Timer 2 interrupt)
* main_loop: low priority tasks (EventQueue: blink led, send ping, ...) and handles communication (serial and CAN) -> main thread

No multi threading is used. The EventQueue is keep in the main thread so it duration can be easily known.

*Note: cf. `code_assumptions.md` for info about frequency and duration of each above task.*

### QReflechi (Board Which Thinks)

Tasks:
    * main_loop (com)

Strategy (maybe with behavioral tree) might me computed somewhere, somehow. TODO

### QEntreSort (Board Which Input-Output)

Tasks:
    * main_loop (com)

Sensor automatic computation might me set up somewhere, somehow. TODO
Actuators => ALWAYS sequences, without low level stuff like sleeps, so let's plan the sleep in Actuator::order_decode_sequence()

## Code (Common Classes Only)

### Debug, StatusLeds, Monitoring

TODO

### Communication (com.cpp, Messenger)

TODO update

```
+----------------------------+       CAN bus         +-------------------------------------------+
| QR                         |       (max 8 B)       | QB                                        |
| bcp de ram/ flash          | <-------------------> | peu de ram/flash                          |
| src des orders             |       orderCom (8 B)  | orderCom buffer (~5) -> 8 B => OrdersList |
| orderCom (X) => OrdersList |                       | OrderExe (1) -> X B => MC                 |
+----------------------------+                       +-------------------------------------------+
```

OrdersList (OrdersFIFO) -> orderCom

### Orders (OrderFIFO, s_order_com, s_order_exe)

order
    pos: specific pos, angle not specified and might drift (if the robot does not follow a perfectly straight path)
    angle: specific angle, with trying to hold pos (not exact)
    delay: hold curent pos while sleeping

when the orderFifo is empty, the robot will execute an angle order (it will hold pos and angle)


# Whole Thread, Timer, Ticker, ISR and Interrupt Thing

TODO what is what + rewrite this section

## ISR (interrupt, Timer) vs RTOStimer (in thread rtosxxx)

interrupt (isr, executed through timer interrupt): very high priority, critical code (CQB::asserv)
timer (rtosTimer, executed in a dedicated thread): low prio functions: can bus handling, sending pings msg, debug ?, ... (might be delayed)

## Thread and interrupts

No threads, we can use wait_ms() (otherwise Thread::wait())

EventQueue.dispatch(0)

interrupt -> asserv + qei

## PWM

channels => freq ?


# CAN Issues

Ref: git commits 29d0f9, 1f2450 and ef1540

* One frame id = one board
* Need to leave_the_bus_for_a_moment() to let other boards enable their CAN
* No auto bus off management -> that could hide real errors (others that acknoledgement errors)
* Don't have a too big sleep in the main loop
