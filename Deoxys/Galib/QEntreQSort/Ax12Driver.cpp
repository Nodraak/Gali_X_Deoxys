#ifdef IAM_QENTRESORT

/*
void BufferedSerial::clear(void) {
    _rxbuf.clear();
}
*/

#include "mbed.h"
#include "BufferedSerial.h"

#include "common/Debug.h"
#include "QEntreQSort/Actuators.h"

#include "config.h"

#include "QEntreQSort/Ax12Driver.h"

extern Debug *g_debug;


Ax12Driver::Ax12Driver(void) : ax12_(AX12_PIN_TX, AX12_PIN_RX), txrx_select_(AX12_PIN_SWITCH) {
    this->txrx_set_rx();
    ax12_.baud(BAUD_RATE);

// todo move this
g_debug->printf("AX12_TIME_FOR_DATA_TO_BE_SENT %d\n", AX12_TIME_FOR_DATA_TO_BE_SENT);
g_debug->printf("AX12_READ_TIMEOUT %d\n", AX12_READ_TIMEOUT);
}

void Ax12Driver::set_baud(int b) {
    ax12_.baud(b);
}

void Ax12Driver::txrx_set_rx(void) {
    txrx_select_ = false;
}

void Ax12Driver::txrx_set_tx(void) {
    txrx_select_ = true;
}

int Ax12Driver::__send_request(uint8_t payload_len, uint8_t *payload_data) {
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
}

int Ax12Driver::_send_request_with_retries(uint8_t len, uint8_t *data) {
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
}

int Ax12Driver::_read(uint8_t id, uint8_t instruction_len, uint8_t instruction_cmd, uint8_t addr, uint8_t read_len) {
    uint8_t data[5] = {id, instruction_len, instruction_cmd, addr, read_len};
    return _send_request_with_retries(5, data);
}

int Ax12Driver::_write(uint8_t id, uint8_t instruction_len, uint8_t instruction_cmd, uint8_t addr, uint8_t data1) {
    uint8_t data[5] = {id, instruction_len, instruction_cmd, addr, data1};
    return _send_request_with_retries(5, data);
}

int Ax12Driver::_write(uint8_t id, uint8_t instruction_len, uint8_t instruction_cmd, uint8_t addr, uint8_t data1, uint8_t data2) {
    uint8_t data[6] = {id, instruction_len, instruction_cmd, addr, data1, data2};
    return _send_request_with_retries(6, data);
}

int Ax12Driver::send_ping(uint8_t id) {
    uint8_t data[3] = {id, AX12_PING_LEN, AX12_PING_CMD};
    return this->_send_request_with_retries(3, data);
}

int Ax12Driver::write_id(uint8_t id, uint8_t new_id) {
    return this->_write(id, AX12_WRITE_1B_LEN, AX12_WRITE_CMD, REG_ADDR_ID, new_id);
}

int Ax12Driver::read_delay(uint8_t id) {
    return this->_read(id, AX12_READ_LEN, AX12_READ_CMD, REG_ADDR_RESPONSE_DELAY, AX12_READ_1B);
}

int Ax12Driver::write_delay(uint8_t id, uint8_t delay) {
    return this->_write(id, AX12_WRITE_1B_LEN, AX12_WRITE_CMD, REG_ADDR_RESPONSE_DELAY, delay);
}

int Ax12Driver::read_pos(uint8_t id) {
    return this->_read(id, AX12_READ_LEN, AX12_READ_CMD, REG_ADDR_CURRENT_POSITION, AX12_READ_2B);
}

int Ax12Driver::write_pos(uint8_t id, uint16_t pos) {
    return this->_write(id, AX12_WRITE_2B_LEN, AX12_WRITE_CMD, REG_ADDR_GOAL_POSITION, pos & 0xFF, pos >> 8);
}

int Ax12Driver::read_speed(uint8_t id) {
    return this->_read(id, AX12_READ_LEN, AX12_READ_CMD, REG_ADDR_SPEED, AX12_READ_2B);
}

int Ax12Driver::write_speed(uint8_t id, uint16_t speed) {
    return this->_write(id, AX12_WRITE_2B_LEN, AX12_WRITE_CMD, REG_ADDR_SPEED, speed & 0xFF, speed >> 8);
}

int Ax12Driver::read_baud_rate(uint8_t id) {
    return this->_read(id, AX12_READ_LEN, AX12_READ_CMD, REG_ADDR_BAUD_RATE, AX12_READ_1B);
}

int Ax12Driver::write_baud_rate(uint8_t id, uint8_t baud_rate_id) {
    return this->_write(id, AX12_WRITE_1B_LEN, AX12_WRITE_CMD, REG_ADDR_BAUD_RATE, baud_rate_id);
}

void Ax12Driver::endless_turn_enable(uint8_t id, uint8_t direction, uint16_t speed) {
    this->_write(id, AX12_WRITE_2B_LEN, AX12_WRITE_CMD, REG_ADDR_CW_ANGLE_LIM, 0, 0);
    this->_write(id, AX12_WRITE_2B_LEN, AX12_WRITE_CMD, REG_ADDR_CCW_ANGLE_LIM, 0, 0);
    this->write_speed(id, ((direction << 10) | speed));
}

void Ax12Driver::endless_turn_disable(uint8_t id) {
    uint8_t speed_l = CW_ANGLE_LIM_DEFAULT & 0xFF, speed_h = CW_ANGLE_LIM_DEFAULT >> 8;

    this->write_speed(id, 0);
    this->_write(id, AX12_WRITE_2B_LEN, AX12_WRITE_CMD, REG_ADDR_CW_ANGLE_LIM, speed_l, speed_h);
    this->_write(id, AX12_WRITE_2B_LEN, AX12_WRITE_CMD, REG_ADDR_CCW_ANGLE_LIM, speed_l, speed_h);
}

#endif // #ifdef IAM_QENTRESORT
