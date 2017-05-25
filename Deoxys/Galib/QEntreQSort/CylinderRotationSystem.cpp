#ifdef IAM_QENTRESORT

#include "common/utils.h"
#include "QEntreQSort/ColorSensor.h"
#include "QEntreQSort/Ax12Driver.h"

#include "QEntreQSort/CylinderRotationSystem.h"

CylinderRotationSystem::CylinderRotationSystem(
    Ax12Driver *ax12, EventQueue *queue, PinName c_green, PinName c_blue, uint8_t ax12_id, bool rotation_reversed)
{
    cs_ = new ColorSensor(c_green, c_blue);
    ax12_ = ax12;
    ax12_id_ = ax12_id;
    rotation_reversed_ = rotation_reversed;
    running_ = false;

    queue_ = queue;
    timeout_.start();
}

void CylinderRotationSystem::rotate(int angle_deg) {
    if (running_)
        return;
    running_ = true;

    ax12_->endless_turn_enable(ax12_id_, rotation_reversed_ ? AX12_DIR_CCW : AX12_DIR_CW, AX12_ROTATION_SPEED);

    timeout_.reset();
    float post_yellow_wait = 1000.0 * ONE_ROTATION_DURATION * angle_deg/360;
    queue_->call_in(0.001, callback(this, &CylinderRotationSystem::rotate_wait_trigger), post_yellow_wait);
}

void CylinderRotationSystem::rotate_wait_trigger(float post_yellow_wait) {
    // timeout -> abort now, do nothing
    if (timeout_.read_ms() > 5*1000)  // todo define timeout
        this->rotate_finish();
    // trigger (yellow) -> schedule finish in XX ms
    else if (cs_->get_val() == COLOR_YELLOW)
        queue_->call_in(post_yellow_wait, callback(this, &CylinderRotationSystem::rotate_finish));
    // else -> schedule call again in 1 ms
    else
        queue_->call_in(0.001, callback(this, &CylinderRotationSystem::rotate_wait_trigger), post_yellow_wait);
}

void CylinderRotationSystem::rotate_finish(void) {
    ax12_->endless_turn_disable(ax12_id_);
    running_ = false;
}

int CylinderRotationSystem::is_color_done(void) {
    return ! running_;
}

#endif // #ifdef IAM_QENTRESORT
