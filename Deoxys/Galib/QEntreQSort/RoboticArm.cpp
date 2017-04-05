#ifdef IAM_QENTRESORT

/*
void BufferedSerial::clear(void) {
    _rxbuf.clear();
}
*/

#include "mbed.h"
#include "BufferedSerial.h"

#include "common/Debug.h"

#include "QEntreQSort/RoboticArm.h"


extern Debug *g_debug;


AX12::AX12(void) : ax12_(AX12_PIN_TX, AX12_PIN_RX), txrx_select_(AX12_PIN_SWITCH) {
    this->txrx_set_rx();
    ax12_.baud(BAUD_RATE);

g_debug->printf("AX12_TIME_FOR_DATA_TO_BE_SENT %d\n", AX12_TIME_FOR_DATA_TO_BE_SENT);
g_debug->printf("AX12_READ_TIMEOUT %d\n", AX12_READ_TIMEOUT);
};

void AX12::txrx_set_rx(void) {
    txrx_select_ = false;
}

void AX12::txrx_set_tx(void) {
    txrx_select_ = true;
}

int AX12::__send_request(uint8_t payload_len, uint8_t *payload_data) {
    uint8_t checksum = 0, i = 0;
    uint8_t last_byte = 0, cur_byte = 0;
    uint8_t s_id = 0, s_instruction = 0, s_read_cmd_read_len = 0;
    uint8_t r_argv[10], r_id = 0, r_len = 0, r_error = 0, r_checksum = 0, expected_len = 0;
    Timer t;

    // sended fields
    s_id = payload_data[0];
    s_instruction = payload_data[2];
    // cmd specific fields
    s_read_cmd_read_len = payload_data[4];

    // compute checksum

    for (i = 0; i < payload_len; ++i)
        checksum += payload_data[i];
    checksum = ~checksum;

    // send data

    txrx_set_tx();

    ax12_.printf("%c%c", 0xFF, 0xFF);
    for (i = 0; i < payload_len; ++i)
        ax12_.printf("%c", payload_data[i]);
    ax12_.printf("%c", checksum);

    wait_us(AX12_TIME_FOR_DATA_TO_BE_SENT);  // wait a little for the data to be send
    ax12_.clear();  // clear the rx buffer to remove any outdated char
    txrx_set_rx();

    // wait for response and parse it

    t.start();

    do {
        cur_byte = ax12_.getc();

        if ((last_byte == 0xFF) && (cur_byte == 0xFF))
        {
            checksum = 0;

            r_id = ax12_.getc();
            checksum += r_id;

            r_len = ax12_.getc();
            checksum += r_len;

            r_error = ax12_.getc();
            checksum += r_error;

            for (i = 0; i < r_len-2; ++i)  // skip 1st which is r_error, and last, which is r_checksum
            {
                r_argv[i] = ax12_.getc();
                checksum += r_argv[i];
            }

            r_checksum = ax12_.getc();

            checksum = ~ checksum;

            // check errors

            if ((s_instruction == AX12_PING_CMD) || (s_instruction == AX12_WRITE_CMD))
                expected_len = 2;  // status packet: fixed length
            else if (s_instruction == AX12_READ_CMD)
                expected_len = 2 + s_read_cmd_read_len;  // variable packet-lenth
            else
                return AX12_E_UNHANDLED_CMD;

            if (r_id != s_id)
                return AX12_E_ID;
            if (r_len != expected_len)
                return AX12_E_LEN;
            if (r_checksum != checksum)
                return AX12_E_CHECKSUM;

            if (r_error)
                return -r_error;

            // return something valid

            if (s_instruction != AX12_READ_CMD)  // cmd is successfull
                return 0;
            else  // return the read value
            {
                if (s_read_cmd_read_len == AX12_READ_1B)
                    return r_argv[0];
                else if (s_read_cmd_read_len == AX12_READ_2B)
                    return (r_argv[1] << 8) | r_argv[0];
                else
                    return AX12_E_READ_CMD;
            }
        }

        last_byte = cur_byte;

    } while (t.read_us() < AX12_READ_TIMEOUT);

    return AX12_E_TIMEOUT;
};

int AX12::_send_request_with_retries(uint8_t len, uint8_t *data) {
    int i = 0,  ret = 0;
    for (i = 0; i < AX12_MAX_RETRIES; ++i)
    {
        ret = this->__send_request(len, data);
        if (ret >= 0)
        {
            // if (i != 0)
                g_debug->printf("\tCommand succeded after %d retries\n", i);
            return ret;
        }
        g_debug->printf("\terror (%d), retrying\n", ret);
        wait_ms(AX12_SLEEP_TIME_BETWEEN_RETRIES);
    }

    g_debug->printf("\tCommand failed after %d retries\n", i);

    if (ret < 0)
    {
        g_debug->printf("Error (%d): ", ret);

        if (ret == AX12_E_TIMEOUT)
            g_debug->printf("AX12_E_TIMEOUT\n");
        else if (ret == AX12_E_ID)
            g_debug->printf("AX12_E_ID\n");
        else if (ret == AX12_E_LEN)
            g_debug->printf("AX12_E_LEN\n");
        else if (ret == AX12_E_CHECKSUM)
            g_debug->printf("AX12_E_CHECKSUM\n");
        else
        {
            g_debug->printf("Error AX12:\n");

            ret = -ret;
            g_debug->printf("\tInput voltage %d\n", !!(ret & (1 << 0)));
            g_debug->printf("\tAngle limit   %d\n", !!(ret & (1 << 1)));
            g_debug->printf("\tOverheating   %d\n", !!(ret & (1 << 2)));
            g_debug->printf("\tRange         %d\n", !!(ret & (1 << 3)));
            g_debug->printf("\tChecksum      %d\n", !!(ret & (1 << 4)));
            g_debug->printf("\tOverload      %d\n", !!(ret & (1 << 5)));
            g_debug->printf("\tInstruction   %d\n", !!(ret & (1 << 6)));
            g_debug->printf("\tNone          %d\n", !!(ret & (1 << 7)));
            ret = -ret;
        }
    }

g_debug->printf("\n");
    return ret;
};

int AX12::_read(uint8_t id, uint8_t instruction_len, uint8_t instruction_cmd, uint8_t addr, uint8_t read_len) {
    uint8_t data[5] = {id, instruction_len, instruction_cmd, addr, read_len};
    return _send_request_with_retries(5, data);
};

int AX12::_write(uint8_t id, uint8_t instruction_len, uint8_t instruction_cmd, uint8_t addr, uint8_t data1) {
    uint8_t data[5] = {id, instruction_len, instruction_cmd, addr, data1};
    return _send_request_with_retries(5, data);
};

int AX12::_write(uint8_t id, uint8_t instruction_len, uint8_t instruction_cmd, uint8_t addr, uint8_t data1, uint8_t data2) {
    uint8_t data[6] = {id, instruction_len, instruction_cmd, addr, data1, data2};
    return _send_request_with_retries(6, data);
};

int AX12::send_ping(uint8_t id) {
    uint8_t data[3] = {id, AX12_PING_LEN, AX12_PING_CMD};
    return this->_send_request_with_retries(3, data);
};

int AX12::write_id(uint8_t id, uint8_t new_id) {
    return this->_write(id, AX12_WRITE_1B_LEN, AX12_WRITE_CMD, 0x03, new_id);
}

int AX12::read_delay(uint8_t id) {
    return this->_read(id, AX12_READ_LEN, AX12_READ_CMD, 0x05, AX12_READ_1B);
}

int AX12::write_delay(uint8_t id, uint8_t delay) {
    return this->_write(id, AX12_WRITE_1B_LEN, AX12_WRITE_CMD, 0x05, delay);
}

int AX12::read_pos(uint8_t id) {
    return this->_read(id, AX12_READ_LEN, AX12_READ_CMD, 0x24, AX12_READ_2B);  // TODO 0x24 -> reg addr of position
};

int AX12::write_pos(uint8_t id, uint16_t pos) {
    return this->_write(id, AX12_WRITE_2B_LEN, AX12_WRITE_CMD, 0x1E, pos & 0xFF, pos >> 8);
};

int AX12::read_speed(uint8_t id) {
    return this->_read(id, AX12_READ_LEN, AX12_READ_CMD, 0x20, AX12_READ_2B);
};

int AX12::write_speed(uint8_t id, uint16_t speed) {
    return this->_write(id, AX12_WRITE_2B_LEN, AX12_WRITE_CMD, 0x20, speed & 0xFF, speed >> 8);
};

int AX12::read_baud_rate(uint8_t id) {
    return this->_read(id, AX12_READ_LEN, AX12_READ_CMD, 0x04, AX12_READ_1B);
};

int AX12::write_baud_rate(uint8_t id, uint8_t baud_rate_id) {
    return this->_write(id, AX12_WRITE_1B_LEN, AX12_WRITE_CMD, 0x04, baud_rate_id);
};

/*============================================================================*/

AX12_arm::AX12_arm(AX12 *ax12_com, uint8_t which_arm, int id_base, int id_vert, int id_horiz, PinName pin_pwm, PinName valve) :
    id_base_(id_base), id_vert_(id_vert), id_horiz_(id_horiz),
    servo_(pin_pwm), valve_(valve)
{
    ax12_ = ax12_com;
    which_arm_ = which_arm;
    servo_.period(1./50);
    this->seq_init();
};

void AX12_arm::ping_all(void) {
    ax12_->send_ping(id_base_);
    ax12_->send_ping(id_vert_);
    ax12_->send_ping(id_horiz_);
};

void AX12_arm::read_pos_all(void) {
    g_debug->printf("\t\t\t\tall pos %4d %4d %4d\n",
        ax12_->read_pos(id_base_),
        ax12_->read_pos(id_vert_),
        ax12_->read_pos(id_horiz_)
    );
};

void AX12_arm::write_pos_all(int pos1, int pos2, int pos3) {
    ax12_->write_pos(id_base_, pos1);
    ax12_->write_pos(id_vert_, pos2);
    ax12_->write_pos(id_horiz_, pos3);
};

void AX12_arm::read_speed_all(void) {
    g_debug->printf("\t\t\t\tall speed %4d %4d %4d\n",
        ax12_->read_speed(id_base_),
        ax12_->read_speed(id_vert_),
        ax12_->read_speed(id_horiz_)
    );
}

void AX12_arm::write_speed_all(uint16_t speed) {
    ax12_->write_speed(id_base_, speed);
    ax12_->write_speed(id_vert_, speed);
    ax12_->write_speed(id_horiz_, speed);
};

void AX12_arm::set_servo_on(void) {
    switch (which_arm_)
    {
        case ARM_LEFT:
            servo_.write(0.03);
            break;
        case ARM_RIGHT:
            servo_.write(0.13);
            break;
    }
}

void AX12_arm::set_servo_off(void) {
    switch (which_arm_)
    {
        case ARM_LEFT:
            servo_.write(0.12);
            break;
        case ARM_RIGHT:
            servo_.write(0.07);
            break;
    }
}

void AX12_arm::set_valve_on(void) {
    valve_ = false;
}

void AX12_arm::set_valve_off(void) {
    valve_ = true;
}

void AX12_arm::seq_init(void) {
g_debug->printf("\tinit\n");
    this->set_servo_off();
    this->set_valve_off();
    // this->arm_move_down();
}

void AX12_arm::seq_grab(void) {
g_debug->printf("\tgrab\n");
    this->set_valve_on();
    this->set_servo_on();
}

void AX12_arm::seq_move_up(void) {
g_debug->printf("\tmove up\n");
    this->set_servo_off();
    switch (which_arm_)
    {
        case ARM_LEFT:
            this->write_pos_all(360, 740, 640);
            break;
        case ARM_RIGHT:
            this->write_pos_all(430, 750, 330);
            break;
    }
}

void AX12_arm::seq_release(void) {
g_debug->printf("\trelease\n");
    this->set_valve_off();
}

void AX12_arm::seq_move_down(void) {
g_debug->printf("\tmove down\n");
    switch (which_arm_)
    {
        case ARM_LEFT:
            this->write_pos_all(450, 160, 330);
            break;
        case ARM_RIGHT:
            this->write_pos_all(370, 160, 640);
            break;
    }
}

#endif // #ifdef IAM_QENTRESORT
