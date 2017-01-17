
#include <cstring>  // memset, memmove
#include "common/OrdersFIFO.h"


OrdersFIFO::OrdersFIFO(uint8_t fifo_size) {
    fifo_size_ = fifo_size;
    orders_ = new s_order[fifo_size];
    order_count_ = 0;
}

OrdersFIFO::~OrdersFIFO(void) {
    delete[] orders_;
}

void OrdersFIFO::reset(void) {
    memset(orders_, 0, sizeof(s_order)*order_count_);
    order_count_ = 0;
}

int OrdersFIFO::push(s_order *item) {
    if (order_count_ == fifo_size_)
        return 1;

    memmove(&orders_[0], item, sizeof(s_order));
    order_count_ ++;

    return 0;
}

void OrdersFIFO::pop(void) {
    order_count_ --;

    memmove(&orders_[0], &orders_[1], sizeof(s_order)*order_count_);

    if (order_count_ == 0)
    {
        memset(&orders_[0], 0, sizeof(s_order));
        orders_[0].type = ORDER_TYPE_NONE;
    }
}

s_order *OrdersFIFO::front(void) {
    return this->elem(0);
}

s_order *OrdersFIFO::back(void) {
    return this->elem(order_count_-1);
}

s_order *OrdersFIFO::elem(uint8_t id) {
    if (id >= order_count_)
        return NULL;
    return &orders_[id];
}

uint8_t OrdersFIFO::size(void) {
    return order_count_;
}

/******************************************************************************/

int OrdersFIFO::ordersAppend(e_order_type type, int16_t x, int16_t y, float angle, uint16_t delay) {
    s_order tmp;

    tmp.type = type;

    switch (type) {
        case ORDER_TYPE_POS:
            tmp.pos.x = x;
            tmp.pos.y = y;
            break;

        case ORDER_TYPE_ANGLE:
            tmp.angle = std_rad_angle(angle);
            break;

        case ORDER_TYPE_DELAY:
            tmp.delay = delay;
            break;

        case ORDER_TYPE_NONE:
            // todo
            break;
    }

    return this->push(&tmp);
}

int OrdersFIFO::ordersAppendAbsPos(int16_t x, int16_t y) {
    return this->ordersAppend(ORDER_TYPE_POS, x, y, 0, 0);
}

int OrdersFIFO::ordersAppendAbsAngle(float angle) {
    return this->ordersAppend(ORDER_TYPE_ANGLE, 0, 0, angle, 0);
}

int OrdersFIFO::ordersAppendAbsDelay(uint16_t delay) {
    return this->ordersAppend(ORDER_TYPE_DELAY, 0, 0, 0, delay);
}

int OrdersFIFO::ordersAppendRelDist(int16_t dist) {
    s_order *last_order = this->back();
    return this->ordersAppend(
        ORDER_TYPE_POS,
        last_order->pos.x + dist*cos(last_order->angle), last_order->pos.y + dist*sin(last_order->angle),
        last_order->angle, last_order->delay
    );
}

int OrdersFIFO::ordersAppendRelAngle(float angle) {
    s_order *last_order = this->back();
    return this->ordersAppend(
        ORDER_TYPE_ANGLE, last_order->pos.x, last_order->pos.y, last_order->angle + angle, last_order->delay
    );
}
