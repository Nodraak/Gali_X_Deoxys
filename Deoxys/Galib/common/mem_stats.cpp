
#include "mbed.h"
#include "mbed_stats.h"

#include "common/Debug.h"
#include "common/Messenger.h"

#include "config.h"

#ifdef IAM_QBOUGE
    #include "PID.h"
    #include "QBouge/Qei.h"
    #include "QBouge/Motor.h"
    #include "QBouge/MotionController.h"
#endif

#include "mem_stats.h"


void mem_stats_objects(Debug *debug)
{
    debug->printf("----- sizeof\n");

    debug->printf("\tDebug            %d\n", sizeof(Debug));
    debug->printf("\t\tBufferedSerial %d*%d=%d\n", sizeof(BufferedSerial), Debug::DEBUG_LAST, sizeof(BufferedSerial)*Debug::DEBUG_LAST);

    debug->printf("\tTimer            %d\n", sizeof(Timer));
    debug->printf("\tCanMessenger     %d\n", sizeof(CanMessenger));
    debug->printf("\ts_order_com      %d\n", sizeof(s_order_com));

#ifdef IAM_QBOUGE
    debug->printf("\tMotionController %d\n", sizeof(MotionController));
    debug->printf("\t\tMotor          %d*2=%d\n", sizeof(Motor), sizeof(Motor)*2);
    debug->printf("\t\tQei            %d*2=%d\n", sizeof(Qei), sizeof(Qei)*2);
    debug->printf("\t\tPID            %d*2=%d\n", sizeof(PID), sizeof(PID)*2);
    debug->printf("\t\ts_order_exe    %d*%d=%d\n", sizeof(s_order_exe), ORDERS_COUNT, sizeof(s_order_exe)*ORDERS_COUNT);
#endif

#ifdef IAM_QREFLECHI
    debug->printf("\ts_order_com      %d*%d=%d\n", sizeof(s_order_com), ORDERS_COUNT, sizeof(s_order_com)*ORDERS_COUNT);
#endif

    debug->printf("\n");
}


void mem_stats_dynamic(Debug *debug)
{
    mbed_stats_heap_t heap_stats;
    osEvent info;
    osThreadId threadid;

    debug->printf("----- symbols\n");

    debug->printf("\tMain SP      0x%x\n", __get_MSP());
    debug->printf("\tProcess SP   0x%x\n", __get_PSP());

    debug->printf("\n");

    debug->printf("\t__HeapLimit  0x%x\n", &__HeapLimit);
    debug->printf("\t__StackLimit 0x%x\n", &__StackLimit);  // MSP start

    debug->printf("\n");

    debug->printf("----- stack\n");

    osThreadEnumId enumid = _osThreadsEnumStart();
    while ((threadid = _osThreadEnumNext(enumid)))
    {
        debug->printf("\tthread id 0x%x\n", (uint32_t)threadid);

        info = _osThreadGetInfo(threadid, osThreadInfoStackSize);
        if (info.status != osOK)
            error("\t\tCould not get stack size");
        uint32_t stack_size = (uint32_t)info.value.v;

        info = _osThreadGetInfo(threadid, osThreadInfoStackMax);
        if (info.status != osOK)
            error("\t\tCould not get max stack");
        uint32_t max_stack = (uint32_t)info.value.v;

        debug->printf("\t\tStack used %d of %d bytes\n", max_stack, stack_size);
    }

    debug->printf("\n");

    osThreadId main_id = osThreadGetId();

    debug->printf("\tcurThreadId = 0x%x\n", (uint32_t)main_id);
    mbed_stats_heap_get(&heap_stats);
    debug->printf("\t\tCurrent heap: %d\n", heap_stats.current_size);
    debug->printf("\t\tMax heap size: %d\n", heap_stats.max_size);

    debug->printf("\n");
}


void mem_stats_settings(Debug *debug)
{
    debug->printf("----- settings\n");

#ifdef IAM_QBOUGE
    debug->printf("\tMAIN_LOOP_FPS    %d\n", MAIN_LOOP_FPS);
    debug->printf("\tASSERV_FPS       %d\n", ASSERV_FPS);
    debug->printf("\tORDERS_COUNT     %d\n", ORDERS_COUNT);
#endif
#ifdef IAM_QREFLECHI
    debug->printf("\tMAIN_LOOP_FPS    %d\n", MAIN_LOOP_FPS);
    debug->printf("\tORDERS_COUNT     %d\n", ORDERS_COUNT);
#endif

#ifdef IAM_QBOUGE
    debug->printf("\t[PID] dist  %.2f %.2f %.2f\n", PID_DIST_P, PID_DIST_I, PID_DIST_D);
    debug->printf("\t[PID] angle %.2f %.2f %.2f\n", PID_ANGLE_P, PID_ANGLE_I, PID_ANGLE_D);
#endif

    debug->printf("\n");
}
