#ifdef IAM_QENTRESORT

#include "mbed.h"
#include "common/Debug.h"
#include "common/OrdersFIFO.h"
#include "QEntreQSort/Ax12Driver.h"
#include "QEntreQSort/CylinderRotationSystem.h"

#include "config.h"

#include "QEntreQSort/Actuators.h"


/*
    # Low level drivers
*/

/*
    ## Servo
*/

ServoActuator::ServoActuator(const char *name, PwmOut servo, float retracted, float extended) :
    name_(name), servo_(servo), retracted_(retracted), extended_(extended)
{
    servo_.period(SERVO_PWM_PERIOD);
    this->retract();
}

void ServoActuator::print(Debug *debug, int depth) {
    int i = 0;

    for (i = 0; i < depth; ++i)
        debug->printf("\t");

    debug->printf("%-6s %4.2f %4.2f    (r e)\n", name_, retracted_, extended_);
}

void ServoActuator::set(t_act act, float val) {
    if (act & ACT_STATE_EXTENDED)
    {
        extended_ = float(val);
        this->extend();
    }
    if (act & ACT_STATE_RETRACTED)
    {
        retracted_ = float(val);
        this->retract();
    }
}

void ServoActuator::activate(t_act act) {
    if (act & ACT_STATE_EXTENDED)
        this->extend();
    if (act & ACT_STATE_RETRACTED)
        this->retract();
}

void ServoActuator::extend(void) {
    servo_.write(extended_);
}
void ServoActuator::retract(void) {
    servo_.write(retracted_);
}

void ServoActuator::open(void) {
    this->extend();
}
void ServoActuator::close(void) {
    this->retract();
}


/*
    ## Ax12
*/

Ax12Actuator::Ax12Actuator(const char *name, Ax12Driver *ax12, uint8_t id, uint16_t retracted, uint16_t neutral, uint16_t extended) :
    name_(name), ax12_(ax12), id_(id), retracted_(retracted), neutral_(neutral), extended_(extended)
{
    // this->retract();
}

void Ax12Actuator::print(Debug *debug, int depth) {
    int i = 0;

    for (i = 0; i < depth; ++i)
        debug->printf("\t");

    debug->printf("%-6s %4d %4d %2d (r e i)\n", name_, retracted_, extended_, id_);
}

void Ax12Actuator::set(t_act act, float val) {
    if (act & ACT_STATE_EXTENDED)
    {
        extended_ = uint16_t(val);
        this->extend();
    }
    if (act & ACT_STATE_NEUTRAL)
    {
        extended_ = uint16_t(val);
        this->neutral();
    }
    if (act & ACT_STATE_RETRACTED)
    {
        retracted_ = uint16_t(val);
        this->retract();
    }
}

void Ax12Actuator::activate(t_act act) {
    if (act & ACT_STATE_EXTENDED)
        this->extend();
    if (act & ACT_STATE_NEUTRAL)
        this->neutral();
    if (act & ACT_STATE_RETRACTED)
        this->retract();
}

void Ax12Actuator::extend(void) {
    ax12_->write_pos(id_, extended_);
}

void Ax12Actuator::neutral(void) {
    ax12_->write_pos(id_, neutral_);
}

void Ax12Actuator::retract(void) {
    ax12_->write_pos(id_, retracted_);
}

void Ax12Actuator::ping(void) {
    ax12_->send_ping(id_);
}

uint16_t Ax12Actuator::read_pos(void) {
    return ax12_->read_pos(id_);
}

void Ax12Actuator::write_pos(uint16_t pos) {
    ax12_->write_pos(id_, pos);
}

uint16_t Ax12Actuator::read_speed(void) {
    return ax12_->read_speed(id_);
}

void Ax12Actuator::write_speed(uint16_t speed) {
    ax12_->write_speed(id_, speed);
}

/*
    ## Boolean
*/

BooleanActuator::BooleanActuator(const char *name, DigitalOut out, bool retracted, bool extended) :
    name_(name), out_(out), retracted_(retracted), extended_(extended)
{
    this->retract();
}

void BooleanActuator::print(Debug *debug, int depth) {
    int i = 0;

    for (i = 0; i < depth; ++i)
        debug->printf("\t");

    debug->printf("%-6s %4d %4d    (r e)\n", name_, retracted_, extended_);
}

void BooleanActuator::set(t_act act, float val) {
    if (act & ACT_STATE_EXTENDED)
    {
        extended_ = bool(val);
        this->extend();
    }
    if (act & ACT_STATE_RETRACTED)
    {
        retracted_ = bool(val);
        this->retract();
    }
}

void BooleanActuator::activate(t_act act) {
    if (act & ACT_STATE_EXTENDED)
        this->extend();
    if (act & ACT_STATE_RETRACTED)
        this->retract();
}

void BooleanActuator::extend(void) {
    out_.write(extended_);
}

void BooleanActuator::retract(void) {
    out_.write(retracted_);
}

void BooleanActuator::on(void) {
    this->extend();
}

void BooleanActuator::off(void) {
    this->retract();
}


/*
    # Medium level drivers
*/

/*
    ## Arm
*/

ArmActuator::ArmActuator(
    Ax12Actuator height,
    Ax12Actuator vert,
    Ax12Actuator horiz,
    ServoActuator clamp,
    BooleanActuator pump
) : height_(height), vert_(vert), horiz_(horiz), clamp_(clamp), pump_(pump) {
    // height_.extend();
    // vert_.extend();
    // horiz_.extend();
    clamp_.open();
    pump_.off();
}

void ArmActuator::print(Debug *debug, int depth) {
    int i = 0;

    for (i = 0; i < depth; ++i)
        debug->printf("\t");
    debug->printf("Arm\n");

    height_.print(debug, depth+1);
    vert_.print(debug, depth+1);
    horiz_.print(debug, depth+1);
    clamp_.print(debug, depth+1);
    pump_.print(debug, depth+1);
}

void ArmActuator::set(t_act act, float val) {
    if (act & ACT_ACTUATOR_HEIGHT)
        height_.set(act, val);
    if (act & ACT_ACTUATOR_VERT)
        vert_.set(act, val);
    if (act & ACT_ACTUATOR_HORIZ)
        horiz_.set(act, val);
    if (act & ACT_ACTUATOR_CLAMP)
        clamp_.set(act, val);
    if (act & ACT_ACTUATOR_PUMP)
        pump_.set(act, val);
}

void ArmActuator::activate(t_act act) {
    if (act & ACT_ACTUATOR_HEIGHT)
        height_.activate(act);
    if (act & ACT_ACTUATOR_VERT)
        vert_.activate(act);
    if (act & ACT_ACTUATOR_HORIZ)
        horiz_.activate(act);
    if (act & ACT_ACTUATOR_CLAMP)
        clamp_.activate(act);
    if (act & ACT_ACTUATOR_PUMP)
        pump_.activate(act);
}

void ArmActuator::ping_all(void) {
    height_.ping();
    vert_.ping();
    horiz_.ping();
}

void ArmActuator::read_pos_all(Debug *debug) {
    debug->printf("\t\t\t\tall pos %4d %4d %4d\n",
        height_.read_pos(),
        vert_.read_pos(),
        horiz_.read_pos()
    );
}

void ArmActuator::write_pos_all(uint16_t pos1, uint16_t pos2, uint16_t pos3) {
    height_.write_pos(pos1);
    vert_.write_pos(pos2);
    horiz_.write_pos(pos3);
}


void ArmActuator::read_speed_all(Debug *debug) {
    g_debug->printf("\t\t\t\tall speed %4d %4d %4d\n",
        height_.read_speed(),
        vert_.read_speed(),
        horiz_.read_speed()
    );
}

void ArmActuator::write_speed_all(uint16_t speed) {
    height_.write_speed(speed);
    vert_.write_speed(speed);
    horiz_.write_speed(speed);
}


/*
    ## OneSide
*/

OneSideCylindersActuators::OneSideCylindersActuators(
    const char *name,
    ArmActuator arm,
    ServoActuator flap,
    ServoActuator prograde_dispenser,
    CylinderRotationSystem crs
) : name_(name), arm_(arm), flap_(flap), prograde_dispenser_(prograde_dispenser), crs_(crs)
{
    flap_.close();
    prograde_dispenser_.close();
}

void OneSideCylindersActuators::print(Debug *debug, int depth) {
    int i = 0;

    for (i = 0; i < depth; ++i)
        debug->printf("\t");
    debug->printf("%s\n", name_);

    arm_.print(debug, depth+1);
    flap_.print(debug, depth+1);
    prograde_dispenser_.print(debug, depth+1);
    // print crs ?
}

void OneSideCylindersActuators::set(t_act act, float val) {
    if (
        (act & ACT_ACTUATOR_HEIGHT)
        || (act & ACT_ACTUATOR_VERT)
        || (act & ACT_ACTUATOR_HORIZ)
        || (act & ACT_ACTUATOR_CLAMP)
        || (act & ACT_ACTUATOR_PUMP)
    )
        arm_.set(act, val);
    if (act & ACT_ACTUATOR_FLAP)
        flap_.set(act, val);
    if (act & ACT_ACTUATOR_PROG)
        prograde_dispenser_.set(act, val);
    // set crs ?
}

void OneSideCylindersActuators::activate(t_act act) {
    if (
        (act & ACT_ACTUATOR_HEIGHT)
        || (act & ACT_ACTUATOR_VERT)
        || (act & ACT_ACTUATOR_HORIZ)
        || (act & ACT_ACTUATOR_CLAMP)
        || (act & ACT_ACTUATOR_PUMP)
    )
        arm_.activate(act);
    if (act & ACT_ACTUATOR_FLAP)
        flap_.activate(act);
    if (act & ACT_ACTUATOR_PROG)
        prograde_dispenser_.activate(act);
    if (act & ACT_ACTUATOR_COLOR)
        crs_.rotate(180);
}

int OneSideCylindersActuators::is_color_done(void) {
    return crs_.is_color_done();
}

/*
    # High level
*/

/*
    ## Actuators
*/

Actuators::Actuators(
    OneSideCylindersActuators left,
    OneSideCylindersActuators right
) : left_(left), right_(right)
{
    // nothing to do
}

/* static */ void Actuators::order_decode_sequence(OrdersFIFO *orders, e_order_com_type type, t_act act_param) {

// !! prepend in reverse order !!

    switch (type)
    {
        case ORDER_COM_TYPE_NONE:
        case ORDER_COM_TYPE_DELAY:
        case ORDER_COM_TYPE_WAIT_CQB_FINISHED:
        case ORDER_COM_TYPE_WAIT_CQES_FINISHED:
        case ORDER_COM_TYPE_WAIT_CQR_FINISHED:
        case ORDER_COM_TYPE_MOV_ABS_POS:
        case ORDER_COM_TYPE_MOV_ABS_ANGLE:
        case ORDER_COM_TYPE_MOV_REL_DIST:
        case ORDER_COM_TYPE_MOV_REL_ANGLE:
        case ORDER_COM_TYPE_ACTUATOR:
        case ORDER_COM_TYPE_LAST:
            // ignore. Here we handle only actuators related orders
            break;

        case ORDER_COM_TYPE_SEQ_ARM_INIT:
            orders->prepend(OrderCom_makeDelay(ACT_DELAY_SEQ_INIT));
            orders->prepend(OrderCom_makeActuator(act_param | ACT_ACTUATOR_PUMP | ACT_STATE_OFF));
            orders->prepend(OrderCom_makeActuator(act_param | ACT_ACTUATOR_CLAMP | ACT_STATE_OPEN));
            orders->prepend(OrderCom_makeActuator(act_param | ACT_ACTUATOR_HORIZ | ACT_STATE_EXTENDED));
            orders->prepend(OrderCom_makeActuator(act_param | ACT_ACTUATOR_VERT | ACT_STATE_EXTENDED));
            orders->prepend(OrderCom_makeActuator(act_param | ACT_ACTUATOR_HEIGHT | ACT_STATE_EXTENDED));
            break;

        case ORDER_COM_TYPE_SEQ_ARM_GRAB:
            orders->prepend(OrderCom_makeDelay(ACT_DELAY_SEQ_GRAB));
            orders->prepend(OrderCom_makeActuator(act_param | ACT_ACTUATOR_PUMP | ACT_STATE_ON));
            orders->prepend(OrderCom_makeActuator(act_param | ACT_ACTUATOR_CLAMP | ACT_STATE_CLOSED));
            break;

        case ORDER_COM_TYPE_SEQ_ARM_MOVE_UP:
            orders->prepend(OrderCom_makeDelay(ACT_DELAY_SEQ_MOVE_UP_FULL));
            orders->prepend(OrderCom_makeActuator(act_param | ACT_ACTUATOR_VERT | ACT_STATE_RETRACTED));
            orders->prepend(OrderCom_makeDelay(ACT_DELAY_SEQ_MOVE_UP_HALF));
            orders->prepend(OrderCom_makeActuator(act_param | ACT_ACTUATOR_HORIZ | ACT_STATE_RETRACTED));
            orders->prepend(OrderCom_makeActuator(act_param | ACT_ACTUATOR_VERT | ACT_STATE_NEUTRAL));
            orders->prepend(OrderCom_makeActuator(act_param | ACT_ACTUATOR_HEIGHT | ACT_STATE_RETRACTED));
            break;

        case ORDER_COM_TYPE_SEQ_ARM_RELEASE:
            orders->prepend(OrderCom_makeDelay(ACT_DELAY_SEQ_RELEASE));
            orders->prepend(OrderCom_makeActuator(act_param | ACT_ACTUATOR_PUMP | ACT_STATE_OFF));
            orders->prepend(OrderCom_makeActuator(act_param | ACT_ACTUATOR_CLAMP | ACT_STATE_OPEN));
            break;

        case ORDER_COM_TYPE_SEQ_ARM_MOVE_DOWN:
            orders->prepend(OrderCom_makeDelay(ACT_DELAY_SEQ_MOVE_DOWN_FULL));
            orders->prepend(OrderCom_makeActuator(act_param | ACT_ACTUATOR_HORIZ | ACT_STATE_EXTENDED));
            orders->prepend(OrderCom_makeActuator(act_param | ACT_ACTUATOR_VERT | ACT_STATE_EXTENDED));
            orders->prepend(OrderCom_makeActuator(act_param | ACT_ACTUATOR_HEIGHT | ACT_STATE_EXTENDED));
            orders->prepend(OrderCom_makeDelay(ACT_DELAY_SEQ_MOVE_DOWN_HALF));
            orders->prepend(OrderCom_makeActuator(act_param | ACT_ACTUATOR_VERT | ACT_STATE_NEUTRAL));
            break;

        case ORDER_COM_TYPE_SEQ_FLAP:
            orders->prepend(OrderCom_makeDelay(ACT_DELAY_SEQ_FLAP_CLOSE));
            orders->prepend(OrderCom_makeActuator(act_param | ACT_ACTUATOR_FLAP | ACT_STATE_CLOSED));
            orders->prepend(OrderCom_makeDelay(ACT_DELAY_SEQ_FLAP_OPEN));
            orders->prepend(OrderCom_makeActuator(act_param | ACT_ACTUATOR_FLAP | ACT_STATE_OPEN));
            break;

        case ORDER_COM_TYPE_SEQ_COLOR:
            orders->prepend(OrderCom_makeActuator(act_param | ACT_ACTUATOR_COLOR));
            break;

        case ORDER_COM_TYPE_SEQ_PROGRADE_DISPENSER:
            orders->prepend(OrderCom_makeDelay(ACT_DELAY_SEQ_PROG_CLOSE));
            orders->prepend(OrderCom_makeActuator(act_param | ACT_ACTUATOR_PROG | ACT_STATE_CLOSED));
            orders->prepend(OrderCom_makeDelay(ACT_DELAY_SEQ_PROG_OPEN));
            orders->prepend(OrderCom_makeActuator(act_param | ACT_ACTUATOR_PROG | ACT_STATE_OPEN));
            break;
    }
}

void Actuators::print(Debug *debug, int depth) {
    int i = 0;

    for (i = 0; i < depth; ++i)
        debug->printf("\t");
    debug->printf("Actuators\n");

    left_.print(debug, depth+1);
    right_.print(debug, depth+1);
}

void Actuators::set(t_act act, float val) {
    if (act & ACT_SIDE_LEFT)
        left_.set(act, val);
    if (act & ACT_SIDE_RIGHT)
        right_.set(act, val);
}

void Actuators::activate(t_act act) {
    if (act & ACT_SIDE_LEFT)
        left_.activate(act);
    if (act & ACT_SIDE_RIGHT)
        right_.activate(act);
}

int Actuators::is_color_done(t_act act) {
    if (act & ACT_SIDE_LEFT)
        return left_.is_color_done();
    if (act & ACT_SIDE_RIGHT)
        return right_.is_color_done();
}

#endif // #ifdef IAM_QENTRESORT
