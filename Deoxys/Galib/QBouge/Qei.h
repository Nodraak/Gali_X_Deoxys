/*
    Code forked from https://developer.mbed.org/users/aberk/code/QEI/
    Changes:
        Removed unused options (X2 vs X4 encoding and the index channel).
        This save 3 us (10%) per interrupt (QEI::encode()) call as well as some
        .text space.
*/

#ifndef QEI_H_INCLUDED
#define QEI_H_INCLUDED

#include "mbed.h"

#define PREV_MASK 0x1 //Mask for the previous state in determining direction of rotation.
#define CURR_MASK 0x2 //Mask for the current state in determining direction of rotation.
#define INVALID   0x3 //XORing two states where both bits have changed.

class Qei {
public:
    Qei(PinName channelA, PinName channelB);

    /**
     * Read the number of pulses recorded by the encoder.
     *
     * @return Number of pulses which have occured.
     */
    int getPulses(void);

private:

    /**
     * Update the pulse count.
     *
     * Called on every rising/falling edge of channels A/B.
     *
     * Reads the state of the channels and determines whether a pulse forward
     * or backward has occured, updating the count appropriately.
     */
    // ~ 31.474 us per call
    void encode(void);

    InterruptIn channelA_;
    InterruptIn channelB_;

    int         prevState_;
    int         currState_;

    volatile int pulses_;
};

#endif /* QEI_H */
