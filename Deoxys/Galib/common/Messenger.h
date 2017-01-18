#ifndef MESSENGER_H_INCLUDED
#define MESSENGER_H_INCLUDED

#include "common/OrdersFIFO.h"
#include "common/utils.h"

class Message {

public:

    /*
        ** Message Type (ID) **

        This list all possible messages that can be transmitted on the CAN bus.
        The value is used as the priority for the CAN message send: lower means
        higher priority.

        Naming convention: example with MT_CQB_MC_pos
            MT: message type
            CQB: carte qui bouge
            MC: class (motion controller)
            pos: info (most likely attribute of the class)
    */
    typedef enum    _e_message_type {
        MT_empty,
        MT_ping,

        /*
            From CQBOUGE
        */
        // info
        MT_CQB_pong,
        MT_CQB_MC_pos,
        // debug
        MT_CQB_MC_angle_speed,
        MT_CQB_MC_encs,
        MT_CQB_MC_pids,
        MT_CQB_MC_motors,
        MT_CQB_MC_order,

        /*
            From CQREFLECHI
        */
        // info
        MT_CQR_pong,
    }               e_message_type;

    /*
        ** Payloads **

        CP = CAN Payload
    */

    typedef struct {
        char data[8];
    } CP_ping;

    typedef struct {
        char data[8];
    } CP_pong;

    /*
        From CQBOUGE
    */

    // info

    typedef struct {
        s_vector_float pos;
    } CP_CQB_MC_pos;

    typedef struct {
        float angle, speed;
    } CP_CQB_MC_angle_speed;

    // debug

    typedef struct {
        int32_t enc_l, enc_r;
    } CP_CQB_MC_encs;

    typedef struct {
        float dist, angle;
    } CP_CQB_MC_pids;

    typedef struct {
        float pwm_l;
        float pwm_r;
    } CP_CQB_MC_motors;

    typedef s_order_com CP_CQB_MC_order;

    /*
        ** CAN Message **
    */

    typedef union {
        char raw_data[8];

        CP_ping ping;
        CP_pong pong;

        CP_CQB_MC_pos CQB_MC_pos;

        CP_CQB_MC_angle_speed CQB_MC_angle_speed;
        CP_CQB_MC_encs CQB_MC_encs;
        CP_CQB_MC_pids CQB_MC_pids;
        CP_CQB_MC_motors CQB_MC_motors;
        CP_CQB_MC_order CQB_MC_order;

    } u_payload;

    /*
        Class Methods
    */

    Message(void);  // constructor for receiving msg
    Message(e_message_type id_, unsigned int len_, u_payload payload_);  // constructor to send msg

    /*
        Class Attributes
    */

    e_message_type id;  // lower = higher priority
    // todo id:11 -> width of 'Message::id' exceeds its type
    uint8_t len;  // max len of a CAN message is 8 bytes
    u_payload payload;
};

class CanMessenger {
public:
    CanMessenger(void);

    int read_msg(Message *msg);

    int send_msg_ping(char data[8]);
    int send_msg_pong(char data[8]);

    int send_msg_CQB_MC_pos(float x, float y);
    int send_msg_CQB_MC_angle_speed(float angle, float speed);
    int send_msg_CQB_MC_encs(int32_t enc_l, int32_t enc_r);
    int send_msg_CQB_MC_pids(float dist, float angle);
    int send_msg_CQB_MC_motors(float pwm_l, float pwm_r);

    int send_msg_CQB_MC_order(s_order_com order);

private:
    int send_msg(Message msg);

private:
    CAN can_;
};

#endif // MESSENGER_H_INCLUDED

