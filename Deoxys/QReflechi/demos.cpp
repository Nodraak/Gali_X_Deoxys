
#include "common/Debug.h"
#include "common/OrdersFIFO.h"
#include "QEntreQSort/Actuators.h"

#include "config.h"

#include "demos.h"


s_order_com demo_table[] = {
    OrderCom_makeAbsPos(600-30-130-50-200, 1000),
    OrderCom_makeAbsAngle(DEG2RAD(0)),
    OrderCom_makeAbsPos(600-30-130-200, 1000),
    OrderCom_makeDelay(15.1),                // devant la starting zone

    OrderCom_makeRelDist(-200),
    OrderCom_makeAbsPos(990-100, 630-100),
    OrderCom_makeDelay(15.2),                // milieu du terrain

    OrderCom_makeAbsPos(1300, 270-200),
    OrderCom_makeAbsAngle(DEG2RAD(-85)),
    OrderCom_makeRelDist(-70),
    OrderCom_makeDelay(15.3),                // fusee

    OrderCom_makeAbsPos(1200, 550),
    OrderCom_makeAbsAngle(DEG2RAD(0)),
    OrderCom_makeAbsPos(2000-150-130-10-200, 800-60),
    OrderCom_makeDelay(15.4),                // cylindre du fond

    OrderCom_makeAbsPos(1200-100, 600-100),
    OrderCom_makeAbsPos(1310-100, 770-100),
    OrderCom_makeDelay(15.5),                // devant le palmier

    OrderCom_makeRelDist(-(-150)),
    OrderCom_makeAbsAngle(DEG2RAD(-45)),
    OrderCom_makeAbsPos(800, 1000),  // ~= RelDist(-450)
    OrderCom_makeAbsAngle(DEG2RAD(0)),
    OrderCom_makeAbsPos(600-30-130, 1000),
    OrderCom_makeAbsPos(MC_START_X+100, MC_START_Y),
    OrderCom_makeAbsPos(MC_START_X, MC_START_Y),
    OrderCom_makeDelay(15.6),                // back home

    OrderCom_makeNone()
};


s_order_com demo_test_enc_turn[] = {
    OrderCom_makeRelAngle(DEG2RAD(-120)),
    OrderCom_makeRelAngle(DEG2RAD(-120)),
    OrderCom_makeRelAngle(DEG2RAD(-120)),

    OrderCom_makeRelAngle(DEG2RAD(-120)),
    OrderCom_makeRelAngle(DEG2RAD(-120)),
    OrderCom_makeRelAngle(DEG2RAD(-120)),

    OrderCom_makeRelAngle(DEG2RAD(-120)),
    OrderCom_makeRelAngle(DEG2RAD(-120)),
    OrderCom_makeRelAngle(DEG2RAD(-120)),

    OrderCom_makeRelAngle(DEG2RAD(-120)),
    OrderCom_makeRelAngle(DEG2RAD(-120)),
    OrderCom_makeRelAngle(DEG2RAD(-120)),

    OrderCom_makeRelAngle(DEG2RAD(-120)),
    OrderCom_makeRelAngle(DEG2RAD(-120)),
    OrderCom_makeRelAngle(DEG2RAD(-120)),

    OrderCom_makeRelAngle(DEG2RAD(-120)),
    OrderCom_makeRelAngle(DEG2RAD(-120)),
    OrderCom_makeRelAngle(DEG2RAD(-120)),

    OrderCom_makeRelAngle(DEG2RAD(-120)),
    OrderCom_makeRelAngle(DEG2RAD(-120)),
    OrderCom_makeRelAngle(DEG2RAD(-120)),

    OrderCom_makeRelAngle(DEG2RAD(-120)),
    OrderCom_makeRelAngle(DEG2RAD(-120)),
    OrderCom_makeRelAngle(DEG2RAD(-120)),

    OrderCom_makeRelAngle(DEG2RAD(-120)),
    OrderCom_makeRelAngle(DEG2RAD(-120)),
    OrderCom_makeRelAngle(DEG2RAD(-120)),

    OrderCom_makeRelAngle(DEG2RAD(-120)),
    OrderCom_makeRelAngle(DEG2RAD(-120)),
    OrderCom_makeRelAngle(DEG2RAD(-120)),

//--

    OrderCom_makeRelAngle(DEG2RAD(120)),
    OrderCom_makeRelAngle(DEG2RAD(120)),
    OrderCom_makeRelAngle(DEG2RAD(120)),

    OrderCom_makeRelAngle(DEG2RAD(120)),
    OrderCom_makeRelAngle(DEG2RAD(120)),
    OrderCom_makeRelAngle(DEG2RAD(120)),

    OrderCom_makeRelAngle(DEG2RAD(120)),
    OrderCom_makeRelAngle(DEG2RAD(120)),
    OrderCom_makeRelAngle(DEG2RAD(120)),

    OrderCom_makeRelAngle(DEG2RAD(120)),
    OrderCom_makeRelAngle(DEG2RAD(120)),
    OrderCom_makeRelAngle(DEG2RAD(120)),

    OrderCom_makeRelAngle(DEG2RAD(120)),
    OrderCom_makeRelAngle(DEG2RAD(120)),
    OrderCom_makeRelAngle(DEG2RAD(120)),

    OrderCom_makeRelAngle(DEG2RAD(120)),
    OrderCom_makeRelAngle(DEG2RAD(120)),
    OrderCom_makeRelAngle(DEG2RAD(120)),

    OrderCom_makeRelAngle(DEG2RAD(120)),
    OrderCom_makeRelAngle(DEG2RAD(120)),
    OrderCom_makeRelAngle(DEG2RAD(120)),

    OrderCom_makeRelAngle(DEG2RAD(120)),
    OrderCom_makeRelAngle(DEG2RAD(120)),
    OrderCom_makeRelAngle(DEG2RAD(120)),

    OrderCom_makeRelAngle(DEG2RAD(120)),
    OrderCom_makeRelAngle(DEG2RAD(120)),
    OrderCom_makeRelAngle(DEG2RAD(120)),

    OrderCom_makeRelAngle(DEG2RAD(120)),
    OrderCom_makeRelAngle(DEG2RAD(120)),
    OrderCom_makeRelAngle(DEG2RAD(120)),

    OrderCom_makeNone()
};


#define Pra         (150-60)    // pos from center to robotic arm
#define Rc          32          // cylinder radius
#define Lra         100         // robotic arm length
#define Hr2         (260/2)     // half robot height

#define l           (Pra)
#define L           (Rc + Lra + Hr2)

// 277.0
#define D           DIST(l, L)          // distance to center of cylinder
#define theta       (1.0*l/L)           // angle to turn to take the cylinder
// 19.7
#define theta_deg   (theta*180/3.14)

// todo print these stats

/*

600, 1000  // cylinder starting zone        - ok
1400, 900  // cylinder end of palmier
1100, 500  // cylinder center               - ok
1850, 800  // cylinder far away

1350, 80/2  // rocket side
80/2, 1150  // rocket starting zone
*/


s_order_com demo_table_arm[] = {

    OrderCom_makeSequence(ORDER_COM_TYPE_SEQ_ARM_INIT, ACT_SIDE_LEFT),
    OrderCom_makeSequence(ORDER_COM_TYPE_SEQ_ARM_INIT, ACT_SIDE_RIGHT),

/*
    OrderCom_makeArmMoveDown(ACT_SIDE_LEFT),
    OrderCom_makeArmGrab(ACT_SIDE_LEFT),
    OrderCom_makeArmMoveUp(ACT_SIDE_LEFT),
    OrderCom_makeArmRelease(ACT_SIDE_LEFT),
    OrderCom_makeArmMoveDown(ACT_SIDE_LEFT),

    OrderCom_makeArmMoveDown(ACT_SIDE_RIGHT),
    OrderCom_makeArmGrab(ACT_SIDE_RIGHT),
    OrderCom_makeArmMoveUp(ACT_SIDE_RIGHT),
    OrderCom_makeArmRelease(ACT_SIDE_RIGHT),
    OrderCom_makeArmMoveDown(ACT_SIDE_RIGHT),

    OrderCom_makeFlap(ACT_SIDE_LEFT | ACT_CONF_OPEN),
    OrderCom_makeProgradeDispenser(ACT_SIDE_LEFT | ACT_CONF_OPEN),

    OrderCom_makeFlap(ACT_SIDE_RIGHT | ACT_CONF_OPEN),
    OrderCom_makeProgradeDispenser(ACT_SIDE_RIGHT | ACT_CONF_OPEN),
*/

    OrderCom_makeWaitCQESFinished(),


/*
    1) Cylinder starting zone
*/
    // go in pos

    OrderCom_makeAbsPos(600-(D+50), 1000),
    OrderCom_makeAbsAngle(DEG2RAD(180)),
    OrderCom_makeAbsPos(600-(D+0), 1000),

    // take

    OrderCom_makeAbsAngle(DEG2RAD(180-theta_deg)),  // left (back-side)
    OrderCom_makeWaitCQBFinished(),
    OrderCom_makeSequence(ORDER_COM_TYPE_SEQ_ARM_GRAB, ACT_SIDE_RIGHT),

    OrderCom_makeWaitCQESFinished(),

    OrderCom_makeAbsAngle(DEG2RAD(180)),

    // load it

// todo: optimize angle/pos to move to next
    OrderCom_makeSequence(ORDER_COM_TYPE_SEQ_ARM_MOVE_UP, ACT_SIDE_RIGHT),
    OrderCom_makeSequence(ORDER_COM_TYPE_SEQ_ARM_RELEASE, ACT_SIDE_RIGHT),
    OrderCom_makeSequence(ORDER_COM_TYPE_SEQ_ARM_MOVE_DOWN, ACT_SIDE_RIGHT),

    OrderCom_makeWaitCQESFinished(),

/*
    2) Cylinder middle
*/
    // go in pos

    OrderCom_makeAbsPos(1050, 1000),
    OrderCom_makeAbsAngle(DEG2RAD(90)),
    OrderCom_makeAbsPos(1100, 500+(D+50)),
    OrderCom_makeAbsAngle(DEG2RAD(90)),
    OrderCom_makeAbsPos(1100, 500+(D+0)),

    // take

    OrderCom_makeAbsAngle(DEG2RAD(90-theta_deg)),  // left (back-side)
    OrderCom_makeWaitCQBFinished(),
    OrderCom_makeSequence(ORDER_COM_TYPE_SEQ_ARM_GRAB, ACT_SIDE_RIGHT),

    // OrderCom_makeWaitCQESFinished(),

    // load it

// todo: optimize angle/pos to move to next
    OrderCom_makeSequence(ORDER_COM_TYPE_SEQ_ARM_MOVE_UP, ACT_SIDE_RIGHT),
    OrderCom_makeSequence(ORDER_COM_TYPE_SEQ_ARM_RELEASE, ACT_SIDE_RIGHT),
    OrderCom_makeSequence(ORDER_COM_TYPE_SEQ_ARM_MOVE_DOWN, ACT_SIDE_RIGHT),

    OrderCom_makeWaitCQESFinished(),

/*
    Unload the two cylinders
*/

    OrderCom_makeAbsAngle(DEG2RAD(270)),
    OrderCom_makeAbsPos(1040, 260+80),
    OrderCom_makeAbsAngle(DEG2RAD(270)),
    OrderCom_makeAbsPos(1040, 260+0),
    OrderCom_makeWaitCQBFinished(),

// todo move in position

    OrderCom_makeSequence(ORDER_COM_TYPE_SEQ_FLAP, ACT_SIDE_RIGHT),
    OrderCom_makeSequence(ORDER_COM_TYPE_SEQ_PROGRADE_DISPENSER, ACT_SIDE_RIGHT),

    OrderCom_makeWaitCQESFinished(),

/*
    3) Rocket stacked side
*/
    // go in pos

    OrderCom_makeAbsPos(1350, 80/2+(D+50)),
    OrderCom_makeAbsAngle(DEG2RAD(90)),
    OrderCom_makeAbsPos(1350, 80/2+(D+10)),

    // grab 1

    OrderCom_makeAbsAngle(DEG2RAD(90-theta_deg)),  // left (back-side)

    OrderCom_makeWaitCQBFinished(),

    // OrderCom_makeArmGrab(ACT_SIDE_RIGHT),

    OrderCom_makeWaitCQESFinished(),

    // load 1 + grab 2

    OrderCom_makeAbsAngle(DEG2RAD(90+theta_deg)),  // right (back-side)

    OrderCom_makeWaitCQBFinished(),

    // OrderCom_makeArmGrab(ACT_SIDE_LEFT),
    OrderCom_makeSequence(ORDER_COM_TYPE_SEQ_ARM_MOVE_UP, ACT_SIDE_RIGHT),
    OrderCom_makeSequence(ORDER_COM_TYPE_SEQ_ARM_RELEASE, ACT_SIDE_RIGHT),
    OrderCom_makeSequence(ORDER_COM_TYPE_SEQ_ARM_MOVE_DOWN, ACT_SIDE_RIGHT),

    OrderCom_makeWaitCQESFinished(),

    // load 2 + grab 3

    OrderCom_makeAbsAngle(DEG2RAD(90-theta_deg)),  // left (back-side)

    OrderCom_makeWaitCQBFinished(),

    // OrderCom_makeArmGrab(ACT_SIDE_RIGHT),
    OrderCom_makeSequence(ORDER_COM_TYPE_SEQ_ARM_MOVE_UP, ACT_SIDE_LEFT),
    OrderCom_makeSequence(ORDER_COM_TYPE_SEQ_ARM_RELEASE, ACT_SIDE_LEFT),
    OrderCom_makeSequence(ORDER_COM_TYPE_SEQ_ARM_MOVE_DOWN, ACT_SIDE_LEFT),

    OrderCom_makeWaitCQESFinished(),

    // get back

    OrderCom_makeAbsAngle(DEG2RAD(90)),
    OrderCom_makeRelDist(300),

    OrderCom_makeNone()
};


s_order_com demo_basic_test[] = {
    OrderCom_makeAbsPos(500, 0),
    OrderCom_makeAbsAngle(180),
    OrderCom_makeAbsPos(250, 0),
    OrderCom_makeAbsAngle(0),
    OrderCom_makeAbsPos(0, 0),

    OrderCom_makeNone()
};


int demo_load(s_order_com *demo, OrdersFIFO *dest)
{
    s_order_com *ptr = demo;
    int e = 0;

    while (ptr->type != ORDER_COM_TYPE_NONE)
    {
        e += dest->append(*ptr);
        ++ptr;
    }

    return e;
}
