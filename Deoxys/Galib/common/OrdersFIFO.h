#ifndef ORDER_H_INCLUDED
#define ORDER_H_INCLUDED

#include "common/Debug.h"
#include "common/utils.h"


/*
    Orders that are transmitted through the CAN bus.
    They are not directly usable and can be of various types (abs or rel).

    ** WARNING **
    s_order_com should be exactly 8 bytes (it can be smaller, but best is to
    keep it 8 bytes), because of the CAN protocol.
    Therefore e_order_com_type is stored in a uint8_t to keep the size fixed.
*/

typedef enum    _e_order_com_type {
    ORDER_COM_TYPE_NONE,

    ORDER_COM_TYPE_DELAY,

    ORDER_COM_TYPE_WAIT_CQB_FINISHED,
    ORDER_COM_TYPE_WAIT_CQES_FINISHED,

// movement orders (CQB)
    ORDER_COM_TYPE_ABS_POS,
    ORDER_COM_TYPE_ABS_ANGLE,
    ORDER_COM_TYPE_REL_DIST,
    ORDER_COM_TYPE_REL_ANGLE,

// robotic arm orders (CQES)
    ORDER_COM_TYPE_ARM_INIT,
    ORDER_COM_TYPE_ARM_GRAB,
    ORDER_COM_TYPE_ARM_MOVE_UP,
    ORDER_COM_TYPE_ARM_RELEASE,
    ORDER_COM_TYPE_ARM_MOVE_DOWN,

    ORDER_COM_TYPE_LAST
}               e_order_com_type;

extern const char *e2s_order_com_type[ORDER_COM_TYPE_LAST];

typedef struct  _s_order_com {
    uint8_t type;
    char padding[3];
    union {
        char            raw[4];

        float           delay;      // sec

        s_vector_int16  abs_pos;    // mm
        float           abs_angle;  // rad
        int32_t         rel_dist;   // mm
        float           rel_angle;  // rad

        uint8_t         which_arm;  // 0=nothing, 1=left, 2=right
    } order_data;
}               s_order_com;

/*
    Order types that are stored for direct use.
    They are ready to be executed and can be of a limited types.
*/

typedef enum    _e_order_exe_type {
    ORDER_EXE_TYPE_NONE,

    ORDER_EXE_TYPE_DELAY,

    ORDER_EXE_TYPE_WAIT_CQB_FINISHED,
    ORDER_EXE_TYPE_WAIT_CQES_FINISHED,

    ORDER_EXE_TYPE_POS,
    ORDER_EXE_TYPE_ANGLE,

    ORDER_EXE_TYPE_ARM_INIT,
    ORDER_EXE_TYPE_ARM_GRAB,
    ORDER_EXE_TYPE_ARM_MOVE_UP,
    ORDER_EXE_TYPE_ARM_RELEASE,
    ORDER_EXE_TYPE_ARM_MOVE_DOWN,

    ORDER_EXE_TYPE_LAST
}               e_order_exe_type;

extern const char *e2s_order_exe_type[ORDER_EXE_TYPE_LAST];

typedef struct  _s_order_exe {
    e_order_exe_type type;

    float delay;  // sec

#ifdef IAM_QBOUGE
    s_vector_int16 pos;  // mm
    float angle;  // rad
#endif

#ifdef IAM_QENTRESORT
    uint8_t which_arm;  // 0=nothing, 1=left, 2=right
#endif
}               s_order_exe;


class OrdersFIFO {
public:
    OrdersFIFO(uint8_t fifo_size);
    ~OrdersFIFO(void);

    /*
        Clear the FIFO.
    */
    void reset(void);

    void we_are_at(int16_t x, int16_t y, float angle);

    /*
        Append a new order to the list of orders to execute.
        Returns
            0 if success
            1 if error (fifo is full)
    */
    int push(s_order_com item);

    /*
        Load (pop) an order from the list into current_order_.
        Returns 0 if it found a valid other, 1 otherwise.
        If it returned 1, this function should, be called again.
    */
    int next_order_execute(void);

    /*
        Returns 1 if another order should be requested from CQR, 0 otherwise.
        It will check if there is enough space to store a new order, and check
        the duration from the last order request in order not to flood the CAN bus.
    */
    bool next_order_should_request(void);

    /*
        Remove the first elem.
    */
    void pop(void);

    /*
        Returns the first elem.
        Equivalent to this->elem(0);
        Do not free the returned pointer.
    */
    s_order_com *front(void);

    /*
        Returns the number of orders in orders_.
    */
    int size(void);

    void debug(Debug *debug);

public:
    s_order_exe current_order_;

private:
    Timer timer_;
    float last_order_executed_timestamp_;   // sec
    float last_order_request_timestamp_;    // sec

public:
    s_order_com *orders_;
private:
    uint16_t order_count_;                  // current number of elements
    uint16_t fifo_size_;                    // max number of elements
};


s_order_com OrderCom_makeNone(void);

s_order_com OrderCom_makeDelay(float delay);

s_order_com OrderCom_makeWaitCQBFinished(void);
s_order_com OrderCom_makeWaitCQESFinished(void);

s_order_com OrderCom_makeAbsPos(int16_t x, int16_t y);
s_order_com OrderCom_makeAbsAngle(float angle);
s_order_com OrderCom_makeRelDist(int32_t dist);
s_order_com OrderCom_makeRelAngle(float angle);

s_order_com OrderCom_makeArmInit(uint8_t which_arm);
s_order_com OrderCom_makeArmGrab(uint8_t which_arm);
s_order_com OrderCom_makeArmMoveUp(uint8_t which_arm);
s_order_com OrderCom_makeArmRelease(uint8_t which_arm);
s_order_com OrderCom_makeArmMoveDown(uint8_t which_arm);

#endif // #ifndef ORDER_H_INCLUDED
