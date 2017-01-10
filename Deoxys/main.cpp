
#include "mbed.h"
#include "rtos.h"
#include "mbed_stats.h"

#include "Debug.h"
#include "MotionController.h"
#include "pinout.h"
#include "utils.h"

#include "test.h"
#include "test_mc.h"


void mem_stats(Debug *debug)
{
    debug->printf("----- sizeof\n");
    debug->printf("MotionController %d\n", sizeof(MotionController));
    debug->printf("\tMotor          %d*2=%d\n", sizeof(Motor), sizeof(Motor)*2);
    debug->printf("\tQEI            %d*2=%d\n", sizeof(QEI), sizeof(QEI)*2);
    debug->printf("\tPID            %d*2=%d\n", sizeof(PID), sizeof(PID)*2);
    debug->printf("\ts_order        %d*%d=%d\n", sizeof(s_order), MAX_ORDERS_COUNT, sizeof(s_order)*MAX_ORDERS_COUNT);
    // debug->printf("\ts_vector_float %d\n", sizeof(s_vector_float));
    debug->printf("Debug            %d\n", sizeof(Debug));
    debug->printf("\tBufferedSerial %d*%d=%d\n", sizeof(BufferedSerial), Debug::DEBUG_LAST, sizeof(BufferedSerial)*Debug::DEBUG_LAST);
    debug->printf("Timer            %d\n", sizeof(Timer));

    mbed_stats_heap_t heap_stats;
    osEvent info;
    osThreadId main_id = osThreadGetId();

    debug->printf("----- heap\n");

    mbed_stats_heap_get(&heap_stats);
    debug->printf("Current heap: %d\n", heap_stats.current_size);
    debug->printf("Max heap size: %d\n", heap_stats.max_size);

    debug->printf("----- stack\n");

    info = _osThreadGetInfo(main_id, osThreadInfoStackSize);
    if (info.status != osOK)
        error("Could not get stack size");
    uint32_t stack_size = (uint32_t)info.value.v;

    info = _osThreadGetInfo(main_id, osThreadInfoStackMax);
    if (info.status != osOK)
        error("Could not get max stack");
    uint32_t max_stack = (uint32_t)info.value.v;

    debug->printf("Stack used %d of %d bytes\n", max_stack, stack_size);

    debug->printf("-----\n");
}


void com_handle(Debug *debug, MotionController *mc)
{
    char buffer[BUFFER_SIZE];

    // do com (serial, ...) - This might overwrite sensors inputs
    // todo move this shit in a ~class~ separate file
    if (debug->get_line(buffer, BUFFER_SIZE) != -1)
    {
        if (strcmp(buffer, "ping") == 0)
            debug->printf(Debug::DEBUG_ERROR, "pong\n");
        else if (strncmp(buffer, "dist", 4) == 0)
        {
            int val = atoi(&buffer[4+1]);
            debug->printf(Debug::DEBUG_ERROR, "setting dist to %d\n", val);
            mc->pidDistSetGoal(MM_TO_TICKS(val));
        }
        else if (strncmp(buffer, "angle", 5) == 0)
        {
            int val = atoi(&buffer[5+1]);

            while (val < -180)
                val += 2*180;
            while (val > 180)
                val -= 2*180;

            debug->printf(Debug::DEBUG_ERROR, "setting angle to %d\n", val);
            mc->pidAngleSetGoal(DEG2RAD(val));
        }
        else if (strncmp(buffer, "debug", 5) == 0)
        {
            if (strcmp(&buffer[5+1], "on") == 0)
                debug->set_level(Debug::DEBUG_DEBUG);
            else if (strcmp(&buffer[5+1], "info") == 0)
                debug->set_level(Debug::DEBUG_INFO);
            else if (strcmp(&buffer[5+1], "off") == 0)
                debug->set_level(Debug::DEBUG_ERROR);
            else
                debug->printf(Debug::DEBUG_ERROR, "Error: unknown debug level \"%s\"\n", &buffer[5+1]);
        }
        else
            debug->printf(Debug::DEBUG_ERROR, "Please say again (\"%s\" is not a valid command)\n", buffer);
    }
}


int demo_1(MotionController *mc)
{
    #define EX_DIST 1000

    int e = 0;

    e += mc->ordersAppendAbsPos(EX_DIST,    0);
                    e += mc->ordersAppendAbsDelay(0.500);
            e += mc->ordersAppendAbsAngle(DEG2RAD(90));
                    e += mc->ordersAppendAbsDelay(0.500);
    e += mc->ordersAppendAbsPos(EX_DIST,    EX_DIST);
                    e += mc->ordersAppendAbsDelay(0.500);
            e += mc->ordersAppendAbsAngle(DEG2RAD(180));
                    e += mc->ordersAppendAbsDelay(0.500);
    e += mc->ordersAppendAbsPos(200,        EX_DIST);
                    e += mc->ordersAppendAbsDelay(0.500);
            e += mc->ordersAppendAbsAngle(DEG2RAD(90));
            e += mc->ordersAppendAbsAngle(DEG2RAD(0));
            e += mc->ordersAppendAbsAngle(DEG2RAD(270));
            e += mc->ordersAppendAbsAngle(DEG2RAD(180));
                    e += mc->ordersAppendAbsDelay(0.500);
    e += mc->ordersAppendAbsPos(EX_DIST,    EX_DIST);
                    e += mc->ordersAppendAbsDelay(0.500);
            e += mc->ordersAppendAbsAngle(DEG2RAD(270));
                    e += mc->ordersAppendAbsDelay(0.500);
    e += mc->ordersAppendAbsPos(EX_DIST,    0);
                    e += mc->ordersAppendAbsDelay(0.500);
            e += mc->ordersAppendAbsAngle(DEG2RAD(0));
                    e += mc->ordersAppendAbsDelay(0.500);
    e += mc->ordersAppendAbsPos(0,          0);

    return e;
}


int demo_2(MotionController *mc)
{
    #define EX_DIST 1000

    int e = 0;

    e += mc->ordersAppendRelDist(EX_DIST);
                    e += mc->ordersAppendAbsDelay(0.500);
            e += mc->ordersAppendRelAngle(DEG2RAD(90));
                    e += mc->ordersAppendAbsDelay(0.500);
    e += mc->ordersAppendRelDist(EX_DIST);
                    e += mc->ordersAppendAbsDelay(0.500);
            e += mc->ordersAppendRelAngle(DEG2RAD(90));
                    e += mc->ordersAppendAbsDelay(0.500);
    e += mc->ordersAppendRelDist(EX_DIST-200);
                    e += mc->ordersAppendAbsDelay(0.500);
            e += mc->ordersAppendRelAngle(DEG2RAD(-90));
            e += mc->ordersAppendRelAngle(DEG2RAD(-90));
            e += mc->ordersAppendRelAngle(DEG2RAD(-90));
            e += mc->ordersAppendRelAngle(DEG2RAD(-90));
                    e += mc->ordersAppendAbsDelay(0.500);
    e += mc->ordersAppendRelDist(-(EX_DIST-200));
                    e += mc->ordersAppendAbsDelay(0.500);
            e += mc->ordersAppendRelAngle(DEG2RAD(90));
                    e += mc->ordersAppendAbsDelay(0.500);
    e += mc->ordersAppendRelDist(EX_DIST);
                    e += mc->ordersAppendAbsDelay(0.500);
            e += mc->ordersAppendRelAngle(DEG2RAD(90));
                    e += mc->ordersAppendAbsDelay(0.500);
    e += mc->ordersAppendRelDist(-EX_DIST);

    return e;
}


/*
void i_am_alive(Debug *debug, char *s)
{
    Thread::wait(0.5);
    debug->printf("I am alive! (%s)\n", s);
    Thread::wait(0.5);
}
*/

int main(void)
{
    int to_sleep = 0;

    Debug *debug = new Debug;

    mem_stats(debug);
    test_run_all(debug);

    /*
        Initializing
    */

    debug->printf("Initializing\n");

    MotionController *mc = new MotionController;
    Timer match, loop;  // todo dynamic alloc ?
    match.start();
    loop.start();

    // init sharp + other sensors
    // init servos + other actuators


    // init ia ?

    // init tirette interrupt -> polling

    int ret = demo_1(mc);

    if (ret != 0)
    {
        debug->printf("ERROR MC.ordersAppendAbs() %d\n", ret);
        while (1)
            ;
    }

    debug->printf("Initialisation done.\n\n");

    mem_stats(debug);

    /*
        Ready, wait for tirette
    */

    // todo wait for tirette

    /*
        Go!
    */

    match.reset();
    while (true)
    {
        loop.reset();
        debug->printf("[timer/match] %.3f\n", match.read());

        /*
            inputs
        */

        // update qei
        mc->fetchEncodersValue();

        // update sharp + other sensors

        com_handle(debug, mc);

        /*
            Computations
        */

        mc->updatePosition();
        mc->updateCurOrder(match.read());
        mc->computePid();

        /*
            outputs
        */

        // move
        mc->updateMotors();

        // debug
        mc->debug(debug);

        // sleep
        to_sleep = PID_UPDATE_INTERVAL*1000 - loop.read_ms();
        if (to_sleep > 0)
        {
            debug->printf("[timer/loop] %d\n\n", loop.read_ms());
            Thread::wait(to_sleep);
        }
        else
        {
            debug->printf("[timer/loop] Warn: to_sleep == %d < 0\n\n", to_sleep);
        }
    }

    // do some cleanup ?

    return 0;
}
