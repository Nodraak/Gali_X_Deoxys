#ifdef IAM_QENTRESORT

#include "common/utils.h"
#include "QEntreQSort/ColorSensor.h"
#include "QEntreQSort/Ax12Driver.h"

#include "QEntreQSort/CylinderRotationSystem.h"

CylinderRotationSystem::CylinderRotationSystem(
    Ax12Driver *ax12, EventQueue *queue, PinName c_green, PinName c_blue, uint8_t ax12_id)
{
    cs_ = new ColorSensor(c_green, c_blue);
    ax12_ = ax12;
    ax12_id_ = ax12_id;

    queue_ = queue;
    timeout_.start();
}

void CylinderRotationSystem::rotate(float post_yellow_wait) {
    ax12_->endless_turn_enable(ax12_id_, AX12_SPEED_ROTATION);

    timeout_.reset();
    queue_->call_in(0.001, callback(this, &CylinderRotationSystem::rotate_wait_trigger), post_yellow_wait);
}

void CylinderRotationSystem::rotate_for_prograde_disp(void) {
    this->rotate(0.0 * ONE_ROTATION_DURATION/8);
}

void CylinderRotationSystem::rotate_for_radial_disp(void) {
    this->rotate(1.5 * ONE_ROTATION_DURATION/8);
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
}

#endif // #ifdef IAM_QENTRESORT
