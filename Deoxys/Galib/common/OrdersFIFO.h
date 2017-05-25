#ifndef ORDER_H_INCLUDED
#define ORDER_H_INCLUDED

/*
    This file define everything needed for Orders management:
        * struct to store an Order:
            * s_order_com: less than 8 bytes struct that can be transmitted over CAN
            * s_order_exe: expanded order that is ready to be executed
        * OrdersFIFO: a list of orders with various other things to make it easy
            to add or remove an order
        * OrderCom_make*() function: used to create and Order that can be send
            with CanMessenger::send_msg_order() or stored in the OrdersFIFO class.
*/

#include "common/Debug.h"
#include "common/utils.h"


// delay not to flood the CAN bus requesting next order (if CQR does not have more orders)
#define REQUEST_NEXT_ORDER_DELAY 0.050  // sec


/*
    Actuator parameters

    All these define can be OR'ed to build a complete configuration

    32 bits:
        state (8 bits): what to do
        actuator (16 bits): which actuator to move
        side (8 bits): which side (actuator group) are we talking about
*/

// state: 8 bits
#define ACT_STATE_EXTENDED       (0x1 << 0)
#define ACT_STATE_NEUTRAL        (0x1 << 1)
#define ACT_STATE_RETRACTED      (0x1 << 2)

// alternate API
#define ACT_STATE_OPEN           ACT_STATE_EXTENDED
#define ACT_STATE_CLOSED         ACT_STATE_RETRACTED
#define ACT_STATE_ON             ACT_STATE_EXTENDED
#define ACT_STATE_OFF            ACT_STATE_RETRACTED

// actuator: 16 bits
#define ACT_ACTUATOR_HEIGHT     (0x1 << 8)
#define ACT_ACTUATOR_VERT       (0x1 << 9)
#define ACT_ACTUATOR_HORIZ      (0x1 << 10)
#define ACT_ACTUATOR_CLAMP      (0x1 << 11)
#define ACT_ACTUATOR_PUMP       (0x1 << 12)
#define ACT_ACTUATOR_FLAP       (0x1 << 13)
#define ACT_ACTUATOR_PROG       (0x1 << 14)
#define ACT_ACTUATOR_COLOR      (0x1 << 15)

// side: 8 bits
#define ACT_SIDE_LEFT           (0x1 << 24)
#define ACT_SIDE_RIGHT          (0x1 << 25)

typedef uint32_t t_act;  // Warning: 32 bits max, this is transmitted as a CAN payload with other things


/*
    Order com

    Orders that are transmitted through the CAN bus.
    They are not directly usable, as they are optimized for size. They can be of
    various types (abs pos or rel pos, angle, delay, ...).

    ** WARNING **
    s_order_com should be exactly 8 bytes (it can be smaller, but best is to
    keep it 8 bytes), because of the CAN protocol.
    Therefore e_order_com_type is stored in a uint8_t to keep the size fixed.

    Note: they are common to all boards.
*/

typedef enum    _e_order_com_type {
    ORDER_COM_TYPE_NONE,

    ORDER_COM_TYPE_DELAY,                   // param: delay

// Synchronization orders
    ORDER_COM_TYPE_WAIT_CQB_FINISHED,
    ORDER_COM_TYPE_WAIT_CQES_FINISHED,
    ORDER_COM_TYPE_WAIT_CQR_FINISHED,
// wait_CQB_im_finished (what: move order, reach spec pos, ... ?)
// wait_CQES_im_finished (idem) (what: taking cylinder, loading cylinder, ..., valve, ... ?)

// Movement orders (CQB)
    ORDER_COM_TYPE_MOV_ABS_POS,             // param: x, y
    ORDER_COM_TYPE_MOV_ABS_ANGLE,           // param: angle
    ORDER_COM_TYPE_MOV_REL_DIST,            // param: dist
    ORDER_COM_TYPE_MOV_REL_ANGLE,           // param: angle
    // todo rel pos ?

// Actuators orders (CQES)
    ORDER_COM_TYPE_ACTUATOR,                // param: full t_act configuration

    ORDER_COM_TYPE_SEQ_ARM_INIT,            // param: side
    ORDER_COM_TYPE_SEQ_ARM_GRAB,            // param: side
    ORDER_COM_TYPE_SEQ_ARM_MOVE_UP,         // param: side
    ORDER_COM_TYPE_SEQ_ARM_RELEASE,         // param: side
    ORDER_COM_TYPE_SEQ_ARM_MOVE_DOWN,       // param: side
    ORDER_COM_TYPE_SEQ_FLAP,                // param: side
    ORDER_COM_TYPE_SEQ_PROGRADE_DISPENSER,  // param: side
    ORDER_COM_TYPE_SEQ_COLOR,               // param: side
// todo order radial_dispenser

    ORDER_COM_TYPE_LAST
}               e_order_com_type;

extern const char *e2s_order_com_type[ORDER_COM_TYPE_LAST];

typedef struct  _s_order_com {
    uint8_t type;                   // type is e_order_com_type but we force it to 8 bits
    char padding[3];  // todo uint16_t order_id (auto increment)
    union {
        char            raw[4];

        float           delay;      // sec

        s_vector_int16  abs_pos;    // mm
        float           abs_angle;  // rad
        int32_t         rel_dist;   // mm
        float           rel_angle;  // rad

        t_act           act_param;
    } order_data;  // Warning: 32 bits max
}               s_order_com;

/*
    Order exe

    Order that are ready for direct use.
    They are ready to be executed and can be of a limited types.

    Note: they are board-specific.
*/

typedef enum    _e_order_exe_type {
    ORDER_EXE_TYPE_NONE,

    ORDER_EXE_TYPE_DELAY,                   // param: delay

// Synchronization orders
    ORDER_EXE_TYPE_WAIT_CQB_FINISHED,
    ORDER_EXE_TYPE_WAIT_CQES_FINISHED,
    ORDER_EXE_TYPE_WAIT_CQR_FINISHED,

// Movement orders (CQB)
    ORDER_EXE_TYPE_MOV_POS,                 // param: x, y
    ORDER_EXE_TYPE_MOV_ANGLE,               // param: angle

// Actuators orders (CQES)
    ORDER_EXE_TYPE_ACTUATOR,
    ORDER_EXE_TYPE_COLOR_WAIT,

    ORDER_EXE_TYPE_LAST
}               e_order_exe_type;

extern const char *e2s_order_exe_type[ORDER_EXE_TYPE_LAST];

typedef struct  _s_order_exe {
    e_order_exe_type type;

    float delay;        // sec

#ifdef IAM_QBOUGE
    s_vector_int16 pos; // mm
    float angle;        // rad
#endif

#ifdef IAM_QENTRESORT
    t_act act_param;
#endif
}               s_order_exe;


class OrdersFIFO {
public:
    OrdersFIFO(uint8_t fifo_size);
    ~OrdersFIFO(void);

    /*
        Clear all the stored orders.
    */
    void reset(void);

    /*
        Specify where the robot currently is.
        This is used to properly expand relative orders (pos and angle) to
        absolute orders (pos and angle) when they are executed.
    */
    void we_are_at(int16_t x, int16_t y, float angle);

    /*
        Add a new order to the list of orders to execute.
        Returns
            0 if success
            1 if error (most probably the list is full)
    */
    int append(s_order_com item);
    int prepend(s_order_com item);

    /*
        Pop (remove) the first an order of the FIFO list and use it to overwrite
        current_order_.
    */
    void pop(void);

    /*
        Returns the first elem.
        Equivalent to this->elem(0);
        Do not free the returned pointer as it is managed internally by the class.
    */
    s_order_com *front(void);

    /*
        Returns the number of orders in the FIFO list orders_.
    */
    int size(void);

    /*
        Load (pop) an order from the list into current_order_.
        Returns:
            0: a valid order has been found
            1: the order is not to be executed (a CQB order while we are on the CQES for ex)
                The function should be called again, to load the next order
    */
    int next_order_execute(void);

    /*
        Returns 1 if another order should be requested from CQR, 0 otherwise.

        It will check if there is enough space to store a new order, and check
        the duration from the last order request in order not to flood the CAN bus.
    */
    bool next_order_should_request(void);

    /*
        Print some debug info: mainly the current order and all the orders in
        the FIFO list.
    */
    void debug(Debug *debug);

public:
    s_order_exe current_order_;

private:
    Timer timer_;
    float last_order_executed_timestamp_;   // sec
    float request_next_order_at_timestamp_; // sec

    s_order_com *orders_;
    uint16_t order_count_;                  // current number of elements
    uint16_t fifo_size_;                    // max number of elements
};


s_order_com OrderCom_makeNone(void);

s_order_com OrderCom_makeDelay(float delay);

s_order_com OrderCom_makeWaitCQBFinished(void);
s_order_com OrderCom_makeWaitCQESFinished(void);
s_order_com OrderCom_makeWaitCQRFinished(void);

s_order_com OrderCom_makeAbsPos(s_vector_int16 pos);
s_order_com OrderCom_makeAbsPos(int16_t x, int16_t y);
s_order_com OrderCom_makeAbsAngle(float angle);
// todo s_order_com OrderCom_makeAngleToPos(s_vector_int16 pos);
s_order_com OrderCom_makeRelDist(int32_t dist);
s_order_com OrderCom_makeRelAngle(float angle);

s_order_com OrderCom_makeActuator(t_act act_param);
s_order_com OrderCom_makeSequence(e_order_com_type seq, t_act act_param);

#endif // #ifndef ORDER_H_INCLUDED
