WIP

# System Design

## Overview

We designed a highly distributed architecture with several microcontrollers, with the goal to reuse them the following years. Each microcontroller has a nice french name.

* QReflechi (Board Which Thinks): high level computations, in charge of the startegy and decide what to do
* QBouge (Board Which Moves): in charge of the motion of the robot and keep track of its position
* QEntreSort (Board Which Input-Output): in charge of the actuators and sensors
* Lidar: rotatining laser used to have a 360° view. We lacked time to make it work

Communication:

* Each microcontroller is connected to a CAN bus.
* QReflechi is connected to an Xbee for wireless communication to a computer (for debug) or another robot (for strategy planning).

## Boards

*Note: cf. `code_assumptions.md` for info about frequency and duration of each above task.*

### QBouge (Board Which Moves)

Tasks:

Name    | Scheduling Policy                 | Goal
--------|-----------------------------------|-------
QEI     | Interrupt on GPIO                 | Register the movement of the robot
Asserv  | mbed Ticker (Timer 2 interrupt) - 200 Hz  | Compute the position of the robot from the QEI and update the motors' output (PID)
main loop | main loop - 200 Hz              | Handles low priority tasks (EventQueue: blink led, send ping, etc) and communication (Serial and CAN)

No multi threading is used. The EventQueue is keeped in the main thread so its duration can be easily known.

### QReflechi (Board Which Thinks)

Tasks:

Name    | Scheduling Policy                 | Goal
--------|-----------------------------------|-------
main loop | main loop - 200 Hz              | Handles low priority tasks (EventQueue: blink led, send ping, etc) and communication (Serial and CAN)

Strategy (maybe with behavioral tree) might me computed somewhere here, somehow.

### QEntreSort (Board Which Input-Output)

Tasks:

Name    | Scheduling Policy                 | Goal
--------|-----------------------------------|-------
main loop | main loop - 200 Hz              | Handles low priority tasks (EventQueue: blink led, send ping, etc) and communication (Serial and CAN)

Sensors automatic computation might me set up somewhere here, somehow.

Actuators are handled with a list of orders, similar to the CQB movement orders.


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

### Orders (OrderFIFO, s_order_com, s_order_exe) - State Machine

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

The CAN bus was a good choice for the robot 10/10 would use again. The bad news is it can sometimes be hard to get working, but the good news is that be following a few rules, most problems are gone. (Ref: git commits #29d0f9, #1f2450 and #ef1540)

* One frame id = one board, otherwise arbitration does not work and there are bit errors
* Don't have a too big sleep in the main loop
* Avoid enabling auto bus off management, that could hide real errors
* ~Need to leave_the_bus_for_a_moment() to let other boards enable their CAN~ should be fixed in a new version of mbed
