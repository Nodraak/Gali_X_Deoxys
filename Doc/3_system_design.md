
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

* Debug is a wrapper around serial input/output for async communication and debug. It is basically two methods: `printf()` and `get_line()`.
* StatusLeds is a wrapper on top of a few LEDs to show the status of the microcontroller: running, blocked, alive, etc. A LED blinks every second (alive), or on some events (received a CAN msg).
* Monitoring gather some statistics about the performance of the microcontroller: for example, the number of CAN messages sent and received, or the duration of the main loop of the code.

### Communication (com.cpp, Messenger)

com.cpp is just a giant switch case in function of the CAN msg received. Messenger holds the real communication code (which is mostly movement or action orders, whith a few service messages).

A CAN message is composed of three things:

* id: this represent the message type. Only one microcontroller must send a given id, otherwise error might happen, as the CAN arbitration (who speak or shut up when two messages a sent at the same time) will not happen and there will be an error because the payload will be different.
* payload len: 0 to 8
* payload: up to 8 bytes of data

An instanciated Message can be send with the CanMessenger class.

The giant switch case in com.cpp could be factored with callbacks: CanMessenger offer a on_receive_add() method that configure a callback function to be called when receiving a special CAN message id.

### Orders (OrderFIFO, s_order_com, s_order_exe) - State Machine

The OrderFIFO holds the list of action the microcontroller will execute. For CQB it will mostly be movement orders, will for CQES it will mostly be actuator orders.

An order transmitted over the CAN bus and an order ready to be executed is not the same structure. This is due to size constrain of the CAN bus (8 bytes at most).

```
+----------------------------+       CAN bus         +-------------------------------------------+
| QR                         |       (max 8 B)       | QB                                        |
| lots of ram/ flash         | <-------------------> | few ram/flash                             |
| src of orders              |       orderCom (8 B)  | orderCom buffer (~5) -> 8 B => OrdersList |
| orderCom (X) => OrdersList |                       | OrderExe (1) -> X B => MC                 |
+----------------------------+                       +-------------------------------------------+
```

*When the orderFifo is empty, the robot will execute an angle order (it will hold pos and angle).*

## Whole Thread, Timer, Ticker, ISR and Interrupt Thing

This is quite a mess to grasp at the beginning, but let's try to explain that.

* EventQueue: Register functions to be executed at a specific timestamp (every X sec, at time Y, or in Z sec). To actually execute them, a special method must be called (EventQueue::dispatch) and the function will be executed in the current executing thread.
* Thread: A thread execute at the same level as the main loop, possibly with a different priority. Most of the time, the OS is responsible for prempting the current task to execute the next one (to give the illusion of multitasking).
* Ticker: Register functions to be executed at a specific timestamp, but when the timestamp expire, fires an interrupt to execute that function in an ISR context. The current executing task is paused and resume once the function returns.
* ISR Interrupt: An ISR (Interrupt Service Routine) can be fired by a Ticker, of by a GPIO properly configured.

The tradeoff is basically: A long ISR will block the main loop (and possibly other interrupts), so ISR must be kept as short as possible, and only for critical pieces of code. EventQueue and Thread don't block interrupts, but the scheduled functions can be delayed. This is suitable for low priority tasks.

* Ticker (ISR): very high priority, critical code (QEI and CQB::asserv)
* EventQueue: low priority tasks (might be delayed): CAN bus handling, sending ping and debug, ...

Since we don't use threads, we can use wait_ms() (otherwise Thread::wait()). To execute the scheduled tasks, call `EventQueue.dispatch(0)`

## PWM

PWM uses different timers (channels). A same channel equal a same PWM frequency (but not necessarily same value).

## CAN Issues

The CAN bus was a good choice for the robot, 10/10 would use again.
The bad news is it can sometimes be hard to get working, but the good news is that be following a few rules, most problems are gone.
(Ref: git commits #29d0f9, #1f2450 and #ef1540)

A few tips:

* One frame id = one board, otherwise arbitration does not happen and there are bit errors during payload transmission
* Don't have a too big sleep in the main loop
* Avoid enabling auto bus off management, that could hide real errors
* ~Need to leave_the_bus_for_a_moment() to let other boards enable their CAN~ should be fixed in a new version of mbed
