
#include <cstring>
#include "mbed.h"

#include "common/Monitoring.h"
#include "common/OrdersFIFO.h"
#include "common/utils.h"
#include "QBouge/MotionController.h"
#include "pinout.h"

#include "common/Messenger.h"


/*
    Message (payload send via CAN)
*/

Message::Message(void) {
    id = MT_empty;
    len = 0;
    memset(payload.raw_data, 0, 8);
}

Message::Message(e_message_type id_, unsigned int len_, u_payload payload_) {
    id = id_;
    len = len_;
    payload = payload_;
}


/*
    CanMessenger (medium used to transmit Message). This could be UartMessenger or I2cMessenger.
*/

CanMessenger::CanMessenger(void) : can_(CAN_RX, CAN_TX) {
    can_.frequency(CAN_BUS_FREQUENCY);
    or_count_ = 0;
}

int CanMessenger::send_msg(Message msg) {
    // Can::write               returns 1 if success 0 if error.
    // CanMessenger.send_msg    returns 0 if success, 1 if error.
    int ret = !can_.write(
        CANMessage(msg.id, msg.payload.raw_data, msg.len)
    );

    if (g_mon != NULL)
    {
        g_mon->can_usage.inc();
        if (ret)
            g_mon->can_send_errors.inc();
    }

    return ret;
}

int CanMessenger::read_msg(Message *dest) {
    CANMessage msg;
    int ret = 0, i = 0;

    ret = can_.read(msg);
    if (ret)
    {
        dest->id = (Message::e_message_type)msg.id;
        dest->len = msg.len;
        memset(dest->payload.raw_data, 0, 8);
        memcpy(dest->payload.raw_data, msg.data, msg.len);

        if (g_mon != NULL)
            g_mon->can_usage.inc();

        for (i = 0; i < or_count_; ++i)
        {
            if (or_types_[i] == dest->id)
                or_callbacks_[i](dest);
        }
    }
    return ret;
}

void CanMessenger::set_silent(bool enable) {
    can_.monitor(enable);
}

void CanMessenger::leave_the_bus_for_a_moment(void) {
    this->set_silent(true);
    Thread::wait(3);  // ms
    // 1/(500*1000) * (128*11) == 2.8 ms - bus off recovery time (let other boards initialise their CAN)
    this->set_silent(false);
}

int CanMessenger::on_receive_add(Message::e_message_type type, Callback<void(void*)> cb) {
    if (or_count_ == ON_RECEIVE_SLOT_COUNT)
        return 1;

    or_types_[or_count_] = type;
    or_callbacks_[or_count_] = cb;
    or_count_ ++;

    return 0;
}


/*
    CanMessenger::send_msg_*
*/

#ifdef IAM_QREFLECHI
int CanMessenger::send_msg_CQR_ping(void) {
    Message::CP_CQR_ping payload;
    return this->send_msg(Message(Message::MT_CQR_ping, sizeof(payload), (Message::u_payload){.CQR_ping = payload}));
}
#endif

int CanMessenger::send_msg_pong(void) {
    Message::e_message_type message_type;
    Message::CP_pong payload;

#ifdef IAM_QBOUGE
    message_type = Message::MT_CQB_pong;
#endif
#ifdef IAM_QREFLECHI
    message_type = Message::MT_CQR_pong;
#endif
#ifdef IAM_QENTRESORT
    message_type = Message::MT_CQES_pong;
#endif

    return this->send_msg(Message(message_type, sizeof(payload), (Message::u_payload){.pong = payload}));
}

#ifdef IAM_QREFLECHI
int CanMessenger::send_msg_CQR_match_start(void) {
    Message::CP_CQR_match_start payload;
    return this->send_msg(Message(Message::MT_CQR_match_start, sizeof(payload), (Message::u_payload){.CQR_match_start = payload}));
}

int CanMessenger::send_msg_CQR_match_stop(void) {
    Message::CP_CQR_match_stop payload;
    return this->send_msg(Message(Message::MT_CQR_match_stop, sizeof(payload), (Message::u_payload){.CQR_match_stop = payload}));
}

int CanMessenger::send_msg_CQR_we_are_at(int16_t x, int16_t y, float angle) {
    Message::CP_CQR_we_are_at payload;
    payload.pos.x = x;
    payload.pos.y = y;
    payload.angle = angle;
    return this->send_msg(Message(Message::MT_CQR_we_are_at, sizeof(payload), (Message::u_payload){.CQR_we_are_at = payload}));
}

int CanMessenger::send_msg_CQR_reset(void) {
    Message::CP_CQR_reset payload;
    return this->send_msg(Message(Message::MT_CQR_reset, sizeof(payload), (Message::u_payload){.CQR_reset = payload}));
}

int CanMessenger::send_msg_CQR_order(s_order_com order) {
    return this->send_msg(Message(Message::MT_CQR_order, sizeof(order), (Message::u_payload){.CQR_order = order}));
}
#endif

#ifdef IAM_QBOUGE
int CanMessenger::send_msg_CQB_finished(void) {
    return this->send_msg(Message(Message::MT_CQB_finished, 0, (Message::u_payload){}));
}

int CanMessenger::send_msg_CQB_next_order_request(void) {
    Message::CP_CQB_next_order_request payload;
    return this->send_msg(Message(
        Message::MT_CQB_next_order_request, sizeof(payload), (Message::u_payload){.CQB_next_order_request = payload}
    ));
}
#endif

#ifdef IAM_QENTRESORT
int CanMessenger::send_msg_CQES_finished(void) {
    return this->send_msg(Message(Message::MT_CQES_finished, 0, (Message::u_payload){}));
}

int CanMessenger::send_msg_CQES_next_order_request(void) {
    Message::CP_CQES_next_order_request payload;
    return this->send_msg(Message(
        Message::MT_CQES_next_order_request, sizeof(payload), (Message::u_payload){.CQES_next_order_request = payload}
    ));
}
#endif

int CanMessenger::send_msg_I_am_doing(e_order_exe_type order) {
    Message::e_message_type message_type;
    Message::CP_I_am_doing payload;
#ifdef IAM_QBOUGE
    message_type = Message::MT_CQB_I_am_doing;
    sprintf(payload.i_am, "CQB");
#endif
#ifdef IAM_QREFLECHI
    return 1;
#endif
#ifdef IAM_QENTRESORT
    message_type = Message::MT_CQES_I_am_doing;
    sprintf(payload.i_am, "CQE");
#endif
    payload.order = order;
    return this->send_msg(Message(message_type, sizeof(payload), (Message::u_payload){.I_am_doing = payload}));
}

#ifdef IAM_QBOUGE
int CanMessenger::send_msg_CQB_MC_pos_angle(float x, float y, float angle) {
    Message::CP_CQB_MC_pos_angle payload;
    payload.pos.x = x;
    payload.pos.y = y;
    payload.angle = angle;
    return this->send_msg(Message(Message::MT_CQB_MC_pos_angle, sizeof(payload), (Message::u_payload){.CQB_MC_pos_angle = payload}));
}

int CanMessenger::send_msg_CQB_MC_speeds(float speed, float speed_ang) {
    Message::CP_CQB_MC_speeds payload;
    payload.speed = speed;
    payload.speed_ang = speed_ang;
    return this->send_msg(Message(Message::MT_CQB_MC_speeds, sizeof(payload), (Message::u_payload){.CQB_MC_speeds = payload}));
}

int CanMessenger::send_msg_CQB_MC_pids(float dist, float angle) {
    Message::CP_CQB_MC_pids payload;
    payload.dist = dist;
    payload.angle = angle;
    return this->send_msg(Message(Message::MT_CQB_MC_pids, sizeof(payload), (Message::u_payload){.CQB_MC_pids = payload}));
}

int CanMessenger::send_msg_CQB_MC_motors(float pwm_l, float pwm_r) {
    Message::CP_CQB_MC_motors payload;
    payload.pwm_l = pwm_l;
    payload.pwm_r = pwm_r;
    return this->send_msg(Message(Message::MT_CQB_MC_motors, sizeof(payload), (Message::u_payload){.CQB_MC_motors = payload}));
}

int CanMessenger::send_msg_CQB_MC_encs(int32_t enc_l, int32_t enc_r) {
    Message::CP_CQB_MC_encs payload;
    payload.enc_l = enc_l;
    payload.enc_r = enc_r;
    return this->send_msg(Message(Message::MT_CQB_MC_encs, sizeof(payload), (Message::u_payload){.CQB_MC_encs = payload}));
}
#endif
