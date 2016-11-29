
/*

Remember, you will need a pull-up resistor on sda and scl.

All drivers on the I2C bus are required to be open collector, and so it is
    necessary for pull up resistors to be used on the two signals. A typical
    value for the pullup resistors is around 2.2k ohms, connected between the
    pin and 3v3.

Possible speeds : (hz == 100*1000) || (hz == 400*1000) || (hz == 1000*1000)

Addr 0-15 (inclusive) and 240-255 (inclusive) are reserved

The mbed API uses 8 bit addresses, so make sure to take that 7 bit address and
left shift it by 1 before passing it.

*/

#include "mbed.h"
#include "BufferedSerial.h"

#define ADDR 0x20
#define SLAVE_ADDR_MASTER_READ ((ADDR) << 1 | 1)
#define SLAVE_ADDR_MASTER_WRITE ((ADDR) << 1 | 0)
#define SLAVE_ADDR_SLAVE (ADDR << 1)

#define I2C_FREQ (100*1000)

#define PAYLOAD_LEN 10


#ifdef BOARD_MINI

#warning "mini"
#define BOARD "mini"
#define I2C_SDA PB_7  // D4
#define I2C_SCL PB_6  // D5

BufferedSerial pc(USBTX, USBRX);
I2C i2c(I2C_SDA, I2C_SCL);
Ticker ticker;
int counter = 0;

void send(void)
{
    char s[100] = "";
    int ret = 0;

    memset(s, 0, PAYLOAD_LEN+1);
    sprintf(s, "%6d", counter);
    ret = i2c.write(SLAVE_ADDR_MASTER_WRITE, s, PAYLOAD_LEN);  // ret==0 == success
    pc.printf("[%s] send(\"%s\") == %d\n", BOARD, s, ret);

    counter ++;
}

int main(void)
{
    char buffer[100] = "";
    int ret = 0;

    pc.baud(9600);
    // pc.format(8, Serial::None, 1);
    pc.printf("[%s] Hello, world!\n", BOARD);

    i2c.frequency(I2C_FREQ);

    ticker.attach(&send, 7.000);
    wait(0.500);

    while (true)
    {
        memset(buffer, 0, PAYLOAD_LEN+1);
        ret = i2c.read(SLAVE_ADDR_MASTER_READ, buffer, PAYLOAD_LEN);  // ret==0 == success
        pc.printf("[%s] read %d \"%s\"\n", BOARD, ret, buffer);
        wait(5.000);
    }
}

#else
#ifdef BOARD_MEDIUM

#warning "medium"
#define BOARD "medium"
#define I2C_SDA PB_9  // D14
#define I2C_SCL PB_8  // D15

BufferedSerial pc(USBTX, USBRX);
I2CSlave slave(I2C_SDA, I2C_SCL);
Ticker ticker;

/*
    Around 2*10**-6 sec per call if case: NoData.
*/
void callback(void)
{
    char buf[100] = "";
    int i = 0;

    i = slave.receive();
    if (i == I2CSlave::NoData)
        return;

    memset(buf, 0, PAYLOAD_LEN+1);

    switch (i)
    {
        case I2CSlave::ReadAddressed:
            sprintf(buf, "pong");
            slave.write(buf, PAYLOAD_LEN);
            pc.printf("[%s] Read Addressed (sending \"pong\")\n", BOARD);
            break;
        case I2CSlave::WriteGeneral:
            slave.read(buf, PAYLOAD_LEN);
            pc.printf("[%s] Write General: \"%s\"\n", BOARD, buf);
            break;
        case I2CSlave::WriteAddressed:
            slave.read(buf, PAYLOAD_LEN);
            pc.printf("[%s] Write Addressed: \"%s\"\n", BOARD, buf);
            break;
        default:
            pc.printf("[%s] received %d\n", BOARD, i);
            break;
    }
}

int main(void)
{
    pc.baud(9600);
    // pc.format(8, Serial::None, 1);
    pc.printf("[%s] Hello, world!\n", BOARD);

    slave.frequency(I2C_FREQ);
    slave.address(SLAVE_ADDR_SLAVE);

    ticker.attach(&callback, 0.001);

    while (true)
    {
        pc.printf("[%s] plop\n", BOARD);

        wait(1.000);
    }
}

#else

#error "define a board please"

#endif
#endif
