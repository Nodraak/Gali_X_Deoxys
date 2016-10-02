
#include "mbed.h"
#include "rtos.h"

#include "QEI.h"
#include "PID.h"

#include "Motor.h"
#include "utils.h"
#include "Debug.h"
#include "MotionController.h"


int main(void)
{
    char buffer[1024];

    // init com (serial, wifi, xbee, ...)

    Debug debug(USBTX, USBRX, PC_10, PC_11);

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
        // update qei
        // update sharp + other sensors

        // do com (serial, ...) - This might overwrite sensors inputs

        if (debug.get_line(buffer, 1024) != -1)
        {
            if (strcmp(buffer, "hello") == 0)
                debug.printf("gotcha!\n");
            else if (strcmp(buffer, "ping") == 0)
                debug.printf("pong\n");
            else
                debug.printf("Please say again\n");
        }

        // update ia

        // debug

        // sleep

        mc.updateMovement(debug);

        Thread::wait(PID_UPDATE_INTERVAL*1000);
    }

    // do some cleanup ?

    return 0;
}
