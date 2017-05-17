#ifdef IAM_QENTRESORT

#ifndef CYLINDERROTATORSYSTEM_H_INCLUDED
#define CYLINDERROTATORSYSTEM_H_INCLUDED

#include "common/utils.h"
#include "QEntreQSort/ColorSensor.h"
#include "QEntreQSort/Ax12Driver.h"


#define AX12_SPEED_ROTATION     ((AX12_DIR_CW << 10) | 0x3FF)  // should be 114 RPM ie 2 turns per sec ie 1 sec for a full cylinder turn, but it is 2 sec :/


struct CylinderRotationSystem {
public:
    CylinderRotationSystem(
        Ax12Driver *ax12, EventQueue *queue, PinName c_green, PinName c_blue, uint8_t ax12_id
    );

    void rotate(float post_yellow_wait);

    void rotate_for_prograde_disp(void);

    void rotate_for_radial_disp(void);

private:
    void rotate_wait_trigger(float post_yellow_wait);

    void rotate_finish(void);

private:
    ColorSensor *cs_;
    Ax12Driver *ax12_;
    uint8_t ax12_id_;

    Timer timeout_;
    EventQueue *queue_;
};

#endif // #ifndef CYLINDERROTATORSYSTEM_H_INCLUDED
#endif // #ifdef IAM_QENTRESORT
