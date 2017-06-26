
# Test Protocol

Validate the robot is working - ie. every board is behaving as expected.

* Compile
* On boards - static (Dont forget to switch on the power)
    * Check it boots (init + run auto tests)
        * Serial
        * CanMessenger ping/pong
        * Loading orders and starting match (tirette ?)
    * Run manual tests
        * Sequence is loading and starting :
            * CQB
                * Encoders
                * Motors (PWM, direction)
            * CQES
                * ArmActuators
                * Distance sensor : sharp, us, front, back
                * Color Sensors
* Simulation: it was planned that the strategy would be tested in a simualted environnement, but due to lack of time, it was not implemented
