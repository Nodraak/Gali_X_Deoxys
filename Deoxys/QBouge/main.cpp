
#include "mbed.h"
#include "rtos.h"
#include "mbed_stats.h"

#include "common/Debug.h"
#include "common/Messenger.h"
#include "common/com.h"
#include "common/utils.h"
#include "QBouge/MotionController.h"

#include "common/test.h"
#include "QBouge/test_mc.h"

#include "pinout.h"


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
    debug->printf("CanMessenger     %d\n", sizeof(CanMessenger));

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


int main(void)
{
    int to_sleep = 0;

    Debug *debug = new Debug;

    mem_stats(debug);
    test_run_all(debug);
    Thread::wait(10);

    /*
        Initializing
    */

    debug->printf("Initializing\n");

    MotionController *mc = new MotionController;
    Timer match, loop;  // todo dynamic alloc ?
    match.start();
    loop.start();
    CanMessenger *messenger = new CanMessenger;

    // init sharp + other sensors
    // init servos + other actuators


    // init ia ?

    // init tirette interrupt -> polling

    int ret = demo_2(mc);

    if (ret != 0)
    {
        debug->printf("ERROR MC.ordersAppendAbs() %d\n", ret);
        while (1)
            ;
    }

    debug->printf("Initialisation done.\n\n");

    mem_stats(debug);
    Thread::wait(10);

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

        com_handle_serial(debug, messenger, mc);
        com_handle_can(debug, messenger, mc);

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
        mc->debug(messenger);

        // sleep
        to_sleep = 1000/MAIN_LOOP_FPS - loop.read_ms();
        if (to_sleep > 0)
        {
            debug->printf("[timer/loop] %d (%d)\n\n", loop.read_ms(), to_sleep);
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
