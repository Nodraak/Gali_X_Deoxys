
# Tests

Validate the robot is working - ie. every board is behaving as expected.

* On computer
    * Compilation
    * Simulation ?

* On boards - static (Dont forget to switch on the power)
    * Upload all
    * Check it boots (init + run auto tests)
        * Serial
        * CanMessenger
        * RoboticArm (for CQES)
        * Everything else
        * Ping/pong over CAN (wait_other_boards())
    * Run manual tests
        * Xbee and CAN communication
        * Encoders
        * Motors (PWM, direction)
        * Actuators / sensors
        * ...

* On boards - moving
    * Check basic sequence
    * todo - strat
