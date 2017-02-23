
#include "OrdersFIFO.h"

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

int demo_table(OrdersFIFO *oc)
{
    int e = 0;

    e += oc->push(OrderCom_makeAbsPos(570-130, 1020));
    e += oc->push(OrderCom_makeAbsAngle(DEG2RAD(0)));
    e += oc->push(OrderCom_makeDelay(2.0));                     // devant la starting zone

    e += oc->push(OrderCom_makeRelDist(250));
    e += oc->push(OrderCom_makeAbsPos(1000, 650));
    e += oc->push(OrderCom_makeDelay(2.0));

    e += oc->push(OrderCom_makeAbsPos(1000+300, 240));
    e += oc->push(OrderCom_makeAbsAngle(DEG2RAD(-70)));
    e += oc->push(OrderCom_makeDelay(2.0));                     // fusee

    e += oc->push(OrderCom_makeRelDist(-100));
    e += oc->push(OrderCom_makeAbsAngle(DEG2RAD(-90)));
    e += oc->push(OrderCom_makeRelDist(-300));
    e += oc->push(OrderCom_makeAbsAngle(DEG2RAD(20)));
    e += oc->push(OrderCom_makeAbsPos(2000-300, 750));
    e += oc->push(OrderCom_makeDelay(2.0));                     // cylindre du fond

// ok

    e += oc->push(OrderCom_makeRelDist(-600));
    e += oc->push(OrderCom_makeAbsPos(1000+320, 770));
    e += oc->push(OrderCom_makeDelay(2.0));                     // devant le palmier

    e += oc->push(OrderCom_makeRelDist(-150));
    e += oc->push(OrderCom_makeAbsPos(500, 1000));
    e += oc->push(OrderCom_makeAbsAngle(DEG2RAD(0)));
    e += oc->push(OrderCom_makeAbsPos(580-130, 1000));
    e += oc->push(OrderCom_makeDelay(2.0));                     // back home

    return e;
}
