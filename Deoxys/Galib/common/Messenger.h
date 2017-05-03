#ifndef MESSENGER_H_INCLUDED
#define MESSENGER_H_INCLUDED

/*
    Two classes are defined here: Message and CanMessenger.

    Message is a (CAN compatible, ie. 8 Bytes max) payload. See the
    e_message_type enum to have the list of all available messages.

    CanMessenger is used to create, send and receive Message easily over a CAN
    bus (the instanciation and sending/receiving of Message could (should?) be
    decoupled).

    To send a CAN message, use the CanMessenger::send_msg*() methods.
    To receive a CAN message, use the CanMessenger::read_msg() method.
*/

#include "mbed.h"
#include "common/OrdersFIFO.h"
#include "common/utils.h"


/*
    Can bus baud rate, in Hz.
    Limits:
        - STMF3: 1 M Hz
        - MCP2551 module: 1 M Hz
*/
#define CAN_BUS_FREQUENCY               (500*1000)

#define CAN_FRAME_SIZE                  (44+64)                                 // CAN message size in bits
#define CAN_FRAME_BUS_OCCUPATION        (1.0/CAN_BUS_FREQUENCY*CAN_FRAME_SIZE)  // time length (in sec) requiered to transmit a frame
#define CAN_MAX_MSG_PER_SEC             (1.0/CAN_FRAME_BUS_OCCUPATION)
#define CAN_MAX_MSG_PER_200Hz_FRAME     (CAN_MAX_MSG_PER_SEC/200.0)

// Size of the on_receive callbacks list
#define ON_RECEIVE_SLOT_COUNT           20

/*
    The Message is mostly used internally by the CanMessenger class, you most
    probably don't need to create instances, as CanMessenger methods returns
    Message instances.
*/
class Message {

public:

    /*
        ** Message Type (ID ie. priority) **

        This list all possible messages that can be transmitted on the CAN bus.
        The value is used as the priority for the CAN message send: lower means
        higher priority.

        Priorities are sorted in three categories:
            * high (critical: interrupt or event which must be reacted to the fastest)
            * medium (info that can be resend later)
            * low (debug infos that can be ignored without risk)

        Naming convention: example with MT_CQB_MC_pos
            MT (message type): it is a constant from the CAN frame id enum
            CQB (carte qui bouge): sender. Only this board must send this frame, otherwise bad things will happen
            MC_pos (position from motion controller): what is being sent

        Note: e_message_type is encoded on maximum 11 bits due to the CAN protocol.
    */
    typedef enum    _e_message_type {
        /*
            High (200-399)
        */

        MT_CQR_ping                 = 210,
        MT_CQB_pong                 = 211,
        MT_CQR_pong                 = 212,
        MT_CQES_pong                = 213,

        MT_CQR_match_start          = 220,
        MT_CQR_match_stop           = 221,


        MT_CQR_reset                = 310,
        MT_CQR_we_are_at            = 311,

        /*
            Medium (default) (400-599)
        */

        MT_CQR_order                = 500,

        MT_CQB_finished             = 510,
        MT_CQES_finished            = 511,

        MT_CQB_next_order_request   = 520,
        MT_CQES_next_order_request  = 521,

        /*
            Low (debug) (600-799)
        */

        MT_CQB_I_am_doing           = 650,  // current order
        MT_CQES_I_am_doing          = 651,  // current order

        MT_CQB_MC_pos_angle         = 701,
        MT_CQB_MC_speeds            = 702,
        MT_CQB_MC_pids              = 703,
        MT_CQB_MC_motors            = 704,
        MT_CQB_MC_encs              = 705,

        MT_empty                    = 2046,  // placeholder, not actually used
        MT_last                     = 2047
    }               e_message_type;

    /*
        ** Payloads definition (max 8 Bytes per CAN standard) **

        CP = CAN Payload
    */

    typedef struct {
    } s_no_payload;

    typedef s_no_payload CP_CQR_ping;
    typedef s_no_payload CP_pong;

    typedef s_no_payload CP_CQR_match_start;
    typedef s_no_payload CP_CQR_match_stop;

    typedef struct {
        s_vector_int16 pos;
        float angle;
    } CP_CQR_we_are_at;

    typedef s_no_payload CP_CQR_reset;

    typedef s_order_com CP_CQR_order;

    typedef s_no_payload CP_CQB_next_order_request;
    typedef s_no_payload CP_CQES_next_order_request;

    typedef struct {
        char i_am[4];
        e_order_exe_type order;
    } CP_I_am_doing;

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
        ** Actual CAN Message (payload) **
    */

    typedef union {
        char raw_data[8];

        CP_CQR_ping                 CQR_ping;
        CP_pong                     pong;

        CP_CQR_match_start          CQR_match_start;
        CP_CQR_match_stop           CQR_match_stop;

        CP_CQR_reset                CQR_reset;
        CP_CQR_we_are_at            CQR_we_are_at;

        CP_CQR_order                CQR_order;
        CP_CQB_next_order_request   CQB_next_order_request;
        CP_CQES_next_order_request  CQES_next_order_request;

        CP_I_am_doing               I_am_doing;

        CP_CQB_MC_pos_angle         CQB_MC_pos_angle;
        CP_CQB_MC_speeds            CQB_MC_speeds;
        CP_CQB_MC_pids              CQB_MC_pids;
        CP_CQB_MC_motors            CQB_MC_motors;
        CP_CQB_MC_encs              CQB_MC_encs;
    } u_payload;

    /*
        Class Methods
    */

    Message(void);  // Receive. Fills the attributes with default placeholders.
    Message(e_message_type id_, unsigned int len_, u_payload payload_);  // Send.

    /*
        Class Attributes
    */

    e_message_type id;  // lower = higher priority
    uint8_t len;  // max len of a CAN message is 8 bytes
    u_payload payload;
};


const char *e2s_message_type(Message::e_message_type msg);


/*
    The CanMessenger class is used to send and receive messages over the CAN bus.

    The methods are wrappers that create the proper Message instance and send it
    using the CanMessenger::send_msg() method.

    The class could easily be splitted into methods to create a Message and
    a Medium class (CanMedium, UartMedium) to send them.
*/
class CanMessenger {
public:
    CanMessenger(void);

    /*
        Silent mode disable write to the bus. We can still read msg.
    */
    void set_silent(bool enable);

    /*
        Enable silent mode for a moment (few ms) to let the other boards
        initialize their CAN bus (CanMessenger)
    */
    void leave_the_bus_for_a_moment(void);

    /*
        Tries to read from the CAN receive buffers.
        If a CAN message is available, fill msg and returns 1;
        If no CAN message is available, returns 0 (msg might be overwritten with
        garbage though).
    */
    int read_msg(Message *msg);

#ifdef IAM_QREFLECHI
    int send_msg_CQR_ping(void);
#endif
    int send_msg_pong(void);

#ifdef IAM_QREFLECHI
    int send_msg_CQR_match_start(void);
    int send_msg_CQR_match_stop(void);

    int send_msg_CQR_we_are_at(int16_t x, int16_t y, float angle);
    int send_msg_CQR_reset(void);

    int send_msg_CQR_order(s_order_com order);
#endif
#ifdef IAM_QBOUGE
    int send_msg_CQB_finished(void);
    int send_msg_CQB_next_order_request(void);
#endif
#ifdef IAM_QENTRESORT
    int send_msg_CQES_finished(void);
    int send_msg_CQES_next_order_request(void);
#endif

    int send_msg_I_am_doing(e_order_exe_type order);

#ifdef IAM_QBOUGE
    int send_msg_CQB_MC_pos_angle(float x, float y, float angle);
    int send_msg_CQB_MC_speeds(float speed, float speed_ang);
    int send_msg_CQB_MC_pids(float dist, float angle);
    int send_msg_CQB_MC_motors(float pwm_l, float pwm_r);
    int send_msg_CQB_MC_encs(int32_t enc_l, int32_t enc_r);
#endif

private:
    /*
        Returns:
            0: Success
            1: Error
    */
    int send_msg(Message msg);

public:
    /*
        Specify a function to execute when a specific CAN message is received.
        Returns:
            0: The callback was successfully registered
            1: An error happened (most probably the callback list is full)
    */
    int on_receive_add(Message::e_message_type type, Callback<void(void*)> cb);

private:

    // On Receive callbacks
    int                     or_count_;
    Message::e_message_type or_types_[ON_RECEIVE_SLOT_COUNT];
    Callback<void (void*)>  or_callbacks_[ON_RECEIVE_SLOT_COUNT];

    // Medium
    CAN can_;
};

#endif // MESSENGER_H_INCLUDED
