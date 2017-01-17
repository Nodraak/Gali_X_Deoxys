#ifndef ORDER_H_INCLUDED
#define ORDER_H_INCLUDED

#include "common/utils.h"

typedef enum    _e_order_type {
    ORDER_TYPE_NONE,
    ORDER_TYPE_POS,
    ORDER_TYPE_ANGLE,
    ORDER_TYPE_DELAY
}               e_order_type;

typedef struct  _s_order {
    e_order_type type;
    s_vector_int16 pos;
    float angle;        // radians -> todo int16 deg ?
    float delay;        // sec -> todo uint16 ms

    // TODO precision ? later -> osef. or at least, automatic in function of max speed
    // TODO max speed + max acc -> osef, bon vieux #define et roule ma poule
}               s_order;


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
    int push(s_order *item);

    /*
        Remove the first elem.
    */
    void pop(void);

    /*
        Returns the first elem.
        Equivalent to this->elem(0);
        Do not free the returned pointer.
    */
    s_order *front(void);

    /*
        Returns the last elem.
        Equivalent to this->elem(order_count_-1);
        Do not free the returned pointer.
    */
    s_order *back(void);

    /*
        Returns the id^th elem of the FIFO.
        Do not free the returned pointer.
    */
    s_order *elem(uint8_t id);

    /*
        Returns the number of elem in the FIFO.
     */
    uint8_t size(void);

    /*
        Add a new order to the list of orders to execute.
        0 if success, 1 if error
    */
private:
    int ordersAppend(e_order_type type, int16_t x, int16_t y, float angle, uint16_t delay);
public:
    int ordersAppendAbsPos(int16_t x, int16_t y);
    int ordersAppendAbsAngle(float angle);
    int ordersAppendAbsDelay(uint16_t delay);

    int ordersAppendRelDist(int16_t dist);
    int ordersAppendRelAngle(float angle);

private:
    s_order *orders_;
    uint8_t order_count_;
    uint8_t fifo_size_;
};

#endif // #ifndef ORDER_H_INCLUDED
