
#include "common/Debug.h"
#include "common/Messenger.h"
#include "common/OrdersFIFO.h"
#include "QEntreQSort/RoboticArm.h"

#include "common/com.h"

#ifdef IAM_QBOUGE
#include "QBouge/MotionController.h"
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
        if (messenger->send_msg_ping())
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
            if (messenger->send_msg_order(OrderCom_makeAbsPos(x, y)))
                debug->printf("[CAN] send_msg_order() failed\n");
        }
        else if (strncmp(ptr, "dist", 4) == 0)
        {
            ptr = &ptr[4+1];

            int val = atoi(ptr);
            debug->printf("[UART] Order rel dist %d mm\n", val);
            if (messenger->send_msg_order(OrderCom_makeRelDist(val)))
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
            if (messenger->send_msg_order(OrderCom_makeRelAngle(DEG2RAD(val))))
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
void com_handle_can(Debug *debug, CanMessenger *messenger, MotionController *mc)
#endif
#ifdef IAM_QREFLECHI
void com_handle_can(Debug *debug, CanMessenger *messenger, OrdersFIFO *orders)
#endif
#ifdef IAM_QENTRESORT
void com_handle_can(Debug *debug, CanMessenger *messenger, AX12_arm *ax12_arm)
#endif
{
    Message rec_msg;
    while (messenger->read_msg(&rec_msg))
    {
        debug->printf("[CAN/rec] id %d\n", rec_msg.id);
        switch (rec_msg.id)
        {
            case Message::MT_ping:
                debug->printf("[CAN/rec] ping. replying pong\n");
                if (messenger->send_msg_pong())
                    debug->printf("[CAN] send_msg_pong() failed\n");
                break;

            case Message::MT_CQB_pong:
                debug->printf("[CAN/rec] pong (CQB)\n");
                break;

            case Message::MT_CQR_pong:
                debug->printf("[CAN/rec] pong (CQR)\n");
                break;

            case Message::MT_CQES_pong:
                debug->printf("[CAN/rec] pong (CQES)\n");
                break;

#ifdef IAM_QBOUGE
            case Message::MT_CQR_we_are_at:
                NVIC_DisableIRQ(TIM2_IRQn);
                mc->we_are_at(
                    rec_msg.payload.CQR_we_are_at.pos.x,
                    rec_msg.payload.CQR_we_are_at.pos.y,
                    rec_msg.payload.CQR_we_are_at.angle
                );
                NVIC_EnableIRQ(TIM2_IRQn);
                break;

            case Message::MT_CQR_reset:
                NVIC_DisableIRQ(TIM2_IRQn);
                mc->reset();
                NVIC_EnableIRQ(TIM2_IRQn);
                break;

            case Message::MT_order:
                NVIC_DisableIRQ(TIM2_IRQn);
                debug->printf("[CAN] rec MT_order (%d)\n", mc->orders_->push(rec_msg.payload.order));
                // todo ack if ok, else send error
                NVIC_EnableIRQ(TIM2_IRQn);
                break;
#endif

#ifdef IAM_QREFLECHI
            case Message::MT_CQB_next_order_request:
                debug->printf("[CAN] MT_CQB_next_order_request\n");
                if (orders->size() == 0)
                    debug->printf("-> orders->size() == 0\n");
                // todo send a shut up order
                else
                {
                    if (messenger->send_msg_order(*orders->front()))
                        debug->printf("[CAN] send_msg_order() failed\n");
                    orders->pop(); // todo wait for ack before poping
                }
                break;

        case Message::MT_CQB_MC_pos_angle:
            debug->printf(
                "[CAN/CQB] pos angle %d %d %.0f\n",
                rec_msg.payload.CQB_MC_pos_angle.pos.x,
                rec_msg.payload.CQB_MC_pos_angle.pos.y,
                RAD2DEG(rec_msg.payload.CQB_MC_pos_angle.angle)
            );
            break;

        case Message::MT_CQB_MC_speeds:
            debug->printf(
                "[CAN/CQB] speeds %.0f %.0f\n",
                rec_msg.payload.CQB_MC_speeds.speed,
                rec_msg.payload.CQB_MC_speeds.speed_ang
            );
            break;

        case Message::MT_CQB_sleeping_a_bit:
            debug->printf("\n\n******\nsleeping\n******\n\n");
            break;
#endif

            default:
                debug->printf("[CAN] Unhandled msg (id=%d)\n", rec_msg.id);
                break;
        }
    }
}
