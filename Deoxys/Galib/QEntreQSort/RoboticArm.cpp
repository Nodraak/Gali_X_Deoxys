
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
    this->set_baud(BAUD_RATE);

g_debug->printf("AX12_TIME_FOR_DATA_TO_BE_SENT %d\n", AX12_TIME_FOR_DATA_TO_BE_SENT);
g_debug->printf("AX12_READ_TIMEOUT %d\n", AX12_READ_TIMEOUT);
};

void AX12::set_baud(int baud) {
    ax12_.baud(baud);
}

void AX12::txrx_set_rx(void) {
    txrx_select_ = false;
}

void AX12::txrx_set_tx(void) {
    txrx_select_ = true;
}

int AX12::___send_request(uint8_t payload_len, uint8_t *payload_data) {
    uint8_t checksum = 0, i = 0;

    // general fields
    uint8_t id = payload_data[0];
    uint8_t len = payload_data[1];
    uint8_t instruction = payload_data[2];
    // cmd specific fields
    uint8_t read_cmd_read_len = payload_data[4];  // todo move me at top of function

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
    ax12_.clear();
    txrx_set_rx();

    // wait for response and parse it

    Timer t;
    t.start();
    uint8_t last_byte = 0, cur_byte = 0;
    while (t.read_us() < AX12_READ_TIMEOUT)
    {
        cur_byte = ax12_.getc();
        if ((last_byte == 0xFF) && (cur_byte == 0xFF))
        {
            uint8_t r_argv[10];
            uint8_t i = 0, checksum = 0;

            uint8_t r_id = ax12_.getc();
            checksum += r_id;
            uint8_t r_len = ax12_.getc();
            checksum += r_len;
            uint8_t r_error = ax12_.getc();
            checksum += r_error;
            for (i = 0; i < r_len-2; ++i)  // skip 1st which is r_error, and last, which is checksum
            {
                r_argv[i] = ax12_.getc();
                checksum += r_argv[i];
            }
            uint8_t r_checksum = ax12_.getc();

            checksum = ~ checksum;

            uint8_t expected_len = len;
            if ((instruction == AX12_PING_CMD) || (instruction == AX12_WRITE_CMD))
                expected_len = 2;  // status packet: fixed length
            else if (instruction == AX12_READ_CMD)
                expected_len = 2 + read_cmd_read_len;  // variable packet-lenth
            else
                expected_len = 255;  // todo error unhandled cmd

            if (r_id != id)
                return AX12_E_ID;
            if (r_len != expected_len)
                return AX12_E_LEN;
            if (r_checksum != checksum)
                return AX12_E_CHECKSUM;

            if (r_error)
                return -r_error;
            else
            {
                if (instruction != AX12_READ_CMD)
                    return 0;
                else
                {
                    if (read_cmd_read_len == AX12_READ_1B)
                        return r_argv[0];
                    else if (read_cmd_read_len == AX12_READ_2B)
                        return (r_argv[1] << 8) | r_argv[0];
                    else
                        return AX12_E_UNKNOWN;  // todo error read cmd (len)
                }
            }
        }

        last_byte = cur_byte;
    }
    return AX12_E_TIMEOUT;
};

int AX12::__send_request(uint8_t len, uint8_t *data) {
    int i = 0,  ret = 0;
    for (i = 0; i < AX12_MAX_RETRIES; ++i)
    {
        ret = this->___send_request(len, data);
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
    return __send_request(5, data);
};

int AX12::_write(uint8_t id, uint8_t instruction_len, uint8_t instruction_cmd, uint8_t addr, uint8_t data1) {
    uint8_t data[5] = {id, instruction_len, instruction_cmd, addr, data1};
    return __send_request(5, data);
};

int AX12::_write(uint8_t id, uint8_t instruction_len, uint8_t instruction_cmd, uint8_t addr, uint8_t data1, uint8_t data2) {
    uint8_t data[6] = {id, instruction_len, instruction_cmd, addr, data1, data2};
    return __send_request(6, data);
};

int AX12::send_ping(uint8_t id) {
    uint8_t data[3] = {id, AX12_PING_LEN, AX12_PING_CMD};
    return this->__send_request(3, data);
};

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

int AX12::write_speed(uint8_t id, uint16_t speed) {
    return this->_write(id, AX12_WRITE_2B_LEN, AX12_WRITE_CMD, 0x20, speed & 0xFF, speed >> 8);
};

int AX12::read_baud_rate(uint8_t id) {
    return this->_read(id, AX12_READ_LEN, AX12_READ_CMD, 0x04, AX12_READ_1B);
};

int AX12::write_baud_rate(uint8_t id, uint8_t baud_rate_id) {
    return this->_write(id, AX12_WRITE_1B_LEN, AX12_WRITE_CMD, 0x04, baud_rate_id, 200);
};

/*============================================================================*/

AX12_arm::AX12_arm(int id_base, int id_vert1, int id_vert2, int id_vert3, int id_horiz, PinName pin_pwm, PinName valve) :
    id_base_(id_base), id_vert1_(id_vert1), id_vert2_(id_vert2), id_vert3_(id_vert3), id_horiz_(id_horiz),
    servo_(pin_pwm), valve_(valve) {

    servo_.period(1./50);

    this->set_servo_off();
    this->set_valve_off();
    this->arm_move_down();
};

void AX12_arm::ping_all(void) {
    ax12_.send_ping(id_base_);
    ax12_.send_ping(id_vert1_);
    ax12_.send_ping(id_vert2_);
    ax12_.send_ping(id_vert3_);
    ax12_.send_ping(id_horiz_);
};

void AX12_arm::read_pos_all(void) {
    g_debug->printf("\t\t\t\tall pos %4d %4d %4d %4d %4d\n",
        ax12_.read_pos(id_base_),
        ax12_.read_pos(id_vert1_),
        ax12_.read_pos(id_vert2_),
        ax12_.read_pos(id_vert3_),
        ax12_.read_pos(id_horiz_)
    );
};

void AX12_arm::write_pos_all(int pos1, int pos2, int pos3, int pos4, int pos5) {
    ax12_.write_pos(id_base_, pos1);
    ax12_.write_pos(id_vert1_, pos2);
    ax12_.write_pos(id_vert2_, pos3);
    ax12_.write_pos(id_vert3_, pos4);
    ax12_.write_pos(id_horiz_, pos5);
};

void AX12_arm::write_speed_all(uint16_t speed) {
    ax12_.write_speed(id_base_, speed);
    ax12_.write_speed(id_vert1_, speed);
    ax12_.write_speed(id_vert2_, speed);
    ax12_.write_speed(id_vert3_, speed);
    ax12_.write_speed(id_horiz_, speed);
};

void AX12_arm::set_servo_on(void) {
    servo_.write(0.03);
}

void AX12_arm::set_servo_off(void) {
    servo_.write(0.12);
}

void AX12_arm::set_valve_on(void) {
    valve_ = true;
}

void AX12_arm::set_valve_off(void) {
    valve_ = false;
}

void AX12_arm::arm_move_down(void) {
    this->write_pos_all(520, 630, 50, 410, 330);
}
void AX12_arm::arm_move_up(void) {
    this->write_pos_all(520, 800, 830, 410, 10);
}

void AX12_arm::do_sequence(void) {
    int sleep = 2000;

g_debug->printf("do_sequence:\n");

    // init
g_debug->printf("\tset_servo_off\n");
    this->set_servo_off();
g_debug->printf("\tset_valve_on\n");
    this->set_valve_on();
g_debug->printf("\tarm_move_down\n");
    this->arm_move_down();
g_debug->printf("\twait_ms\n");
    wait_ms(sleep);

    // grab
g_debug->printf("\tset_servo_on\n");
    this->set_servo_on();
g_debug->printf("\twait_ms\n");
    wait_ms(500);  // just enough for the servo to move and the valve to catch
g_debug->printf("\tset_servo_off\n");
    this->set_servo_off();
g_debug->printf("\twait_ms\n");
    wait_ms(sleep);

    // move up
g_debug->printf("\twrite_pos_all\n");
    this->write_pos_all(520, 630, 400, 410, 170);
g_debug->printf("\twait_ms\n");
    wait_ms(sleep);
g_debug->printf("\tarm_move_up\n");
    this->arm_move_up();
g_debug->printf("\twait_ms\n");
    wait_ms(sleep);

    // leave
g_debug->printf("\tset_valve_off\n");
    this->set_valve_off();
g_debug->printf("\twait_ms\n");
    wait_ms(sleep);

    // go back in starting pos
g_debug->printf("\twrite_pos_all\n");
    this->write_pos_all(520, 630, 400, 410, 170);
g_debug->printf("\twait_ms\n");
    wait_ms(sleep);
g_debug->printf("\tarm_move_down\n");
    this->arm_move_down();
g_debug->printf("\twait_ms\n");
    wait_ms(sleep);
}
