
#include <cstring>
#include "mbed.h"

#include "common/OrdersFIFO.h"
#include "common/utils.h"
#include "QBouge/MotionController.h"
#include "pinout.h"

#include "Messenger.h"


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
    int ret = 0;

    ret = can_.read(msg);
    if (ret)
    {
        dest->id = (Message::e_message_type)msg.id;
        dest->len = msg.len;
        memcpy(dest->payload.raw_data, msg.data, 8);
    }
    return ret;
}

/*
    CanMessenger::send_msg_*
*/

int CanMessenger::send_msg_ping(char data[8]) {
    Message::CP_ping payload;
    if (data != NULL)
        memcpy(payload.data, data, 8);
    else
        memset(payload.data, 0, 8);

    return this->send_msg(Message(Message::MT_ping, sizeof(payload), (Message::u_payload){.ping = payload}));
}

int CanMessenger::send_msg_pong(char data[8]) {
    Message::e_message_type message_type;
    Message::CP_pong payload;

#ifdef IAM_QBOUGE
    message_type = Message::MT_CQB_pong;
#endif
#ifdef IAM_QREFLECHI
    message_type = Message::MT_CQR_pong;
#endif
    if (data != NULL)
        memcpy(payload.data, data, 8);
    else
        memset(payload.data, 0, 8);

    return this->send_msg(Message(message_type, sizeof(payload), (Message::u_payload){.pong = payload}));
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

int CanMessenger::send_msg_order(s_order_com order) {
    return this->send_msg(Message(Message::MT_order, sizeof(order), (Message::u_payload){.order = order}));
}

int CanMessenger::send_msg_CQB_next_order_request(uint8_t count) {
    Message::CP_CQB_next_order_request payload;
    payload.count = count;
    return this->send_msg(Message(
        Message::MT_CQB_next_order_request, sizeof(payload), (Message::u_payload){.CQB_next_order_request = payload}
    ));
}

int CanMessenger::send_msg_CQB_MC_pos(float x, float y) {
    Message::CP_CQB_MC_pos payload;
    payload.pos.x = x;
    payload.pos.y = y;

    return this->send_msg(Message(Message::MT_CQB_MC_pos, sizeof(payload), (Message::u_payload){.CQB_MC_pos = payload}));
}

int CanMessenger::send_msg_CQB_MC_angle_speed(float angle, float speed) {
    Message::CP_CQB_MC_angle_speed payload;
    payload.angle = angle;
    payload.speed = speed;
    return this->send_msg(Message(Message::MT_CQB_MC_angle_speed, sizeof(payload), (Message::u_payload){.CQB_MC_angle_speed = payload}));
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
