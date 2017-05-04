#ifdef IAM_QENTRESORT

#ifndef AX12DRIVER_H_INCLUDED
#define AX12DRIVER_H_INCLUDED

#include "common/OrdersFIFO.h"
#include "pinout.h"


#define BAUD_RATE                       (200*1000)

#define AX12_MAX_RETRIES                3
#define AX12_SLEEP_TIME_BETWEEN_RETRIES 5  // ms

#define AX12_MOVING_SPEED               500


/*
    Commands and their lengths
*/
#define AX12_PING_CMD                   0x1
#define AX12_PING_LEN                   (2+0)
#define AX12_READ_CMD                   0x2
#define AX12_READ_LEN                   (2+2)
#define AX12_READ_1B                    1
#define AX12_READ_2B                    2
#define AX12_WRITE_CMD                  0x3
#define AX12_WRITE_1B_LEN               (3+1)
#define AX12_WRITE_2B_LEN               (3+2)
#define AX12_RESET_CMD                  0x6
#define AX12_RESET_LEN                  (2+0)

/*
    Registers
*/
// EEPROM (ro)
#define REG_ADDR_ID                     0x03    // 1 byte
#define REG_ADDR_BAUD_RATE              0x04    // 1 byte
#define REG_ADDR_RESPONSE_DELAY         0x05    // 1 byte
// RAM (rw)
#define REG_ADDR_GOAL_POSITION          0x1E    // 2 bytes
#define REG_ADDR_SPEED                  0x20    // 2 bytes
#define REG_ADDR_CURRENT_POSITION       0x24    // 2 bytes

/*
    Errors
    Note: -255 to -1 are reserved for error code returned by the AX12
*/
#define AX12_E_TIMEOUT                  (-256)
#define AX12_E_ID                       (-257)
#define AX12_E_LEN                      (-258)
#define AX12_E_CHECKSUM                 (-259)
#define AX12_E_UNHANDLED_CMD            (-260)
#define AX12_E_READ_CMD                 (-261)

#define MAX_BITS_PER_MSG_REQUEST        70
#define AX12_TIME_FOR_DATA_TO_BE_SENT   (1000*1000*MAX_BITS_PER_MSG_REQUEST/BAUD_RATE + 350)  // us
#define MAX_BITS_PER_MSG_RESPONSE       100
#define AX12_READ_TIMEOUT               (1000*1000*MAX_BITS_PER_MSG_RESPONSE/BAUD_RATE + 1000)  // us


#if AX12_TIME_FOR_DATA_TO_BE_SENT > 1000  // us
    #error "[AX12] You might miss the AX12's response."
#endif

#if AX12_READ_TIMEOUT < 1000  // us
    #error "[AX12] AX12_READ_TIMEOUT is too short, you might miss the AX12's response."
#endif

#if AX12_READ_TIMEOUT > 1500  // us
    #error "[AX12] AX12_READ_TIMEOUT is unnecessarly long. You should reduce it."
#endif


class Ax12Driver {
public:
    Ax12Driver(void);

    void set_baud(int b);

// private:
    void txrx_set_rx(void);
    void txrx_set_tx(void);

    /*
        Returns:
            > 0: return valid data for a read cmd
            == 0: ok
            between -255 and -1: error returned by the AX12
            < -255: error during reception
    */
    int __send_request(uint8_t payload_len, uint8_t *payload_data);
    int _send_request_with_retries(uint8_t len, uint8_t *data);

    int _read(uint8_t id, uint8_t instruction_len, uint8_t instruction_cmd, uint8_t addr, uint8_t read_len);
    int _write(uint8_t id, uint8_t instruction_len, uint8_t instruction_cmd, uint8_t addr, uint8_t data1);
    int _write(uint8_t id, uint8_t instruction_len, uint8_t instruction_cmd, uint8_t addr, uint8_t data1, uint8_t data2);

public:
    int send_ping(uint8_t id);

    int write_id(uint8_t id, uint8_t new_id);

    int read_delay(uint8_t id);
    int write_delay(uint8_t id, uint8_t delay);

    int read_pos(uint8_t id);
    int write_pos(uint8_t id, uint16_t pos);

    int read_speed(uint8_t id);
    /*
        max is 0x3FF (1023) which equal 114 RPM.
        0 means no limit.
    */
    int write_speed(uint8_t id, uint16_t speed);

    int read_baud_rate(uint8_t id);
    /*
        baud_rate_id == 9 => baud rate of 200 000 bps
    */
    int write_baud_rate(uint8_t id, uint8_t baud_rate_id);

private:
    BufferedSerial ax12_;
    DigitalOut txrx_select_;
};

#endif // #ifndef AX12DRIVER_H_INCLUDED
#endif // #ifdef IAM_QENTRESORT
