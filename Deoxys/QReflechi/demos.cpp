
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
