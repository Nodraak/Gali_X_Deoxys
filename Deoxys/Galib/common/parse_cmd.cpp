
#include <cstring>

#include "common/Debug.h"
#include "common/Messenger.h"
#include "common/OrdersFIFO.h"

#if defined(IAM_QBOUGE)
#include "QBouge/MotionController.h"
#elif defined(IAM_QREFLECHI)
#include "common/Messenger.h"
#elif defined(IAM_QENTRESORT)
#include "QEntreQSort/Actuators.h"
#endif

#include "common/parse_cmd.h"


#if defined(IAM_QBOUGE)
void parse_cmd(Debug *debug, MotionController *mc)
#elif defined(IAM_QREFLECHI)
void parse_cmd(Debug *debug, CanMessenger *messenger, EventQueue *queue)
#elif defined(IAM_QENTRESORT)
void parse_cmd(Debug *debug, OrdersFIFO *orders, Actuators *actuators)
#endif
{
    char buffer[BUFFER_SIZE] = "";
    char *cmd = NULL;

    if (debug->get_line(buffer, BUFFER_SIZE) == -1)
        return;

    debug->printf("[parse_cmd] %s\n", buffer);

    cmd = strtok(buffer,  " ");
    if (cmd == NULL)
        return;

    if (strcmp(cmd, "h") == 0)
    {
        debug->printf("Available commands:\n");

        debug->printf("\tseq l: seq left\n");
        debug->printf("\tseq r: seq right\n");
        debug->printf("\tprint: print actuator settings\n");
        debug->printf("\tact: actuator cmd\n");
        debug->printf("\t\tact l flap e: extend actuator left flap\n");
        debug->printf("\t\tact l flap e 0.10: set actuator left flap extended to 0.10\n");

        debug->printf("\tmc\n");
        debug->printf("\t\tmc dist <d>: move order\n");
        debug->printf("\t\tmc angle <a>: angle order\n");
        debug->printf("\tpid: pid setting cmd\n");
        debug->printf("\t\tpid dist p 5.5\n");
        debug->printf("\t\tpid angle i 1.5\n");

        debug->printf("--\n");
    }
    else if (strcmp(cmd, "seq") == 0)
    {
        t_act side = 0;

        cmd = strtok(NULL,  " ");
        if (cmd == NULL)
        {
            debug->printf("Error: seq: expected side\n");
            return;
        }

        if (strcmp(cmd, "l") == 0)
            side = ACT_SIDE_LEFT;
        else if (strcmp(cmd, "r") == 0)
            side = ACT_SIDE_RIGHT;
        else
        {
            debug->printf("Error: seq: unknown side\n");
            return;
        }

#if defined(IAM_QREFLECHI)
        queue->call_in(0, callback(messenger, &CanMessenger::send_msg_CQR_order),
            OrderCom_makeSequence(ORDER_COM_TYPE_SEQ_ARM_INIT, side)
        );
        queue->call_in(100, callback(messenger, &CanMessenger::send_msg_CQR_order),
            OrderCom_makeSequence(ORDER_COM_TYPE_SEQ_ARM_GRAB, side)
        );
        queue->call_in(200, callback(messenger, &CanMessenger::send_msg_CQR_order),
            OrderCom_makeSequence(ORDER_COM_TYPE_SEQ_ARM_MOVE_UP, side)
        );
        queue->call_in(300, callback(messenger, &CanMessenger::send_msg_CQR_order),
            OrderCom_makeSequence(ORDER_COM_TYPE_SEQ_ARM_RELEASE, side)
        );
        queue->call_in(400, callback(messenger, &CanMessenger::send_msg_CQR_order),
            OrderCom_makeSequence(ORDER_COM_TYPE_SEQ_ARM_MOVE_DOWN, side)
        );
        queue->call_in(500, callback(messenger, &CanMessenger::send_msg_CQR_order),
            OrderCom_makeSequence(ORDER_COM_TYPE_SEQ_FLAP, side)
        );
        queue->call_in(600, callback(messenger, &CanMessenger::send_msg_CQR_order),
            OrderCom_makeSequence(ORDER_COM_TYPE_SEQ_PROGRADE_DISPENSER, side)
        );
#elif defined(IAM_QENTRESORT)
        orders->append(OrderCom_makeSequence(ORDER_COM_TYPE_SEQ_ARM_INIT, side));
        orders->append(OrderCom_makeSequence(ORDER_COM_TYPE_SEQ_ARM_GRAB, side));
        orders->append(OrderCom_makeSequence(ORDER_COM_TYPE_SEQ_ARM_MOVE_UP, side));
        orders->append(OrderCom_makeSequence(ORDER_COM_TYPE_SEQ_ARM_RELEASE, side));
        orders->append(OrderCom_makeSequence(ORDER_COM_TYPE_SEQ_ARM_MOVE_DOWN, side));
        orders->append(OrderCom_makeSequence(ORDER_COM_TYPE_SEQ_FLAP, side));
        orders->append(OrderCom_makeSequence(ORDER_COM_TYPE_SEQ_PROGRADE_DISPENSER, side));
#endif
    }
#if defined(IAM_QENTRESORT)
    else if (strcmp(cmd, "print") == 0)
    {
        actuators->print(debug, 0);
    }
#endif
    else if (strcmp(cmd, "act") == 0)
    {
        char dst_side[20], dst_act[20], dst_conf[20];

        cmd = strtok(NULL,  " ");
        if (cmd == NULL)
        {
            debug->printf("Error: act: expected side\n");
            return;
        }
        strcpy(dst_side, cmd);

        cmd = strtok(NULL,  " ");
        if (cmd == NULL)
        {
            debug->printf("Error: act: expected act\n");
            return;
        }
        strcpy(dst_act, cmd);

        cmd = strtok(NULL,  " ");
        if (cmd == NULL)
        {
            debug->printf("Error: act: expected conf\n");
            return;
        }
        strcpy(dst_conf, cmd);

        // parse

        t_act act = 0;

        if (strcmp(dst_side, "l") == 0)
            act |= ACT_SIDE_LEFT;
        else if (strcmp(dst_side, "r") == 0)
            act |= ACT_SIDE_RIGHT;
        else
            debug->printf("Error: act: unknown side (%s)\n", dst_side);

        if (strcmp(dst_act, "height") == 0)
            act |= ACT_ACTUATOR_HEIGHT;
        else if (strcmp(dst_act, "vert") == 0)
            act |= ACT_ACTUATOR_VERT;
        else if (strcmp(dst_act, "horiz") == 0)
            act |= ACT_ACTUATOR_HORIZ;
        else if (strcmp(dst_act, "clamp") == 0)
            act |= ACT_ACTUATOR_CLAMP;
        else if (strcmp(dst_act, "pump") == 0)
            act |= ACT_ACTUATOR_PUMP;
        else if (strcmp(dst_act, "flap") == 0)
            act |= ACT_ACTUATOR_FLAP;
        else if (strcmp(dst_act, "prog") == 0)
            act |= ACT_ACTUATOR_PROG;
        else
            debug->printf("Error: act: unknown actuator (%s)\n", dst_act);

        if (strcmp(dst_conf, "e") == 0)
            act |= ACT_STATE_EXTENDED;
        else if (strcmp(dst_conf, "n") == 0)
            act |= ACT_STATE_NEUTRAL;
        else if (strcmp(dst_conf, "r") == 0)
            act |= ACT_STATE_RETRACTED;
        else
            debug->printf("Error: act: unknown conf (%s)\n", dst_conf);

        // exe

        cmd = strtok(NULL,  " ");
        if (cmd == NULL)
        {
#if defined(IAM_QREFLECHI)
            messenger->send_msg_CQR_order(OrderCom_makeActuator(act));
#elif defined(IAM_QENTRESORT)
            actuators->activate(act);
#endif
        }
        else
        {
#if defined(IAM_QREFLECHI)
            messenger->send_msg_CQR_settings_CQES(act, (float)atof(cmd));
#elif defined(IAM_QENTRESORT)
            actuators->set(act, atof(cmd));
            actuators->print(debug, 0);
#endif
        }
    }
    else if (strcmp(cmd, "mc") == 0)
    {
        char dst_action[20], dst_param[20];

        cmd = strtok(NULL,  " ");
        if (cmd == NULL)
        {
            debug->printf("Error: mc: expected action\n");
            return;
        }
        strcpy(dst_action, cmd);

        cmd = strtok(NULL,  " ");
        if (cmd == NULL)
        {
            debug->printf("Error: mc: expected param\n");
            return;
        }
        strcpy(dst_param, cmd);

        // parse + exe

        if (strcmp(dst_action, "dist") == 0)
        {
#if defined(IAM_QREFLECHI)
            messenger->send_msg_CQR_order(OrderCom_makeRelDist(atof(dst_param)));
#endif
        }
        else if (strcmp(dst_action, "angle") == 0)
        {
#if defined(IAM_QREFLECHI)
            messenger->send_msg_CQR_order(OrderCom_makeAbsAngle(DEG2RAD(atof(dst_param))));
#endif
        }
        else
            debug->printf("Error: act: unknown action (%s)\n", dst_action);
    }
    else if (strcmp(cmd, "pid") == 0)
    {
        char dst_pid[20], dst_k[20];

        cmd = strtok(NULL,  " ");
        if (cmd == NULL)
        {
            debug->printf("Error: pid: expected pid (dist or angle)\n");
            return;
        }
        strcpy(dst_pid, cmd);

        cmd = strtok(NULL,  " ");
        if (cmd == NULL)
        {
            debug->printf("Error: pid: expected k (p, i or d)\n");
            return;
        }
        strcpy(dst_k, cmd);

        // parse

        e_cqb_setting cqb_setting;

        if (strcmp(dst_pid, "dist") == 0)
        {
            if (strcmp(dst_k, "p") == 0)
                cqb_setting = CQB_SETTING_PID_DIST_P;
            else if (strcmp(dst_k, "i") == 0)
                cqb_setting = CQB_SETTING_PID_DIST_I;
            else if (strcmp(dst_k, "d") == 0)
                cqb_setting = CQB_SETTING_PID_DIST_D;
            else
            {
                debug->printf("Error: act: expected k (p, i or d) (%s)\n", dst_k);
                return;
            }
        }
        else if (strcmp(dst_pid, "angle") == 0)
        {
            if (strcmp(dst_k, "p") == 0)
                cqb_setting = CQB_SETTING_PID_ANGLE_P;
            else if (strcmp(dst_k, "i") == 0)
                cqb_setting = CQB_SETTING_PID_ANGLE_I;
            else if (strcmp(dst_k, "d") == 0)
                cqb_setting = CQB_SETTING_PID_ANGLE_D;
            else
            {
                debug->printf("Error: act: expected k (p, i or d) (%s)\n", dst_k);
                return;
            }
        }
        else
        {
            debug->printf("Error: act: expected pid (dist or angle) (%s)\n", dst_pid);
            return;
        }

        // exe

        cmd = strtok(NULL,  " ");
        if (cmd == NULL)
        {
            debug->printf("Error: pid: expected val\n");
            return;
        }

#if defined(IAM_QREFLECHI)
        messenger->send_msg_CQR_settings_CQB(cqb_setting, atof(cmd));
#elif defined(IAM_QBOUGE)
        mc->set(cqb_setting, atof(cmd));
        mc->print(debug);
#endif
    }
    else
    {
        debug->printf("Error: unknown cmd\n");
    }

#if 0
    else if (strncmp(buffer, "aws", 3) == 0)
    {
cmd = strtok(NULL,  " ");
if (cmd == NULL)
    return;

        char *ptr = buffer+3+1;
        int a = 0;

        a = atoi(ptr);

        debug->printf("set speed %d\n", a);
        actuators->left_.arm_.write_speed_all(a);
        actuators->right_.arm_.write_speed_all(a);
    }
#endif

}
