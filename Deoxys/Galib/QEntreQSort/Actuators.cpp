#ifdef IAM_QENTRESORT

#include "mbed.h"
#include "common/Debug.h"
#include "common/OrdersFIFO.h"
#include "QEntreQSort/Ax12Driver.h"

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

void ServoActuator::set(t_act act, char *val) {
    if (act & ACT_CONF_EXTENDED)
    {
        extended_ = atof(val);
        this->extend();
    }
    if (act & ACT_CONF_RETRACTED)
    {
        retracted_ = atof(val);
        this->retract();
    }
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

Ax12Actuator::Ax12Actuator(const char *name, Ax12Driver *ax12, uint8_t id, uint16_t retracted, uint16_t extended) :
    name_(name), ax12_(ax12), id_(id), retracted_(retracted), extended_(extended)
{
    this->retract();
}

void Ax12Actuator::print(Debug *debug, int depth) {
    int i = 0;

    for (i = 0; i < depth; ++i)
        debug->printf("\t");

    debug->printf("%-6s %4d %4d %2d (r e i)\n", name_, retracted_, extended_, id_);
}

void Ax12Actuator::set(t_act act, char *val) {
    if (act & ACT_CONF_EXTENDED)
    {
        extended_ = atoi(val);
        this->extend();
    }
    if (act & ACT_CONF_RETRACTED)
    {
        retracted_ = atoi(val);
        this->retract();
    }
}

void Ax12Actuator::extend(void) {
    ax12_->write_pos(id_, extended_);
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

void BooleanActuator::set(t_act act, char *val) {
    if (act & ACT_CONF_EXTENDED)
    {
        extended_ = bool(atoi(val));
        this->extend();
    }
    if (act & ACT_CONF_RETRACTED)
    {
        retracted_ = bool(atoi(val));
        this->extend();
    }
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
    this->init();
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

void ArmActuator::set(t_act act, char *val) {
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

void ArmActuator::init(void) {
    this->move_down();
    this->release();
}

void ArmActuator::grab(void) {
    pump_.on();
    clamp_.close();
}

void ArmActuator::move_up(void) {
    height_.retract();
    vert_.retract();
    horiz_.retract();
}

void ArmActuator::release(void) {
    clamp_.open();
    pump_.off();
}

void ArmActuator::move_down(void) {
    height_.extend();
    vert_.extend();
    horiz_.extend();
}

/*
    ## OneSide
*/

OneSideCylindersActuators::OneSideCylindersActuators(
    const char *name,
    ArmActuator arm,
    ServoActuator flap
) : name_(name), arm_(arm), flap_(flap)
{
    flap_.close();
}

void OneSideCylindersActuators::print(Debug *debug, int depth) {
    int i = 0;

    for (i = 0; i < depth; ++i)
        debug->printf("\t");
    debug->printf("%s\n", name_);

    arm_.print(debug, depth+1);
    flap_.print(debug, depth+1);
}

void OneSideCylindersActuators::set(t_act act, char *val) {
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
}

/*
    # High level
*/

/*
    ## Actuators
*/

Actuators::Actuators(
    OneSideCylindersActuators left,
    OneSideCylindersActuators right,
    ServoActuator prograde_dispenser
) : left_(left), right_(right), prograde_dispenser_(prograde_dispenser)
{
    prograde_dispenser_.close();
}

void Actuators::print(Debug *debug, int depth) {
    int i = 0;

    for (i = 0; i < depth; ++i)
        debug->printf("\t");
    debug->printf("Actuators\n");

    left_.print(debug, depth+1);
    right_.print(debug, depth+1);
    prograde_dispenser_.print(debug, depth+1);
}

void Actuators::set(t_act act, char *val) {
    if (act & ACT_SIDE_LEFT)
        left_.set(act, val);
    if (act & ACT_SIDE_RIGHT)
        right_.set(act, val);
    if (act & ACT_ACTUATOR_PROG)
        prograde_dispenser_.set(act, val);
}

OneSideCylindersActuators *Actuators::side(t_act side) {
    if (side & ACT_SIDE_LEFT)
        return &left_;
    if (side & ACT_SIDE_RIGHT)
        return &right_;

    return NULL;
}

#endif // #ifdef IAM_QENTRESORT
