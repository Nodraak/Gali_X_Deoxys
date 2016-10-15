
#include "mbed.h"
#include "rtos.h"

#include "Debug.h"
#include "pinout.h"
#include "utils.h"

#include "MotionController.h"


int main(void)
{
    char buffer[BUFFER_SIZE];
    Timer loop;

    // init com (serial, wifi, xbee, ...)

    Debug debug;

    // MC
    MotionController mc;

    // init sharp + other sensors
    // init servos + other actuators

    // init timers
    loop.start();

    // init ia ?

    // init tirette interrupt -> polling

    mc.pidDistSetGoal(0);
    mc.pidAngleSetGoal(0);

    while (true)
    {
        loop.reset();

        /*
            inputs
        */

        // update qei
        mc.fetchEncodersValue();

        // update sharp + other sensors

        // do com (serial, ...) - This might overwrite sensors inputs

        if (debug.get_line(buffer, BUFFER_SIZE) != -1)
        {
            if (strcmp(buffer, "ping") == 0)
                debug.printf(Debug::DEBUG_ERROR, "pong\n");
            else if (strncmp(buffer, "dist", 4) == 0)
            {
                int val = atoi(&buffer[4+1]);
                debug.printf(Debug::DEBUG_ERROR, "setting dist to %d\n", val);
                mc.pidDistSetGoal(MM_TO_TICKS(val));
            }
            else if (strncmp(buffer, "angle", 5) == 0)
            {
                int val = atoi(&buffer[5+1]);

                while (val < -180)
                    val += 2*180;
                while (val > 180)
                    val -= 2*180;

                debug.printf(Debug::DEBUG_ERROR, "setting angle to %d\n", val);
                mc.pidAngleSetGoal(DEG2RAD(val));
            }
            else if (strncmp(buffer, "debug", 5) == 0)
            {
                if (strcmp(&buffer[5+1], "on") == 0)
                    debug.set_level(Debug::DEBUG_DEBUG);
                else if (strcmp(&buffer[5+1], "info") == 0)
                    debug.set_level(Debug::DEBUG_INFO);
                else if (strcmp(&buffer[5+1], "off") == 0)
                    debug.set_level(Debug::DEBUG_ERROR);
                else
                    debug.printf(Debug::DEBUG_ERROR, "Error: unknown debug level \"%s\"\n", &buffer[5+1]);
            }
            else
                debug.printf(Debug::DEBUG_ERROR, "Please say again (\"%s\" is not a valid command)\n", buffer);
        }

        /*
            Computations
        */

        mc.updatePositionAndOrder();

        // update ia
        mc.computePid();

        /*
            outputs
        */

        // move
        mc.updateMotors();

        // debug
        mc.debug(&debug);

        debug.printf("=> %d\n\n", loop.read_ms());

        // sleep
        Thread::wait(PID_UPDATE_INTERVAL*1000);
    }

    // do some cleanup ?

    return 0;
}
