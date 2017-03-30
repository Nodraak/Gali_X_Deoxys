#ifdef IAM_QBOUGE

#include "QBouge/Qei.h"

Qei::Qei(PinName channelA, PinName channelB): channelA_(channelA), channelB_(channelB) {
    pulses_       = 0;

    //Workout what the current state is.
    //2-bit state.
    currState_ = (channelA_.read() << 1) | (channelB_.read());
    prevState_ = currState_;

    // register isr
    channelA_.rise(callback(this, &Qei::encode));
    channelA_.fall(callback(this, &Qei::encode));
    channelB_.rise(callback(this, &Qei::encode));
    channelB_.fall(callback(this, &Qei::encode));
}

int Qei::getPulses(void) {
    return pulses_;
}

void Qei::encode(void) {
    int change = 0;

    //2-bit state.
    currState_ = (channelA_.read() << 1) | (channelB_.read());

    //Entered a new valid state.
    if (((currState_ ^ prevState_) != INVALID) && (currState_ != prevState_)) {
        //2 bit state. Right hand bit of prev XOR left hand bit of current
        //gives 0 if clockwise rotation and 1 if counter clockwise rotation.
        change = (prevState_ & PREV_MASK) ^ ((currState_ & CURR_MASK) >> 1);

        if (change == 0) {
            change = -1;
        }

        pulses_ -= change;
    }

    prevState_ = currState_;
}

#endif // #ifdef IAM_QBOUGE
