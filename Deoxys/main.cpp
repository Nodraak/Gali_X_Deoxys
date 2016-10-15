
#include "mbed.h"
#include "rtos.h"

#include "Debug.h"
#include "pinout.h"
#include "utils.h"

#include "MotionController.h"


int main(void)
{
    char buffer[BUFFER_SIZE];

    // init com (serial, wifi, xbee, ...)

    Debug debug;

    // MC
    MotionController mc;

    // init sharp + other sensors
    // init servos + other actuators

    // init timers ?
    // init ia ?

    // init tirette interrupt

    mc.pid_dist_.setSetPoint(MM_TO_TICKS(2000));  // goal
    mc.pid_angle_.setSetPoint(M_PI/2);  // goal

    while (true)
    {
        /*
            inputs
        */

        // update qei
        mc.fetchEncodersValue();

        // update sharp + other sensors

        // do com (serial, ...) - This might overwrite sensors inputs

        if (debug.get_line(buffer, BUFFER_SIZE) != -1)
        {
            if (strcmp(buffer, "hello") == 0)
                debug.printf("gotcha!\n");
            else if (strcmp(buffer, "ping") == 0)
                debug.printf("pong\n");
            else
                debug.printf("Please say again\n");
        }

        /*
            Computations
        */

        // update ia
        mc.computePid();

        /*
            outputs
        */

        // move
        mc.updateMotors();

        // debug
        mc.debug(&debug);

        // sleep
        Thread::wait(PID_UPDATE_INTERVAL*1000);
    }

    // do some cleanup ?

    return 0;
}
