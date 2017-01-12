
#include "Debug.h"
#include "Messenger.h"

#ifdef TARGET_NUCLEO_F303K8
#include "MotionController.h"
#endif

// do com (serial, ...) - This might overwrite sensors inputs
// todo move this shit in a ~class~ separate file
#ifdef TARGET_NUCLEO_F303K8
void com_handle_serial(Debug *debug, CanMessenger *messenger, MotionController *mc)
#else
void com_handle_serial(Debug *debug, CanMessenger *messenger)
#endif
{
    char buffer[BUFFER_SIZE], *ptr = NULL;

    if (debug->get_line(buffer, BUFFER_SIZE) == -1)
        return;

    debug->printf("****************************************\n");

    ptr = &buffer[0];

    if (strcmp(ptr, "ping") == 0)
    {
        debug->printf("pong\n");

        char buffer2[8];
        memset(buffer2, 0, 8);
        buffer2[0] = 1;
        debug->printf("pong via can: %d\n", messenger->send_msg_ping(buffer2));
    }
    else if (strncmp(ptr, "order", 5) == 0)
    {
        ptr = &buffer[5+1];

        if (strncmp(ptr, "dist", 4) == 0)
        {
            ptr = &ptr[4+1];

            int val = atoi(ptr);
            debug->printf("Order rel dist %d mm\n", val);
#ifdef TARGET_NUCLEO_F303K8
            mc->ordersAppendRelDist(val);
#endif
        }
        else if (strncmp(ptr, "angle", 5) == 0)
        {
            ptr = &ptr[5+1];

            int val = atoi(ptr);
            while (val < -180)
                val += 2*180;
            while (val > 180)
                val -= 2*180;

            debug->printf("Order rel angle %d deg\n", val);
#ifdef TARGET_NUCLEO_F303K8
            mc->ordersAppendRelAngle(DEG2RAD(val));
#endif
        }
    }
    else
        debug->printf("Please say again (\"%s\" is not a valid command)\n", buffer);
}

#ifdef TARGET_NUCLEO_F303K8
void com_handle_can(Debug *debug, CanMessenger *messenger, MotionController *mc)
#else
void com_handle_can(Debug *debug, CanMessenger *messenger)
#endif
{
    Message rec_msg;
    while (messenger->read_msg(&rec_msg))
    {
        debug->printf("Messager::read_msg() == 1 - rec_msg.id=%d\n", rec_msg.id);

        switch (rec_msg.id)
        {
            char buffer[8];

            case Message::MT_ping:
                debug->printf("ping %d\n", rec_msg.payload.ping.payload[0]);
                if (rec_msg.payload.ping.payload[0] == 10)
                    break;
                memset(buffer, 0, 8);
                buffer[0] = rec_msg.payload.ping.payload[0]+1;
                messenger->send_msg_ping(buffer);
                break;

            case Message::MT_CQB_MC_order:
                switch (rec_msg.payload.CQB_MC_order.type)
                {
#ifdef TARGET_NUCLEO_F303K8
                    case ORDER_TYPE_POS:
                        mc->ordersAppendAbsPos(
                            rec_msg.payload.CQB_MC_order.order_data.pos.x,
                            rec_msg.payload.CQB_MC_order.order_data.pos.y
                        );
                        break;
                    case ORDER_TYPE_ANGLE:
                        mc->ordersAppendAbsAngle(
                            rec_msg.payload.CQB_MC_order.order_data.angle
                        );
                        break;
                    case ORDER_TYPE_DELAY:
                        mc->ordersAppendAbsDelay(
                            rec_msg.payload.CQB_MC_order.order_data.delay
                        );
                        break;
 #endif
                }
                break;
            default:
                // todo other cases
                debug->printf("Unhandled CAN msg (id=%d)\n", rec_msg.id);
                break;
        }
    }
}
