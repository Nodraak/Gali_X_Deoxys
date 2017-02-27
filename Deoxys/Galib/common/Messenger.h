#ifndef MESSENGER_H_INCLUDED
#define MESSENGER_H_INCLUDED

#include "common/OrdersFIFO.h"
#include "common/utils.h"


/*
    Can bus baud rate, in Hz.
    Limits:
        - STMF3: 1 M Hz
        - MCP2551 module: 1 M Hz
*/
#define CAN_BUS_FREQUENCY (500*1000)


/*
    The Message is mostly used internally by the CanMessenger class, you most
    probably don't need to create instances, as CanMessenger methods returns
    Message instances.
*/
class Message {

public:

    /*
        ** Message Type (ID) **

        This list all possible messages that can be transmitted on the CAN bus.
        The value is used as the priority for the CAN message send: lower means
        higher priority.

        Priorities are sorted in three categories:
            * high (critical: interrupt or event which must be reacted to the fastest)
            * medium (info that can be resend later)
            * low (debug infos that can be ignored without risk)

        Naming convention: example with MT_CQB_MC_pos
            MT: message type
            CQB: carte qui bouge
            MC: class (motion controller)
            pos: info (most likely attribute of the class)

    */
    // todo priorities
    typedef enum    _e_message_type {
        /*
            High
        */


        /*
            Medium (default)
        */

        MT_ping,
        MT_CQB_pong,
        MT_CQR_pong,

        MT_order,
        MT_CQB_next_order_request,

        /*
            Low (debug)
        */

        MT_CQB_MC_pos,
        MT_CQB_MC_angle_speed,
        MT_CQB_MC_encs,
        MT_CQB_MC_pids,
        MT_CQB_MC_motors,

        MT_empty,
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

    typedef s_order_com CP_order;

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

    typedef struct {
        uint8_t count;
        char padding[7];
    } CP_CQB_next_order_request;

    /*
        ** CAN Message **
    */

    typedef union {
        char raw_data[8];

        CP_ping ping;
        CP_pong pong;

        CP_order order;

        CP_CQB_MC_pos CQB_MC_pos;

        CP_CQB_MC_angle_speed CQB_MC_angle_speed;
        CP_CQB_MC_encs CQB_MC_encs;
        CP_CQB_MC_pids CQB_MC_pids;
        CP_CQB_MC_motors CQB_MC_motors;

        CP_CQB_next_order_request CQB_next_order_request;

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


/*
    The CanMessenger class is used to send messages over the CAN bus. The
    methods are wrappers that create the proper Message instance and send it
    using the CanMessenger::send_msg() private method.
*/
class CanMessenger {
public:
    CanMessenger(void);

    int read_msg(Message *msg);

    // data can be NULL if not payload is to be send with the pin/pong message.
    int send_msg_ping(char data[8]);
    int send_msg_pong(char data[8]);

    int send_msg_order(s_order_com order);

    int send_msg_CQB_MC_pos(float x, float y);
    int send_msg_CQB_MC_angle_speed(float angle, float speed);
    int send_msg_CQB_MC_encs(int32_t enc_l, int32_t enc_r);
    int send_msg_CQB_MC_pids(float dist, float angle);
    int send_msg_CQB_MC_motors(float pwm_l, float pwm_r);

    int send_msg_CQB_next_order_request(uint8_t count);

private:
    /*
        Returns:
            0 Success
            1 Error
    */
    int send_msg(Message msg);

private:
    CAN can_;
};

#endif // MESSENGER_H_INCLUDED
