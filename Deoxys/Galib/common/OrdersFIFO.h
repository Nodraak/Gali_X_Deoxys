#ifndef ORDER_H_INCLUDED
#define ORDER_H_INCLUDED

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
    ORDER_COM_TYPE_ABS_POS,
    ORDER_COM_TYPE_ABS_ANGLE,
    ORDER_COM_TYPE_REL_DIST,
    ORDER_COM_TYPE_REL_ANGLE,
    ORDER_COM_TYPE_DELAY
}               e_order_com_type;

typedef struct  _s_order_com {
    uint8_t type;
    char padding[3];
    union {
        s_vector_int16 abs_pos;  // mm
        float abs_angle;  // rad
        int32_t rel_dist;  // mm
        float rel_angle;  // rad
        float delay;  // sec

        // todo rel_pos ?
    } order_data;
}               s_order_com;

/*
    Order types that are stored for direct use.
    They are ready to be executed and can be of a limited types.
*/

typedef enum    _e_order_exe_type {
    ORDER_EXE_TYPE_NONE,
    ORDER_EXE_TYPE_POS,
    ORDER_EXE_TYPE_ANGLE,
    ORDER_EXE_TYPE_DELAY
}               e_order_exe_type;

typedef struct  _s_order_exe {
    e_order_exe_type type;
    s_vector_int16 pos;  // mm
    float angle;  // rad
    float delay;  // sec

    // TODO precision ? later -> osef. or at least, automatic in function of max speed
    // TODO max speed + max acc -> osef, bon vieux #define et roule ma poule
}               s_order_exe;


class OrdersFIFO {
public:
    OrdersFIFO(uint8_t fifo_size);
    ~OrdersFIFO(void);

    /*
        Clear the FIFO.
    */
    void reset(void);

    /*
        Append an elem.
        Returns
            0 if success
            1 if error (fifo is full)
    */
    int push(s_order_com *item);

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
        Returns the last elem.
        Equivalent to this->elem(order_count_-1);
        Do not free the returned pointer.
    */
    s_order_com *back(void);

    /*
        Returns the id^th elem of the FIFO.
        Do not free the returned pointer.
    */
    s_order_com *elem(uint8_t id);

    /*
        Returns the number of elem in the FIFO.
     */
    uint8_t size(void);

    /*
        Add a new order to the list of orders to execute.
        0 if success, 1 if error
    */

    int ordersAppendAbsPos(int16_t x, int16_t y);
    int ordersAppendAbsAngle(float angle);

    int ordersAppendRelDist(int32_t dist);
    int ordersAppendRelAngle(float angle);

    int ordersAppendDelay(float delay);

private:
    s_order_com *orders_;
    uint16_t order_count_;
    uint16_t fifo_size_;
};

#endif // #ifndef ORDER_H_INCLUDED
