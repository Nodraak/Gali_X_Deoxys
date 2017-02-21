
#include <cstring>  // memset, memmove
#include "common/OrdersFIFO.h"


const char *e2s_order_com_type[ORDER_COM_TYPE_LAST] = {
    "NONE",
    "ABS_POS",
    "ABS_ANGLE",
    "REL_DIST",
    "REL_ANGLE",
    "DELAY"
};

const char *e2s_order_exe_type[ORDER_EXE_TYPE_LAST] = {
    "NONE",
    "POS",
    "ANGLE",
    "DELAY"
};


OrdersFIFO::OrdersFIFO(uint8_t fifo_size) {
    fifo_size_ = fifo_size;
    orders_ = new s_order_com[fifo_size];
    this->reset();
}

OrdersFIFO::~OrdersFIFO(void) {
    delete[] orders_;
}

void OrdersFIFO::reset(void) {
    memset(orders_, 0, sizeof(s_order_com)*fifo_size_);
    order_count_ = 0;
}

int OrdersFIFO::push(s_order_com item) {
    if (order_count_ == fifo_size_)
        return 1;

    memmove(&orders_[order_count_], &item, sizeof(s_order_com));
    order_count_ ++;

    return 0;
}

void OrdersFIFO::pop(void) {
    if (this->size() == 0)
        return;

    order_count_ --;

    memmove(&orders_[0], &orders_[1], sizeof(s_order_com)*order_count_);

    if (order_count_ == 0)
    {
        memset(&orders_[0], 0, sizeof(s_order_com));
        orders_[0].type = ORDER_COM_TYPE_NONE;
    }
}

s_order_com *OrdersFIFO::front(void) {
    if (this->size() == 0)
        return NULL;
    else
        return this->elem(0);
}

s_order_com *OrdersFIFO::back(void) {
    if (this->size() == 0)
        return NULL;
    else
        return this->elem(order_count_-1);
}

s_order_com *OrdersFIFO::elem(uint8_t id) {
    if (id >= order_count_)
        return NULL;
    return &orders_[id];
}

uint8_t OrdersFIFO::size(void) {
    return order_count_;
}


/*
    Order_make*
*/

s_order_com OrderCom_makeAbsPos(int16_t x, int16_t y) {
    s_order_com tmp;
    tmp.type = ORDER_COM_TYPE_ABS_POS;
    tmp.order_data.abs_pos.x = x;
    tmp.order_data.abs_pos.y = y;
    return tmp;
}

s_order_com OrderCom_makeAbsAngle(float angle) {
    s_order_com tmp;
    tmp.type = ORDER_COM_TYPE_ABS_ANGLE;
    tmp.order_data.abs_angle = angle;
    return tmp;
}

s_order_com OrderCom_makeRelDist(int32_t dist) {
    s_order_com tmp;
    tmp.type = ORDER_COM_TYPE_REL_DIST;
    tmp.order_data.rel_dist = dist;
    return tmp;
}

s_order_com OrderCom_makeRelAngle(float angle) {
    s_order_com tmp;
    tmp.type = ORDER_COM_TYPE_REL_ANGLE;
    tmp.order_data.rel_angle = angle;
    return tmp;
}

s_order_com OrderCom_makeDelay(float delay) {
    s_order_com tmp;
    tmp.type = ORDER_COM_TYPE_DELAY;
    tmp.order_data.delay = delay;
    return tmp;
}
