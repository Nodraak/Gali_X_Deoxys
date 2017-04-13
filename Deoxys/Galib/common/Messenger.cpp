
#include <cstring>
#include "mbed.h"

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
    return !can_.write(
        CANMessage(msg.id, msg.payload.raw_data, msg.len)
    );
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

int CanMessenger::send_msg_ping(void) {
    Message::CP_ping payload;
    return this->send_msg(Message(Message::MT_ping, sizeof(payload), (Message::u_payload){.ping = payload}));
}

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

int CanMessenger::send_msg_CQR_match_start(void) {
    Message::CP_CQR_match_start payload;
    return this->send_msg(Message(Message::MT_CQR_match_start, sizeof(payload), (Message::u_payload){.CQR_match_start = payload}));
}

int CanMessenger::send_msg_CQR_match_stop(void) {
    Message::CP_CQR_match_stop payload;
    return this->send_msg(Message(Message::MT_CQR_match_stop, sizeof(payload), (Message::u_payload){.CQR_match_stop = payload}));
}

int CanMessenger::send_msg_we_are_at(int16_t x, int16_t y, float angle) {
    Message::CP_CQR_we_are_at payload;
    payload.pos.x = x;
    payload.pos.y = y;
    payload.angle = angle;
    return this->send_msg(Message(Message::MT_we_are_at, sizeof(payload), (Message::u_payload){.CQR_we_are_at = payload}));
}

int CanMessenger::send_msg_CQR_reset(void) {
    Message::CP_CQR_reset payload;
    return this->send_msg(Message(Message::MT_CQR_reset, sizeof(payload), (Message::u_payload){.CQR_reset = payload}));
}

int CanMessenger::send_msg_order(s_order_com order) {
    return this->send_msg(Message(Message::MT_order, sizeof(order), (Message::u_payload){.order = order}));
}

int CanMessenger::send_msg_CQB_finished(void) {
    return this->send_msg(Message(Message::MT_CQB_finished, 0, (Message::u_payload){}));
}

int CanMessenger::send_msg_CQES_finished(void) {
    return this->send_msg(Message(Message::MT_CQES_finished, 0, (Message::u_payload){}));
}

int CanMessenger::send_msg_CQB_next_order_request(uint8_t count) {
    Message::CP_CQB_next_order_request payload;
    payload.count = count;
    return this->send_msg(Message(
        Message::MT_CQB_next_order_request, sizeof(payload), (Message::u_payload){.CQB_next_order_request = payload}
    ));
}

int CanMessenger::send_msg_I_am_doing(e_order_exe_type order) {
    Message::CP_I_am_doing payload;
#ifdef IAM_QBOUGE
    sprintf(payload.i_am, "CQB");
#endif
#ifdef IAM_QREFLECHI
    sprintf(payload.i_am, "CQR");
#endif
#ifdef IAM_QENTRESORT
    sprintf(payload.i_am, "CQE");
#endif
    payload.order = order;
    return this->send_msg(Message(Message::MT_I_am_doing, sizeof(payload), (Message::u_payload){.I_am_doing = payload}));
}

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
