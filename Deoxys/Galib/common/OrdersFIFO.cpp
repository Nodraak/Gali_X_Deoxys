
#include <cstring>  // memset, memmove

#include "common/Debug.h"
#ifdef IAM_QENTRESORT
#include "QEntreQSort/Actuators.h"
#endif
#include "config.h"

#include "common/OrdersFIFO.h"


const char *e2s_order_com_type[ORDER_COM_TYPE_LAST] = {
    "NONE",

    "DELAY",

    "WAIT_CQB",
    "WAIT_CQES",

    "MOV_ABS_POS",
    "MOV_ABS_ANGLE",
    "MOV_REL_DIST",
    "MOV_REL_ANGLE",

    "ORDER_COM_TYPE_ACTUATOR",

    "ACT_ORDER_COM_TYPE_SEQ_ARM_INIT",
    "ACT_ORDER_COM_TYPE_SEQ_ARM_GRAB",
    "ACT_ORDER_COM_TYPE_SEQ_ARM_MOVE_UP",
    "ACT_ORDER_COM_TYPE_SEQ_ARM_RELEASE",
    "ACT_ORDER_COM_TYPE_SEQ_ARM_MOVE_DOWN",
    "ACT_ORDER_COM_TYPE_SEQ_FLAP",
    "ACT_ORDER_COM_TYPE_SEQ_PROGRADE_DISPENSER"
};


const char *e2s_order_exe_type[ORDER_EXE_TYPE_LAST] = {
    "NONE",

    "DELAY",

    "WAIT_CQB",
    "WAIT_CQES",

    "MOV_POS",
    "MOV_ANGLE",

    "ORDER_EXE_TYPE_ACTUATOR"
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
}

int OrdersFIFO::append(s_order_com item) {
    if (order_count_ == fifo_size_)
        return 1;

    // copy the new one at the end
    memcpy(&orders_[order_count_], &item, sizeof(s_order_com)*1);  // areas do *not* overlap, memcpy is ok
    // plus one order
    order_count_ ++;

    // we just received an order, therefore we can request the next one right away if needed
    request_next_order_at_timestamp_ = timer_.read();

    return 0;
}

int OrdersFIFO::prepend(s_order_com item) {
    if (order_count_ == fifo_size_)
        return 1;

    // first move all orders to make room for the new one
    memmove(&orders_[1], &orders_[0], sizeof(s_order_com)*order_count_);  // areas *do* overlap, memmove is required
    // copy the new one in front
    memcpy(&orders_[0], &item, sizeof(s_order_com)*1);
    // plus one order
    order_count_ ++;

    // we just received an order, therefore we can request the next one right away if needed
    request_next_order_at_timestamp_ = timer_.read();

    return 0;
}

int OrdersFIFO::next_order_execute(void) {
    int ret = 0;
    s_order_com next;

    // if no other orders in the queue, hold angle and position
    if (order_count_ == 0)
    {
        current_order_.type = ORDER_EXE_TYPE_NONE;
        current_order_.delay = 0;
    }
    // else, consume the next order
    else
    {
        memcpy(&next, this->front(), sizeof(s_order_com));
        this->pop();

        switch (next.type)
        {
            case ORDER_COM_TYPE_NONE:
                current_order_.type = ORDER_EXE_TYPE_NONE;
                break;

            case ORDER_COM_TYPE_DELAY:
                current_order_.type = ORDER_EXE_TYPE_DELAY;
                current_order_.delay = next.order_data.delay;
                break;

            case ORDER_COM_TYPE_WAIT_CQB_FINISHED:
                current_order_.type = ORDER_EXE_TYPE_WAIT_CQB_FINISHED;
                break;

            case ORDER_COM_TYPE_WAIT_CQES_FINISHED:
                current_order_.type = ORDER_EXE_TYPE_WAIT_CQES_FINISHED;
                break;

#ifdef IAM_QBOUGE
            case ORDER_COM_TYPE_MOV_ABS_POS:
                current_order_.type = ORDER_EXE_TYPE_MOV_POS;
                current_order_.pos.x = next.order_data.abs_pos.x;
                current_order_.pos.y = next.order_data.abs_pos.y;
                break;

            case ORDER_COM_TYPE_MOV_ABS_ANGLE:
                current_order_.type = ORDER_EXE_TYPE_MOV_ANGLE;
                current_order_.angle = next.order_data.abs_angle;
                break;

            case ORDER_COM_TYPE_MOV_REL_DIST:
                current_order_.type = ORDER_EXE_TYPE_MOV_POS;
                current_order_.pos.x += next.order_data.rel_dist * cos(current_order_.angle);
                current_order_.pos.y += next.order_data.rel_dist * sin(current_order_.angle);
                break;

            case ORDER_COM_TYPE_MOV_REL_ANGLE:
                current_order_.type = ORDER_EXE_TYPE_MOV_ANGLE;
                current_order_.angle += next.order_data.rel_angle;
                break;
#else
            case ORDER_COM_TYPE_MOV_ABS_POS:
            case ORDER_COM_TYPE_MOV_ABS_ANGLE:
            case ORDER_COM_TYPE_MOV_REL_DIST:
            case ORDER_COM_TYPE_MOV_REL_ANGLE:
                // ignore if not on CQB
                ret = 1;
#endif

#ifdef IAM_QENTRESORT
            case ORDER_COM_TYPE_ACTUATOR:
                current_order_.type = ORDER_EXE_TYPE_ACTUATOR;
                current_order_.act_param = next.order_data.act_param;
                break;

            case ORDER_COM_TYPE_SEQ_ARM_INIT:
            case ORDER_COM_TYPE_SEQ_ARM_GRAB:
            case ORDER_COM_TYPE_SEQ_ARM_MOVE_UP:
            case ORDER_COM_TYPE_SEQ_ARM_RELEASE:
            case ORDER_COM_TYPE_SEQ_ARM_MOVE_DOWN:
            case ORDER_COM_TYPE_SEQ_FLAP:
            case ORDER_COM_TYPE_SEQ_PROGRADE_DISPENSER:
                Actuators::order_decode_sequence(this, (e_order_com_type)next.type, next.order_data.act_param);
                break;

#else
            case ORDER_COM_TYPE_ACTUATOR:
            case ORDER_COM_TYPE_SEQ_ARM_INIT:
            case ORDER_COM_TYPE_SEQ_ARM_GRAB:
            case ORDER_COM_TYPE_SEQ_ARM_MOVE_UP:
            case ORDER_COM_TYPE_SEQ_ARM_RELEASE:
            case ORDER_COM_TYPE_SEQ_ARM_MOVE_DOWN:
            case ORDER_COM_TYPE_SEQ_FLAP:
            case ORDER_COM_TYPE_SEQ_PROGRADE_DISPENSER:
                // ignore if not on CQES
                ret = 1;
                break;
#endif

            case ORDER_COM_TYPE_LAST:
                // nothing to do
                break;
        }

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

    debug->printf("[OrdersFIFO/debug] cur -> %s\n", e2s_order_exe_type[current_order_.type]);

    if (this->size() == 0)
        debug->printf("[OrdersFIFO/debug] empty\n");
    else
    {
        for (i = 0; i < this->size(); ++i)
        {
            s_order_com *cur = &orders_[i];
            e_order_com_type type = (e_order_com_type)cur->type;

            debug->printf("[OrdersFIFO/debug] %d/%d -> %s ", i, this->size(), e2s_order_com_type[type]);

            switch (type)
            {
                case ORDER_COM_TYPE_NONE:
                    // nothing to do
                    break;

                case ORDER_COM_TYPE_WAIT_CQB_FINISHED:
                case ORDER_COM_TYPE_WAIT_CQES_FINISHED:
                    // ignore
                    break;

                case ORDER_COM_TYPE_DELAY:
                    debug->printf("%.3f", cur->order_data.delay);
                    break;

                case ORDER_COM_TYPE_MOV_ABS_POS:
                    debug->printf("%d %d", cur->order_data.abs_pos.x, cur->order_data.abs_pos.y);
                    break;

                case ORDER_COM_TYPE_MOV_ABS_ANGLE:
                    debug->printf("%d", (int)RAD2DEG(cur->order_data.abs_angle));
                    break;

                case ORDER_COM_TYPE_MOV_REL_DIST:
                    debug->printf("%d", cur->order_data.rel_dist);
                    break;

                case ORDER_COM_TYPE_MOV_REL_ANGLE:
                    debug->printf("%d", (int)RAD2DEG(cur->order_data.rel_angle));
                    break;

                case ORDER_COM_TYPE_ACTUATOR:

                    debug->printf(
                        "%d%d%d %d%d%d%d%d%d%d %d%d",
                        bool(cur->order_data.act_param & ACT_STATE_EXTENDED),
                        bool(cur->order_data.act_param & ACT_STATE_NEUTRAL),
                        bool(cur->order_data.act_param & ACT_STATE_RETRACTED),

                        bool(cur->order_data.act_param & ACT_ACTUATOR_HEIGHT),
                        bool(cur->order_data.act_param & ACT_ACTUATOR_VERT),
                        bool(cur->order_data.act_param & ACT_ACTUATOR_HORIZ),
                        bool(cur->order_data.act_param & ACT_ACTUATOR_CLAMP),
                        bool(cur->order_data.act_param & ACT_ACTUATOR_PUMP),
                        bool(cur->order_data.act_param & ACT_ACTUATOR_FLAP),
                        bool(cur->order_data.act_param & ACT_ACTUATOR_PROG),

                        bool(cur->order_data.act_param & ACT_SIDE_LEFT),
                        bool(cur->order_data.act_param & ACT_SIDE_RIGHT)
                    );
                    break;

                case ORDER_COM_TYPE_SEQ_ARM_INIT:
                case ORDER_COM_TYPE_SEQ_ARM_GRAB:
                case ORDER_COM_TYPE_SEQ_ARM_MOVE_UP:
                case ORDER_COM_TYPE_SEQ_ARM_RELEASE:
                case ORDER_COM_TYPE_SEQ_ARM_MOVE_DOWN:
                case ORDER_COM_TYPE_SEQ_FLAP:
                case ORDER_COM_TYPE_SEQ_PROGRADE_DISPENSER:
                    debug->printf(
                        "side l=%d r=%d",
                        bool(cur->order_data.act_param & ACT_SIDE_LEFT),
                        bool(cur->order_data.act_param & ACT_SIDE_RIGHT)
                    );
                    break;

                case ORDER_COM_TYPE_LAST:
                    // nothing to do
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
    tmp.type = ORDER_COM_TYPE_MOV_ABS_POS;
    tmp.order_data.abs_pos.x = x;
    tmp.order_data.abs_pos.y = y;
    return tmp;
}

s_order_com OrderCom_makeAbsAngle(float angle) {
    s_order_com tmp;
    tmp.type = ORDER_COM_TYPE_MOV_ABS_ANGLE;
    tmp.order_data.abs_angle = angle;
    return tmp;
}

s_order_com OrderCom_makeRelDist(int32_t dist) {
    s_order_com tmp;
    tmp.type = ORDER_COM_TYPE_MOV_REL_DIST;
    tmp.order_data.rel_dist = dist;
    return tmp;
}

s_order_com OrderCom_makeRelAngle(float angle) {
    s_order_com tmp;
    tmp.type = ORDER_COM_TYPE_MOV_REL_ANGLE;
    tmp.order_data.rel_angle = angle;
    return tmp;
}

s_order_com OrderCom_makeActuator(t_act act_param) {
    s_order_com tmp;
    tmp.type = ORDER_COM_TYPE_ACTUATOR;
    tmp.order_data.act_param = act_param;
    return tmp;
}

s_order_com OrderCom_makeSequence(e_order_com_type seq, t_act act_param) {
    s_order_com tmp;
    tmp.type = seq;
    tmp.order_data.act_param = act_param;
    return tmp;
}
