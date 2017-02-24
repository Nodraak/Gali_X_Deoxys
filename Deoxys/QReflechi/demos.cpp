
#include "common/OrdersFIFO.h"
#include "demos.h"


int demo_1(OrdersFIFO *oc)
{
    int EX_DIST = 1000;
    float DEFAULT_DELAY = 2;

    int e = 0;

    e += oc->push(OrderCom_makeAbsPos(EX_DIST,    0));
                    e += oc->push(OrderCom_makeDelay(DEFAULT_DELAY));
            e += oc->push(OrderCom_makeAbsAngle(DEG2RAD(90)));
                    e += oc->push(OrderCom_makeDelay(DEFAULT_DELAY));
    e += oc->push(OrderCom_makeAbsPos(EX_DIST,    EX_DIST));
                    e += oc->push(OrderCom_makeDelay(DEFAULT_DELAY));
            e += oc->push(OrderCom_makeAbsAngle(DEG2RAD(180)));
                    e += oc->push(OrderCom_makeDelay(DEFAULT_DELAY));
    e += oc->push(OrderCom_makeAbsPos(200,        EX_DIST));
                    e += oc->push(OrderCom_makeDelay(DEFAULT_DELAY));
            e += oc->push(OrderCom_makeAbsAngle(DEG2RAD(90)));
            e += oc->push(OrderCom_makeAbsAngle(DEG2RAD(0)));
            e += oc->push(OrderCom_makeAbsAngle(DEG2RAD(270)));
            e += oc->push(OrderCom_makeAbsAngle(DEG2RAD(180)));
                    e += oc->push(OrderCom_makeDelay(DEFAULT_DELAY));
    e += oc->push(OrderCom_makeAbsPos(EX_DIST,    EX_DIST));
                    e += oc->push(OrderCom_makeDelay(DEFAULT_DELAY));
            e += oc->push(OrderCom_makeAbsAngle(DEG2RAD(270)));
                    e += oc->push(OrderCom_makeDelay(DEFAULT_DELAY));
    e += oc->push(OrderCom_makeAbsPos(EX_DIST,    0));
                    e += oc->push(OrderCom_makeDelay(DEFAULT_DELAY));
            e += oc->push(OrderCom_makeAbsAngle(DEG2RAD(0)));
                    e += oc->push(OrderCom_makeDelay(DEFAULT_DELAY));
    e += oc->push(OrderCom_makeAbsPos(0,          0));

    return e;
}


int demo_2(OrdersFIFO *oc)
{
    int EX_DIST = 1000;
    float DEFAULT_DELAY = 2;

    int e = 0;

    e += oc->push(OrderCom_makeRelDist(EX_DIST));
                    e += oc->push(OrderCom_makeDelay(DEFAULT_DELAY));
            e += oc->push(OrderCom_makeRelAngle(DEG2RAD(90)));
                    e += oc->push(OrderCom_makeDelay(DEFAULT_DELAY));
    e += oc->push(OrderCom_makeRelDist(EX_DIST));
                    e += oc->push(OrderCom_makeDelay(DEFAULT_DELAY));
            e += oc->push(OrderCom_makeRelAngle(DEG2RAD(90)));
                    e += oc->push(OrderCom_makeDelay(DEFAULT_DELAY));
    e += oc->push(OrderCom_makeRelDist(EX_DIST-200));
                    e += oc->push(OrderCom_makeDelay(DEFAULT_DELAY));
            e += oc->push(OrderCom_makeRelAngle(DEG2RAD(-90)));
            e += oc->push(OrderCom_makeRelAngle(DEG2RAD(-90)));
            e += oc->push(OrderCom_makeRelAngle(DEG2RAD(-90)));
            e += oc->push(OrderCom_makeRelAngle(DEG2RAD(-90)));
                    e += oc->push(OrderCom_makeDelay(DEFAULT_DELAY));
    e += oc->push(OrderCom_makeRelDist(-(EX_DIST-200)));
                    e += oc->push(OrderCom_makeDelay(DEFAULT_DELAY));
            e += oc->push(OrderCom_makeRelAngle(DEG2RAD(90)));
                    e += oc->push(OrderCom_makeDelay(DEFAULT_DELAY));
    e += oc->push(OrderCom_makeRelDist(EX_DIST));
                    e += oc->push(OrderCom_makeDelay(DEFAULT_DELAY));
            e += oc->push(OrderCom_makeRelAngle(DEG2RAD(90)));
                    e += oc->push(OrderCom_makeDelay(DEFAULT_DELAY));
    e += oc->push(OrderCom_makeRelDist(-EX_DIST));

    return e;
}


int demo_jpo(OrdersFIFO *oc)
{
    int EX_DIST = 700;
    float DEFAULT_DELAY = 1.0;

    int e = 0;

    e += oc->push(OrderCom_makeRelDist(EX_DIST));
    e += oc->push(OrderCom_makeDelay(DEFAULT_DELAY));
    e += oc->push(OrderCom_makeRelAngle(DEG2RAD(180)));
    e += oc->push(OrderCom_makeDelay(DEFAULT_DELAY));
    e += oc->push(OrderCom_makeRelDist(EX_DIST));
    e += oc->push(OrderCom_makeDelay(DEFAULT_DELAY));
    e += oc->push(OrderCom_makeRelAngle(DEG2RAD(180)));
    e += oc->push(OrderCom_makeDelay(DEFAULT_DELAY));

    return e;
}

int demo_360(OrdersFIFO *oc)
{
    int e = 0;

    e += oc->push(OrderCom_makeRelAngle(DEG2RAD(180)));
    e += oc->push(OrderCom_makeRelAngle(DEG2RAD(180)));

    return e;
}


#define MC_START_X          130
#define MC_START_Y          (1080-150)
#define MC_START_ANGLE      0

s_order_com demo_table[DEMO_TABLE_SIZE] = {
    OrderCom_makeAbsPos(560-130, 1020),
    OrderCom_makeAbsAngle(DEG2RAD(0)),
    OrderCom_makeDelay(2.1),                // devant la starting zone

    OrderCom_makeRelDist(250),
    OrderCom_makeAbsPos(1010, 640),
    OrderCom_makeDelay(2.2),

    OrderCom_makeAbsPos(1000+300, 240),
    OrderCom_makeAbsAngle(DEG2RAD(-70)),
    OrderCom_makeRelDist(30),
    OrderCom_makeDelay(2.3),                // fusee

    OrderCom_makeRelDist(-100),
    OrderCom_makeAbsAngle(DEG2RAD(-90)),
    OrderCom_makeRelDist(-200),
    OrderCom_makeAbsAngle(DEG2RAD(20)),
    OrderCom_makeAbsPos(2000-300, 750),
    OrderCom_makeDelay(2.4),                // cylindre du fond

    OrderCom_makeRelDist(-600),
    OrderCom_makeAbsPos(1000+320, 770),
    OrderCom_makeDelay(2.5),                // devant le palmier

    OrderCom_makeRelDist(-100),
    OrderCom_makeAbsPos(700, 1000),
    OrderCom_makeAbsAngle(DEG2RAD(0)),
    OrderCom_makeAbsPos(580-130, 1000),
    OrderCom_makeAbsPos(MC_START_X+100, MC_START_Y),
    OrderCom_makeAbsPos(MC_START_X, MC_START_Y),
    OrderCom_makeDelay(2.6)                 // back home
};


int demo_load(OrdersFIFO *oc, s_order_com *demo, int demo_size)
{
    int e = 0, i = 0;

    for (i = 0; i < demo_size; ++i)
        e += oc->push(demo[i]);

    return e;
}
