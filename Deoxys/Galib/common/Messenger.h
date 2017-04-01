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

#define CAN_FRAME_BUS_OCCUPATION        (1.0/CAN_BUS_FREQUENCY * (44+64))  // time in sec that a frame requiert to be transmitted
#define CAN_MAX_MSG_PER_SEC             (1.0/CAN_FRAME_BUS_OCCUPATION)
#define CAN_MAX_MSG_PER_ASSERV_FRAME    (CAN_MAX_MSG_PER_SEC/ASSERV_FPS)
// todo print theses stats

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

        Note: e_message_type is encoded on 11 bits due to the CAN protocol.
    */
    typedef enum    _e_message_type {
        /*
            High (200-399)
        */

        MT_ping                     = 210,
        MT_CQB_pong                 = 211,
        MT_CQR_pong                 = 212,
        MT_CQES_pong                = 213,

        MT_CQR_match_start          = 220,
        MT_CQR_match_stop           = 221,


        MT_CQR_reset                = 310,

        MT_we_are_at                = 311,

        /*
            Medium (default) (400-599)
        */

        MT_order                    = 500,

        MT_CQB_finished             = 510,
        MT_CQES_finished            = 511,

        MT_CQB_next_order_request   = 520,

        /*
            Low (debug) (600-799)
        */

        MT_CQB_MC_pos_angle         = 701,
        MT_CQB_MC_speeds            = 702,
        MT_CQB_MC_pids              = 703,
        MT_CQB_MC_motors            = 704,
        MT_CQB_MC_encs              = 705,

        MT_empty                    = 800,  // placeholder, not actually used
    }               e_message_type;

    /*
        ** Payloads **

        CP = CAN Payload
    */

    typedef struct {
    } s_no_payload;

    typedef s_no_payload CP_ping;
    typedef s_no_payload CP_pong;

    typedef s_no_payload CP_CQR_match_start;
    typedef s_no_payload CP_CQR_match_stop;

    typedef struct {
        s_vector_int16 pos;
        float angle;
    } CP_CQR_we_are_at;

    typedef s_no_payload CP_CQR_reset;

    typedef s_order_com CP_order;

    typedef struct {
        uint8_t count;
    } CP_CQB_next_order_request;

    typedef CP_CQR_we_are_at CP_CQB_MC_pos_angle;

    typedef struct {
        float speed;
        float speed_ang;
    } CP_CQB_MC_speeds;

    typedef struct {
        float dist, angle;
    } CP_CQB_MC_pids;

    typedef struct {
        float pwm_l;
        float pwm_r;
    } CP_CQB_MC_motors;

    typedef struct {
        int32_t enc_l, enc_r;
    } CP_CQB_MC_encs;

    /*
        ** CAN Message **
    */

    typedef union {
        char raw_data[8];

        CP_ping                     ping;
        CP_pong                     pong;

        CP_CQR_match_start          CQR_match_start;
        CP_CQR_match_stop           CQR_match_stop;

        CP_CQR_reset                CQR_reset;
        CP_CQR_we_are_at            CQR_we_are_at;

        CP_order                    order;
        CP_CQB_next_order_request   CQB_next_order_request;

        CP_CQB_MC_pos_angle         CQB_MC_pos_angle;
        CP_CQB_MC_speeds            CQB_MC_speeds;
        CP_CQB_MC_pids              CQB_MC_pids;
        CP_CQB_MC_motors            CQB_MC_motors;
        CP_CQB_MC_encs              CQB_MC_encs;
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

    void set_silent(bool enable);

    int read_msg(Message *msg);

    // data can be NULL if not payload is to be send with the pin/pong message.
    int send_msg_ping(void);
    int send_msg_pong(void);

    int send_msg_CQR_match_start(void);
    int send_msg_CQR_match_stop(void);

    int send_msg_we_are_at(int16_t x, int16_t y, float angle);
    int send_msg_CQR_reset(void);

    int send_msg_order(s_order_com order);
    int send_msg_CQB_finished(void);
    int send_msg_CQES_finished(void);
    int send_msg_CQB_next_order_request(uint8_t count);

    int send_msg_CQB_MC_pos_angle(float x, float y, float angle);
    int send_msg_CQB_MC_speeds(float speed, float speed_ang);
    int send_msg_CQB_MC_pids(float dist, float angle);
    int send_msg_CQB_MC_motors(float pwm_l, float pwm_r);
    int send_msg_CQB_MC_encs(int32_t enc_l, int32_t enc_r);

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
