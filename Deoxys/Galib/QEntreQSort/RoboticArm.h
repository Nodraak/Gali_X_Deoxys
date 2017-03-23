#ifndef ROBOTIC_ARM_H_INCLUDED
#define ROBOTIC_ARM_H_INCLUDED


#define AX12_PIN_SWITCH     D13
#define AX12_PIN_TX         D1
#define AX12_PIN_RX         D0
#define AX12_PIN_SERVO      D5
#define AX12_PIN_VALVE      D12

#define AX12_PING_CMD       0x1
#define AX12_PING_LEN       (2+0)
#define AX12_READ_CMD       0x2
#define AX12_READ_LEN       (2+2)
#define AX12_READ_1B        1
#define AX12_READ_2B        2
#define AX12_WRITE_CMD      0x3
#define AX12_WRITE_1B_LEN   (3+1)
#define AX12_WRITE_2B_LEN   (3+2)
#define AX12_RESET_CMD      0x6
#define AX12_RESET_LEN      (2+0)

// errors -255 to -1 are reserved for error code returned by the AX12
#define AX12_E_TIMEOUT      (-256)
#define AX12_E_ID           (-257)
#define AX12_E_LEN          (-258)
#define AX12_E_CHECKSUM     (-259)
#define AX12_E_UNKNOWN      (-260)

#define BAUD_RATE                       (200*1000)
#define MAX_BITS_PER_MSG_REQUEST        70
#define AX12_TIME_FOR_DATA_TO_BE_SENT   (1000*1000*MAX_BITS_PER_MSG_REQUEST/BAUD_RATE + 250)  // us
#define MAX_BITS_PER_MSG_RESPONSE       100
#define AX12_READ_TIMEOUT               (1000*1000*MAX_BITS_PER_MSG_RESPONSE/BAUD_RATE + 750)  // us

#define AX12_MAX_RETRIES                5
#define AX12_SLEEP_TIME_BETWEEN_RETRIES 5  // ms
#define AX12_MAX_DELAY_DUE_TO_RETRIES   (AX12_MAX_RETRIES*AX12_SLEEP_TIME_BETWEEN_RETRIES*6)  // 4 ax12


#if AX12_TIME_FOR_DATA_TO_BE_SENT > 1000  // us
    #error "[AX12] You might miss the AX12's response."
#endif

#if AX12_READ_TIMEOUT < 1000  // us
    #error "[AX12] AX12_READ_TIMEOUT is too short, you might miss the AX12's response."
#endif

#if AX12_READ_TIMEOUT > 1500  // us
    #error "[AX12] AX12_READ_TIMEOUT is unnecessarly long. You should reduce it."
#endif

#if AX12_MAX_DELAY_DUE_TO_RETRIES > 250  // ms
    #error "[AX12] In case of bad communication to the ax12, you might spend a lot of time trying to communicate."
#endif


class AX12 {
public:
    AX12(void);

private:
    void set_baud(int baud);

    void txrx_set_rx(void);
    void txrx_set_tx(void);

    /*
        Returns:
            > 0: return valid data for a read cmd
            == 0: ok
            between -255 and -1: error returned by the AX12
            < -255: error during reception
    */
    int ___send_request(uint8_t payload_len, uint8_t *payload_data);
    int __send_request(uint8_t len, uint8_t *data);

    int _read(uint8_t id, uint8_t instruction_len, uint8_t instruction_cmd, uint8_t addr, uint8_t read_len);
    int _write(uint8_t id, uint8_t instruction_len, uint8_t instruction_cmd, uint8_t addr, uint8_t data1);
    int _write(uint8_t id, uint8_t instruction_len, uint8_t instruction_cmd, uint8_t addr, uint8_t data1, uint8_t data2);

public:
    int send_ping(uint8_t id);

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


class AX12_arm {
public:
    AX12_arm(int id_base, int id_vert1, int id_vert2, int id_vert3, int id_horiz, PinName pin_pwm, PinName valve);

    void ping_all(void);

    void read_pos_all(void);
    void write_pos_all(int pos1, int pos2, int pos3, int pos4, int pos5);

    void read_speed_all(void);
    void write_speed_all(uint16_t speed);

    void set_servo_on(void);
    void set_servo_off(void);

    void set_valve_on(void);
    void set_valve_off(void);

    void arm_move_down(void);
    void arm_move_up(void);

    void do_sequence(void);

private:
    int id_base_, id_vert1_, id_vert2_, id_vert3_, id_horiz_;
    AX12 ax12_;
    PwmOut servo_;
    DigitalOut valve_;
};

#endif  // #ifndef ROBOTIC_ARM_H_INCLUDE
