
# Tips For The Future

Here are some random tips and lessons learned for a better future robot.


## Philosophy

### Architecture

* 1 microcontroller for MotionController (movement) and Position (encoders, lidar, whatever). Should be reused every year.
* 1 Rpi for high level strategy (why not with a high level language such as Python), persistent settings, persistent logs and flashing microcontrollers. Connected with persistent wifi / bluetooth.
* During the configuration of the actuators, live upload new settings without recompiling -> so you need a GUI on the remote computer and code on microcontroller (or on the Rpi) to receive the settings. Flash the microcontroller once at the end when everything is configured.

### Reuse

The goal is to save time on things that are hard / time consuming / not appealing to do (ex: movement and position), so that you can spend more time on high level features that are more interesting or score more points (ex: advanced IA).

In particular, don't redo every year: Power board and CQB: always the same, so why not save time and reuse them?

### Gali OS

An OS could be usefull to carry all sorts of tasks:

* Position determination: distance (proxymity) sensors, lidar, (micro switch are a bad idea: waste of time and not always possible, cf coupe 2017 Theme Moon)
* Advanced path finding and obstacle avoidance
* Advanced strategy (behavioral tree ?)
* Cooperation with the other robot (communication via Xbee)


## Tools

### Git(hub|lab)

Write code that can be reused easily :

* Standalone class (minimal dependencies and coupling)
* Documentation, with usage examples
* Minimal side effect (no global variables). Document theses (usage of Timer, or IO, ...)

### GUI: Debug / Configuration (sequence + actuators) / Simulation

* Debug: Physical connection with UART is not efficient, have a GUI that parses Xbee/Bluetooth and present the data visually
* Configuration: (Geogebra seemed usefull, but not that much actually). A gui to visually configure the actuators is easier that having source code to recompile each time
* Simulation: could same *much* time.

Note sur la GUI de debug: ca peut etre tres cool, mais les requirements evoluent vitent en fonction de l'avancée du robot. Faut une grosse etape de reflexion/conception pour faire ca bien, de telle sorte que ca soit utilisable pour debug, configurer les actionneur/capteurs, la strategie, etc.


## IO

### Sensor

* Sharps are shit:
    * Con: Expensive
    * Con: Hard to wire a lot of them (AnalogIn is scarce)
    * Pro: they are easy to integrate (not CMS)
* STM's VL53 seems better:
    * Pro: I2C bus: same number of wire for 1 or 42 sensors
    * Con: they are CMS
    * Con: never used

### Actuators

* Cheap servo:
    * Con: need one PWM for each
* Ax12:
    * Pro: UART-like proprietary bus
    * Con: UART-like proprietary bus

## Dev

### CAN bus

CAN bus is great:

* CAN allows any microcontroller to send a message to any other microcontroller (ex: the lidar says to the CQB: we are at XX,YY)
* It looks good on a resume and make a talking opportunity during an interview (true story) and makes other opponents jealous (this is a major point)
* It is easy to screw up, but if you follow a few basic rules, everything will be fine:
    * 1 message is sent by only 1 microcontroller: otherwise arbitration don't stop collisions, two microcontrollers send two different payload, and bam, bus is fucked up
    * Always have an alive microcontroller that can ACK messages otherwise Bus off error
    * Don't mess up priorities. Most of the time it does not matter, but when things goes sideways, it will matter and can save a robot

*Note: I2C could also do the job.*

## Miscs

* Encoders are not perfect (so fucking expensive and so fucking low level). Try to replace them with computer mouse, IMU, Lidar, distance (proxymity) sensors, etc
* Kalman does not seems that usefull
* PID is awesome, try hard that shit
* Hardware bugs (features?)
    * L476RG:
        * Power jumper (usb vs external)
        * PC15 wtf no digi out? (maybe colliding with a Serial or something?)
    * L432KC:
        * Led on A7 is fuck up due to serial (Serial2)
        * Led on A5 fuck up PWM on D5. Or is it D5 fuck up PWM on A5?
