
#include <cstring>  // memset, memmove

#include "common/Debug.h"
#include "QEntreQSort/Actuator.h"

#include "config.h"

#include "common/OrdersFIFO.h"


const char *e2s_order_com_type[ORDER_COM_TYPE_LAST] = {
    "NONE",

    "DELAY",

    "WAIT_CQB",
    "WAIT_CQES",

    "ABS_POS",
    "ABS_ANGLE",
    "REL_DIST",
    "REL_ANGLE",

    "ORDER_COM_TYPE_ARM_INIT",
    "ORDER_COM_TYPE_ARM_GRAB",
    "ORDER_COM_TYPE_ARM_MOVE_UP",
    "ORDER_COM_TYPE_ARM_RELEASE",
    "ORDER_COM_TYPE_ARM_MOVE_DOWN"
};


const char *e2s_order_exe_type[ORDER_EXE_TYPE_LAST] = {
    "NONE",

    "DELAY",

    "WAIT_CQB",
    "WAIT_CQES",

    "POS",
    "ANGLE",

    "ORDER_COM_TYPE_ARM_INIT",
    "ORDER_COM_TYPE_ARM_GRAB",
    "ORDER_COM_TYPE_ARM_MOVE_UP",
    "ORDER_COM_TYPE_ARM_RELEASE",
    "ORDER_COM_TYPE_ARM_MOVE_DOWN"
};


OrdersFIFO::OrdersFIFO(uint8_t fifo_size) {
    current_order_.type = ORDER_EXE_TYPE_NONE;
    current_order_.delay = 0;
#ifdef IAM_QBOUGE
    current_order_.pos.x = 0;
    current_order_.pos.y = 0;
    current_order_.angle = 0;
#endif

    timer_.start();
    last_order_executed_timestamp_ = 0;
    request_next_order_at_timestamp_ = 0;

    orders_ = new s_order_com[fifo_size];
    fifo_size_ = fifo_size;
    this->reset();
}

OrdersFIFO::~OrdersFIFO(void) {
    delete[] orders_;
}

void OrdersFIFO::reset(void) {
    memset(orders_, 0, sizeof(s_order_com)*fifo_size_);
    order_count_ = 0;
}

void OrdersFIFO::we_are_at(int16_t x, int16_t y, float angle) {
#ifdef IAM_QBOUGE
    current_order_.pos.x = x;
    current_order_.pos.y = y;
    current_order_.angle = angle;
#endif
#ifdef IAM_QENTRESORT
    current_order_.which_arm = ACT_SIDE_NONE;
#endif
}

int OrdersFIFO::push(s_order_com item) {
    if (order_count_ == fifo_size_)
        return 1;

    memcpy(&orders_[order_count_], &item, sizeof(s_order_com));
    order_count_ ++;

    // we just received an order, therefore we can request the next one right away if needed
    request_next_order_at_timestamp_ = timer_.read();

    return 0;
}

int OrdersFIFO::next_order_execute(void) {
    int ret = 0;

    // if no other orders in the queue, hold angle and position
    if (order_count_ == 0)
    {
        current_order_.type = ORDER_EXE_TYPE_NONE;
        current_order_.delay = 0;
    }
    // else, consume the next order
    else
    {
        s_order_com *next = this->front();

        switch (next->type)
        {
            case ORDER_COM_TYPE_NONE:
                current_order_.type = ORDER_EXE_TYPE_NONE;
                break;

            case ORDER_COM_TYPE_DELAY:
                current_order_.type = ORDER_EXE_TYPE_DELAY;
                current_order_.delay = next->order_data.delay;
                break;

            case ORDER_COM_TYPE_WAIT_CQB_FINISHED:
                current_order_.type = ORDER_EXE_TYPE_WAIT_CQB_FINISHED;
                break;

            case ORDER_COM_TYPE_WAIT_CQES_FINISHED:
                current_order_.type = ORDER_EXE_TYPE_WAIT_CQES_FINISHED;
                break;

#ifdef IAM_QBOUGE
            case ORDER_COM_TYPE_ABS_POS:
                current_order_.type = ORDER_EXE_TYPE_POS;
                current_order_.pos.x = next->order_data.abs_pos.x;
                current_order_.pos.y = next->order_data.abs_pos.y;
                break;

            case ORDER_COM_TYPE_ABS_ANGLE:
                current_order_.type = ORDER_EXE_TYPE_ANGLE;
                current_order_.angle = next->order_data.abs_angle;
                break;

            case ORDER_COM_TYPE_REL_DIST:
                current_order_.type = ORDER_EXE_TYPE_POS;
                current_order_.pos.x += next->order_data.rel_dist * cos(current_order_.angle);
                current_order_.pos.y += next->order_data.rel_dist * sin(current_order_.angle);
                break;

            case ORDER_COM_TYPE_REL_ANGLE:
                current_order_.type = ORDER_EXE_TYPE_ANGLE;
                current_order_.angle += next->order_data.rel_angle;
                break;
#else
            case ORDER_COM_TYPE_ABS_POS:
            case ORDER_COM_TYPE_ABS_ANGLE:
            case ORDER_COM_TYPE_REL_DIST:
            case ORDER_COM_TYPE_REL_ANGLE:
                // ignore if not on CQB
                ret = 1;
#endif

#ifdef IAM_QENTRESORT
            case ORDER_COM_TYPE_ARM_INIT:
                current_order_.type = ORDER_EXE_TYPE_ARM_INIT;
                current_order_.which_arm = next->order_data.which_arm;
                break;

            case ORDER_COM_TYPE_ARM_GRAB:
                current_order_.type = ORDER_EXE_TYPE_ARM_GRAB;
                current_order_.which_arm = next->order_data.which_arm;
                break;

            case ORDER_COM_TYPE_ARM_MOVE_UP:
                current_order_.type = ORDER_EXE_TYPE_ARM_MOVE_UP;
                current_order_.which_arm = next->order_data.which_arm;
                break;

            case ORDER_COM_TYPE_ARM_RELEASE:
                current_order_.type = ORDER_EXE_TYPE_ARM_RELEASE;
                current_order_.which_arm = next->order_data.which_arm;
                break;

            case ORDER_COM_TYPE_ARM_MOVE_DOWN:
                current_order_.type = ORDER_EXE_TYPE_ARM_MOVE_DOWN;
                current_order_.which_arm = next->order_data.which_arm;
                break;
#else
            case ORDER_COM_TYPE_ARM_INIT:
            case ORDER_COM_TYPE_ARM_GRAB:
            case ORDER_COM_TYPE_ARM_MOVE_UP:
            case ORDER_COM_TYPE_ARM_RELEASE:
            case ORDER_COM_TYPE_ARM_MOVE_DOWN:
                // ignore if not on CQES
                ret = 1;
                break;
#endif

            case ORDER_COM_TYPE_LAST:
                // nothing to do
                break;
        }

        this->pop();
        last_order_executed_timestamp_ = timer_.read();
    }

    return ret;
}

bool OrdersFIFO::next_order_should_request(void) {
    if ((ORDERS_COUNT - order_count_ > 0) && (timer_.read() > request_next_order_at_timestamp_))
    {
        request_next_order_at_timestamp_ = timer_.read() + REQUEST_NEXT_ORDER_DELAY;
        return true;
    }
    return false;
}

void OrdersFIFO::pop(void) {
    if (order_count_ == 0)
        return;

    order_count_ --;

    if (order_count_ == 0)
    {
        memset(&orders_[0], 0, sizeof(s_order_com));
        orders_[0].type = ORDER_COM_TYPE_NONE;
    }
    else
    {
        memmove(&orders_[0], &orders_[1], sizeof(s_order_com)*order_count_);
    }
}

s_order_com *OrdersFIFO::front(void) {
    if (order_count_ == 0)
        return NULL;
    else
        return &orders_[0];
}

int OrdersFIFO::size(void) {
    return order_count_;
}

void OrdersFIFO::debug(Debug *debug) {
    int i = 0;

    if (this->size() == 0)
        debug->printf("[MC/orders] empty\n");
    else
    {
        for (i = 0; i < this->size(); ++i)
        {
            s_order_com *cur = &orders_[i];

            debug->printf("[MC/orders] %d/%d -> %s ", i, this->size(), e2s_order_com_type[cur->type]);

            switch (cur->type)
            {
                case ORDER_COM_TYPE_NONE:
                    // nothing to do
                    break;

                case ORDER_COM_TYPE_ABS_POS:
                    debug->printf("%d %d", cur->order_data.abs_pos.x, cur->order_data.abs_pos.y);
                    break;

                case ORDER_COM_TYPE_ABS_ANGLE:
                    debug->printf("%d", (int)RAD2DEG(cur->order_data.abs_angle));
                    break;

                case ORDER_COM_TYPE_REL_DIST:
                    debug->printf("%d", cur->order_data.rel_dist);
                    break;

                case ORDER_COM_TYPE_REL_ANGLE:
                    debug->printf("%d", (int)RAD2DEG(cur->order_data.rel_angle));
                    break;

                case ORDER_COM_TYPE_DELAY:
                    debug->printf("%.3f", cur->order_data.delay);
                    break;
            }

            debug->printf("\n");
        }
    }
}

/*
    OrderCom_make*
*/

s_order_com OrderCom_makeNone(void) {
    s_order_com tmp;
    tmp.type = ORDER_COM_TYPE_NONE;
    return tmp;
}

s_order_com OrderCom_makeDelay(float delay) {
    s_order_com tmp;
    tmp.type = ORDER_COM_TYPE_DELAY;
    tmp.order_data.delay = delay;
    return tmp;
}

s_order_com OrderCom_makeWaitCQBFinished(void) {
    s_order_com tmp;
    tmp.type = ORDER_COM_TYPE_WAIT_CQB_FINISHED;
    return tmp;
}

s_order_com OrderCom_makeWaitCQESFinished(void) {
    s_order_com tmp;
    tmp.type = ORDER_COM_TYPE_WAIT_CQES_FINISHED;
    return tmp;
}

s_order_com OrderCom_makeAbsPos(s_vector_int16 pos) {
    return OrderCom_makeAbsPos(pos.x, pos.y);
}

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

s_order_com OrderCom_makeArmInit(uint8_t which_arm) {
    s_order_com tmp;
    tmp.type = ORDER_COM_TYPE_ARM_INIT;
    tmp.order_data.which_arm = which_arm;
    return tmp;
}

s_order_com OrderCom_makeArmGrab(uint8_t which_arm) {
    s_order_com tmp;
    tmp.type = ORDER_COM_TYPE_ARM_GRAB;
    tmp.order_data.which_arm = which_arm;
    return tmp;
}

s_order_com OrderCom_makeArmMoveUp(uint8_t which_arm) {
    s_order_com tmp;
    tmp.type = ORDER_COM_TYPE_ARM_MOVE_UP;
    tmp.order_data.which_arm = which_arm;
    return tmp;
}

s_order_com OrderCom_makeArmRelease(uint8_t which_arm) {
    s_order_com tmp;
    tmp.type = ORDER_COM_TYPE_ARM_RELEASE;
    tmp.order_data.which_arm = which_arm;
    return tmp;
}

s_order_com OrderCom_makeArmMoveDown(uint8_t which_arm) {
    s_order_com tmp;
    tmp.type = ORDER_COM_TYPE_ARM_MOVE_DOWN;
    tmp.order_data.which_arm = which_arm;
    return tmp;
}
