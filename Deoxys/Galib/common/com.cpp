
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
        debug->printf("[UART] pong\n");
        if (messenger->send_msg_ping(NULL))
            debug->printf("[CAN] send_msg_ping() failed\n");
    }
    else if (strcmp(ptr, "debug") == 0)
    {
        mc->debug(debug);
    }
    else if (strncmp(ptr, "order", 5) == 0)
    {
        ptr = &buffer[5+1];

        if (strncmp(ptr, "reset", 5) == 0)
        {
            mc->ordersReset();
        }
        else if (strncmp(ptr, "pos", 3) == 0)
        {
            ptr = &ptr[3+1];
            int x = atoi(ptr);
            while (*ptr != ' ' && *ptr != '\0')
                ptr ++;
            ptr ++;
            int y = atoi(ptr);
            debug->printf("[UART] Order pos %d %d mm\n", x, y);
#ifdef IAM_QBOUGE
            mc->orders_->push(OrderCom_makeAbsPos(x, y));
#endif
#ifdef IAM_QREFLECHI
            // if (messenger->send_msg_order(OrderCom_makeRelDist(val)))
            //     debug->printf("[CAN] send_msg_order() failed\n");
            // todo
#endif

        }
        else if (strncmp(ptr, "dist", 4) == 0)
        {
            ptr = &ptr[4+1];

            int val = atoi(ptr);
            debug->printf("[UART] Order rel dist %d mm\n", val);
#ifdef IAM_QBOUGE
            mc->orders_->push(OrderCom_makeRelDist(val));
#endif
#ifdef IAM_QREFLECHI
            if (messenger->send_msg_order(OrderCom_makeRelDist(val)))
                debug->printf("[CAN] send_msg_order() failed\n");
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

            debug->printf("[UART] Order rel angle %d deg\n", val);
#ifdef IAM_QBOUGE
            mc->orders_->push(OrderCom_makeRelAngle(DEG2RAD(val)));
#endif
#ifdef IAM_QREFLECHI
            if (messenger->send_msg_order(OrderCom_makeRelAngle(DEG2RAD(val))))
                debug->printf("[CAN] send_msg_order() failed\n");
#endif
        }
    }
    else if (strncmp(ptr, "pid", 3) == 0)
    {
        ptr = &ptr[3+1];

        static float
            s_dist_p = PID_DIST_P,
            s_dist_i = PID_DIST_I,
            s_dist_d = PID_DIST_D,
            s_angle_p = PID_ANGLE_P,
            s_angle_i = PID_ANGLE_I,
            s_angle_d = PID_ANGLE_D;

        if (strncmp(ptr, "dist", 4) == 0)
        {
            ptr = &ptr[4+1];

            if (strncmp(ptr, "p", 1) == 0)
            {
                ptr = &ptr[1+1];
                float val = atof(ptr);
                s_dist_p = val;
                debug->printf("pid dist p %f\n", s_dist_p);
            }
            else if (strncmp(ptr, "i", 1) == 0)
            {
                ptr = &ptr[1+1];
                float val = atof(ptr);
                s_dist_i = val;
                debug->printf("pid dist i %f\n", s_dist_i);
            }
            else if (strncmp(ptr, "d", 1) == 0)
            {
                ptr = &ptr[1+1];
                float val = atof(ptr);
                s_dist_d = val;
                debug->printf("pid dist d %f\n", s_dist_d);
            }
            else
                debug->printf("[PID] Error \"pid dist (p|i|d)\"\n");
        }
        else if (strncmp(ptr, "angle", 5) == 0)
        {
            ptr = &ptr[5+1];

            if (strncmp(ptr, "p", 1) == 0)
            {
                ptr = &ptr[1+1];
                float val = atof(ptr);
                s_angle_p = val;
                debug->printf("pid angle p %f\n", s_dist_p);
            }
            else if (strncmp(ptr, "i", 1) == 0)
            {
                ptr = &ptr[1+1];
                float val = atof(ptr);
                s_angle_i = val;
                debug->printf("pid angle i %f\n", s_dist_i);
            }
            else if (strncmp(ptr, "d", 1) == 0)
            {
                ptr = &ptr[1+1];
                float val = atof(ptr);
                s_angle_d = val;
                debug->printf("pid angle d %f\n", s_dist_d);
            }
            else
                debug->printf("[PID] Error \"pid angle (p|i|d)\"\n");
        }
        else
            debug->printf("[PID] Error \"pid (dist|angle)\"\n");

        mc->pid_dist_.setTunings(s_dist_p, s_dist_i, s_dist_d);
        mc->pid_angle_.setTunings(s_angle_p, s_angle_i, s_angle_d);

        debug->printf("[PID] dist  %.2f %.2f %.2f\n", s_dist_p, s_dist_i, s_dist_d);
        debug->printf("[PID] angle %.2f %.2f %.2f\n", s_angle_p, s_angle_i, s_angle_d);
    }
    else
        debug->printf("[UART] Please say again (\"%s\" is not a valid command)\n", buffer);
}

#ifdef IAM_QBOUGE
void com_handle_can(Debug *debug, CanMessenger *messenger, MotionController *mc)
#endif
#ifdef IAM_QREFLECHI
void com_handle_can(Debug *debug, CanMessenger *messenger, OrdersFIFO *orders)
#endif
{
    Message rec_msg;
    while (messenger->read_msg(&rec_msg))
    {
        switch (rec_msg.id)
        {
            case Message::MT_ping:
                debug->printf("[CAN] ping\n");
                if (messenger->send_msg_pong(rec_msg.payload.ping.data))
                    debug->printf("[CAN] send_msg_pong() failed\n");
                break;

            case Message::MT_CQB_pong:
                debug->printf("[CAN] pong (CQB)\n");
                break;

            case Message::MT_CQR_pong:
                debug->printf("[CAN] pong (CQR)\n");
                break;

            case Message::MT_CQB_MC_pos:
                // todo (pass)
                break;

            case Message::MT_CQB_MC_angle_speed:
                // todo (pass)
                break;

            case Message::MT_CQB_MC_encs:
                // todo (pass)
                break;

#ifdef IAM_QBOUGE
            case Message::MT_order:
                debug->printf("[CAN] MT_order\n");
                mc->orders_->push(rec_msg.payload.order);
                // todo ack if ok, else send error
                break;
#endif

#ifdef IAM_QREFLECHI
            case Message::MT_CQB_next_order_request:
                debug->printf("[CAN] MT_CQB_next_order_request\n");
                if (messenger->send_msg_order(*orders->front()))
                    debug->printf("[CAN] send_msg_order() failed\n");
                orders->pop(); // todo wait for ack before poping
                break;
#endif

            default:
                // todo other cases
                debug->printf("[CAN] Unhandled msg (id=%d)\n", rec_msg.id);
                break;
        }
    }
}
