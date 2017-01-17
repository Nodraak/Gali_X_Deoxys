
#include "mbed.h"
#include "rtos.h"

#include "common/Debug.h"
#include "common/Messenger.h"
#include "common/OrdersFIFO.h"
#include "common/com.h"
#include "common/mem_stats.h"
#include "common/utils.h"
#include "QBouge/MotionController.h"

#include "common/test.h"
#include "QBouge/test_mc.h"

#include "pinout.h"


int demo_1(MotionController *mc)
{
    #define EX_DIST 1000

    int e = 0;

    e += mc->orders_->ordersAppendAbsPos(EX_DIST,    0);
                    e += mc->orders_->ordersAppendAbsDelay(0.500);
            e += mc->orders_->ordersAppendAbsAngle(DEG2RAD(90));
                    e += mc->orders_->ordersAppendAbsDelay(0.500);
    e += mc->orders_->ordersAppendAbsPos(EX_DIST,    EX_DIST);
                    e += mc->orders_->ordersAppendAbsDelay(0.500);
            e += mc->orders_->ordersAppendAbsAngle(DEG2RAD(180));
                    e += mc->orders_->ordersAppendAbsDelay(0.500);
    e += mc->orders_->ordersAppendAbsPos(200,        EX_DIST);
                    e += mc->orders_->ordersAppendAbsDelay(0.500);
            e += mc->orders_->ordersAppendAbsAngle(DEG2RAD(90));
            e += mc->orders_->ordersAppendAbsAngle(DEG2RAD(0));
            e += mc->orders_->ordersAppendAbsAngle(DEG2RAD(270));
            e += mc->orders_->ordersAppendAbsAngle(DEG2RAD(180));
                    e += mc->orders_->ordersAppendAbsDelay(0.500);
    e += mc->orders_->ordersAppendAbsPos(EX_DIST,    EX_DIST);
                    e += mc->orders_->ordersAppendAbsDelay(0.500);
            e += mc->orders_->ordersAppendAbsAngle(DEG2RAD(270));
                    e += mc->orders_->ordersAppendAbsDelay(0.500);
    e += mc->orders_->ordersAppendAbsPos(EX_DIST,    0);
                    e += mc->orders_->ordersAppendAbsDelay(0.500);
            e += mc->orders_->ordersAppendAbsAngle(DEG2RAD(0));
                    e += mc->orders_->ordersAppendAbsDelay(0.500);
    e += mc->orders_->ordersAppendAbsPos(0,          0);

    return e;
}


int demo_2(MotionController *mc)
{
    #define EX_DIST 1000

    int e = 0;

    e += mc->orders_->ordersAppendRelDist(EX_DIST);
                    e += mc->orders_->ordersAppendAbsDelay(0.500);
            e += mc->orders_->ordersAppendRelAngle(DEG2RAD(90));
                    e += mc->orders_->ordersAppendAbsDelay(0.500);
    e += mc->orders_->ordersAppendRelDist(EX_DIST);
                    e += mc->orders_->ordersAppendAbsDelay(0.500);
            e += mc->orders_->ordersAppendRelAngle(DEG2RAD(90));
                    e += mc->orders_->ordersAppendAbsDelay(0.500);
    e += mc->orders_->ordersAppendRelDist(EX_DIST-200);
                    e += mc->orders_->ordersAppendAbsDelay(0.500);
            e += mc->orders_->ordersAppendRelAngle(DEG2RAD(-90));
            e += mc->orders_->ordersAppendRelAngle(DEG2RAD(-90));
            e += mc->orders_->ordersAppendRelAngle(DEG2RAD(-90));
            e += mc->orders_->ordersAppendRelAngle(DEG2RAD(-90));
                    e += mc->orders_->ordersAppendAbsDelay(0.500);
    e += mc->orders_->ordersAppendRelDist(-(EX_DIST-200));
                    e += mc->orders_->ordersAppendAbsDelay(0.500);
            e += mc->orders_->ordersAppendRelAngle(DEG2RAD(90));
                    e += mc->orders_->ordersAppendAbsDelay(0.500);
    e += mc->orders_->ordersAppendRelDist(EX_DIST);
                    e += mc->orders_->ordersAppendAbsDelay(0.500);
            e += mc->orders_->ordersAppendRelAngle(DEG2RAD(90));
                    e += mc->orders_->ordersAppendAbsDelay(0.500);
    e += mc->orders_->ordersAppendRelDist(-EX_DIST);

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
