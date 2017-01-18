
#include "common/Debug.h"
#include "common/Messenger.h"
#include "common/OrdersFIFO.h"

#include "com.h"

#ifdef IAM_QBOUGE
#include "QBouge/MotionController.h"
#endif

/*
    do com (serial, can, ...) - This might overwrite sensors inputs
*/

#ifdef IAM_QBOUGE
void com_handle_serial(Debug *debug, CanMessenger *messenger, MotionController *mc)
#endif
#ifdef IAM_QREFLECHI
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
    }
    else if (strncmp(ptr, "order", 5) == 0)
    {
        ptr = &buffer[5+1];

        if (strncmp(ptr, "dist", 4) == 0)
        {
            ptr = &ptr[4+1];

            int val = atoi(ptr);
            debug->printf("Order rel dist %d mm\n", val);
#ifdef IAM_QBOUGE
            mc->orders_->push(OrderCom_makeRelDist(val));
#endif
#ifdef IAM_QREFLECHI
            messenger->send_msg_CQB_MC_order(OrderCom_makeRelDist(val));
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
#ifdef IAM_QBOUGE
            mc->orders_->push(OrderCom_makeRelAngle(DEG2RAD(val)));
#endif
#ifdef IAM_QREFLECHI
            messenger->send_msg_CQB_MC_order(OrderCom_makeRelAngle(DEG2RAD(val)));
#endif
        }
    }
    else
        debug->printf("Please say again (\"%s\" is not a valid command)\n", buffer);
}

#ifdef IAM_QBOUGE
void com_handle_can(Debug *debug, CanMessenger *messenger, MotionController *mc)
#endif
#ifdef IAM_QREFLECHI
void com_handle_can(Debug *debug, CanMessenger *messenger)
#endif
{
    Message rec_msg;
    while (messenger->read_msg(&rec_msg))
    {
        debug->printf("Messager::read_msg() == 1 - rec_msg.id=%d\n", rec_msg.id);

        switch (rec_msg.id)
        {
            case Message::MT_ping:
                debug->printf("ping (responding)\n");
                messenger->send_msg_pong(rec_msg.payload.ping.data);
                break;

            case Message::MT_CQB_pong:
                debug->printf("pong (CQB)\n");
                break;

            case Message::MT_CQR_pong:
                debug->printf("pong (CQR)\n");
                break;

            case Message::MT_CQB_MC_order:
#ifdef IAM_QBOUGE
                mc->orders_->push(rec_msg.payload.CQB_MC_order);
 #endif
                break;
            default:
                // todo other cases
                debug->printf("Unhandled CAN msg (id=%d)\n", rec_msg.id);
                break;
        }
    }
}
