
#include "common/Debug.h"
#include "common/Messenger.h"
#include "common/OrdersFIFO.h"

#include "common/com.h"

#ifdef IAM_QBOUGE
#include "QBouge/MotionController.h"
#endif

#ifdef IAM_QENTRESORT
#include "QEntreQSort/Actuators.h"
#endif

#ifdef IAM_QREFLECHI

/*
    com over serial
    Only for QR (which has the screen and xbee. All other com are via the CAN bus)
*/

void com_handle_serial(Debug *debug, CanMessenger *messenger)
{
    char buffer[BUFFER_SIZE], *ptr = NULL;

    if (debug->get_line(buffer, BUFFER_SIZE) == -1)
        return;

    debug->printf("****************************************\n");

    ptr = &buffer[0];

    if (strcmp(ptr, "ping") == 0)
    {
        debug->printf("[UART] pong\n");
        if (messenger->send_msg_CQR_ping())
            debug->printf("[CAN] send_msg_ping() failed\n");
    }
    else if (strncmp(ptr, "order", 5) == 0)
    {
        ptr = &buffer[5+1];

        if (strncmp(ptr, "pos", 3) == 0)
        {
            ptr = &ptr[3+1];
            int x = atoi(ptr);
            while (*ptr != ' ' && *ptr != '\0')
                ptr ++;
            ptr ++;
            int y = atoi(ptr);
            debug->printf("[UART] Order pos %d %d mm\n", x, y);
            if (messenger->send_msg_CQR_order(OrderCom_makeAbsPos(x, y)))
                debug->printf("[CAN] send_msg_order() failed\n");
        }
        else if (strncmp(ptr, "dist", 4) == 0)
        {
            ptr = &ptr[4+1];

            int val = atoi(ptr);
            debug->printf("[UART] Order rel dist %d mm\n", val);
            if (messenger->send_msg_CQR_order(OrderCom_makeRelDist(val)))
                debug->printf("[CAN] send_msg_order() failed\n");
        }
        else if (strncmp(ptr, "angle", 5) == 0)
        {
            ptr = &ptr[5+1];

            int val = atoi(ptr);
            val = val % 360;
            if (val > 180)
                val -= 360;

            debug->printf("[UART] Order rel angle %d deg\n", val);
            if (messenger->send_msg_CQR_order(OrderCom_makeRelAngle(DEG2RAD(val))))
                debug->printf("[CAN] send_msg_order() failed\n");
        }
        else
            debug->printf("[UART] error command order\n");
    }
    else
        debug->printf("[UART] Please say again (\"%s\" is not a valid command)\n", buffer);
}
#endif

#ifdef IAM_QBOUGE
void com_handle_can(
    Debug *debug, CanMessenger *messenger, OrdersFIFO *orders,
    bool *cqes_finished, bool *cqr_finished, MotionController *mc
)
#endif
#ifdef IAM_QREFLECHI
void com_handle_can(Debug *debug, CanMessenger *messenger, OrdersFIFO *orders)
#endif
#ifdef IAM_QENTRESORT
void com_handle_can(
    Debug *debug, CanMessenger *messenger, OrdersFIFO *orders,
    bool *cqb_finished, bool *cqr_finished, Actuators *actuators
)
#endif
{
    Message rec_msg;

    while (messenger->read_msg(&rec_msg))
    {
#ifdef PRINT_COM_CAN_REC
        debug->printf("[CAN/rec] %d %s\n", rec_msg.id, e2s_message_type(rec_msg.id));
#endif
        switch (rec_msg.id)
        {
            case Message::MT_CQR_ping:
                debug->printf("\t-> Replying pong\n");
                if (messenger->send_msg_pong())
                    debug->printf("\t-> Error: send_msg_pong()\n");
                break;

            case Message::MT_CQB_pong:
            case Message::MT_CQR_pong:
            case Message::MT_CQES_pong:
                // nothing to do
                break;

#ifdef IAM_QBOUGE
            case Message::MT_CQR_we_are_at:
                orders->we_are_at(
                    rec_msg.payload.CQR_we_are_at.pos.x,
                    rec_msg.payload.CQR_we_are_at.pos.y,
                    rec_msg.payload.CQR_we_are_at.angle
                );
                mc->we_are_at(
                    rec_msg.payload.CQR_we_are_at.pos.x,
                    rec_msg.payload.CQR_we_are_at.pos.y,
                    rec_msg.payload.CQR_we_are_at.angle
                );
                break;

            case Message::MT_CQR_order:
                // todo ack if ok, else send error
                // todo sync with CQES
                debug->printf("\t-> MT_CQR_order %d\n", orders->append(rec_msg.payload.CQR_order));
                break;

            case Message::MT_CQES_finished:
                *cqes_finished = true;
                break;

            case Message::MT_CQR_finished:
                *cqr_finished = true;
                break;

            case Message::MT_CQR_settings_CQB:
                mc->print(debug);
                mc->set(rec_msg.payload.CQR_settings_CQB.what, rec_msg.payload.CQR_settings_CQB.val);
                mc->print(debug);
                break;

            case Message::MT_CQES_I_am_doing:
                // ignore on CQB
                break;
#endif

#ifdef IAM_QREFLECHI
            case Message::MT_CQB_next_order_request:
                debug->printf("\t-> MT_CQB_next_order_request\n");
                if (orders->size() == 0)
                    debug->printf("\t-> orders->size() == 0\n");
                // todo send a shut up order
                else
                {
                    debug->printf("\t-> sending order %s\n", e2s_order_com_type[orders->front()->type]);
                    if (messenger->send_msg_CQR_order(*orders->front()))
                        debug->printf("\t-> Error: send_msg_order()\n");
                    else
                        orders->pop(); // todo wait for ack before poping
                }
                break;

            case Message::MT_CQB_I_am_doing:
            case Message::MT_CQES_I_am_doing:
                debug->printf(
                    "\t-> I am doing %s %s\n",
                    rec_msg.payload.I_am_doing.i_am,
                    e2s_order_exe_type[rec_msg.payload.I_am_doing.order]
                );
                break;

            case Message::MT_CQB_MC_pos_angle:
                debug->printf(
                    "\t-> pos angle %d %d %.0f\n",
                    rec_msg.payload.CQB_MC_pos_angle.pos.x,
                    rec_msg.payload.CQB_MC_pos_angle.pos.y,
                    RAD2DEG(rec_msg.payload.CQB_MC_pos_angle.angle)
                );
                break;
#endif

#ifdef IAM_QENTRESORT
            case Message::MT_CQB_MC_pos_angle:
                // ignore on CQES
                break;

            case Message::MT_CQR_order:
                debug->printf(
                    "\t-> MT_order (%d) (%s)\n",
                    orders->append(rec_msg.payload.CQR_order),
                    e2s_order_com_type[rec_msg.payload.CQR_order.type]
                );
                // todo sync with CQB
                break;

            case Message::MT_CQB_next_order_request:
                // ignore on CQES
                break;

            case Message::MT_CQR_settings_CQB:
                // ignore on CQES
                break;

            case Message::MT_CQR_settings_CQES:
                actuators->set(rec_msg.payload.CQR_settings_CQES.act, rec_msg.payload.CQR_settings_CQES.val);
                break;

            case Message::MT_CQB_finished:
                *cqb_finished = true;
                break;

            case Message::MT_CQR_finished:
                *cqr_finished = true;
                break;

            case Message::MT_CQB_I_am_doing:
                // ignore on CQES
                break;
#endif

            default:
                debug->printf("\t-> Unhandled msg (id=%d)\n", rec_msg.id);
                break;
        }
    }
}
