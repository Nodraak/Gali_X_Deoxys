
#include "OrdersFIFO.h"

int demo_1(OrdersFIFO *oc)
{
    #define EX_DIST 1000

    int e = 0;

    e += oc->push(OrderCom_makeAbsPos(EX_DIST,    0));
                    e += oc->push(OrderCom_makeDelay(0.500));
            e += oc->push(OrderCom_makeAbsAngle(DEG2RAD(90)));
                    e += oc->push(OrderCom_makeDelay(0.500));
    e += oc->push(OrderCom_makeAbsPos(EX_DIST,    EX_DIST));
                    e += oc->push(OrderCom_makeDelay(0.500));
            e += oc->push(OrderCom_makeAbsAngle(DEG2RAD(180)));
                    e += oc->push(OrderCom_makeDelay(0.500));
    e += oc->push(OrderCom_makeAbsPos(200,        EX_DIST));
                    e += oc->push(OrderCom_makeDelay(0.500));
            e += oc->push(OrderCom_makeAbsAngle(DEG2RAD(90)));
            e += oc->push(OrderCom_makeAbsAngle(DEG2RAD(0)));
            e += oc->push(OrderCom_makeAbsAngle(DEG2RAD(270)));
            e += oc->push(OrderCom_makeAbsAngle(DEG2RAD(180)));
                    e += oc->push(OrderCom_makeDelay(0.500));
    e += oc->push(OrderCom_makeAbsPos(EX_DIST,    EX_DIST));
                    e += oc->push(OrderCom_makeDelay(0.500));
            e += oc->push(OrderCom_makeAbsAngle(DEG2RAD(270)));
                    e += oc->push(OrderCom_makeDelay(0.500));
    e += oc->push(OrderCom_makeAbsPos(EX_DIST,    0));
                    e += oc->push(OrderCom_makeDelay(0.500));
            e += oc->push(OrderCom_makeAbsAngle(DEG2RAD(0)));
                    e += oc->push(OrderCom_makeDelay(0.500));
    e += oc->push(OrderCom_makeAbsPos(0,          0));

    return e;
}


int demo_2(OrdersFIFO *oc)
{
    #define EX_DIST 1000

    int e = 0;

    e += oc->push(OrderCom_makeRelDist(EX_DIST));
                    e += oc->push(OrderCom_makeDelay(0.500));
            e += oc->push(OrderCom_makeRelAngle(DEG2RAD(90)));
                    e += oc->push(OrderCom_makeDelay(0.500));
    e += oc->push(OrderCom_makeRelDist(EX_DIST));
                    e += oc->push(OrderCom_makeDelay(0.500));
            e += oc->push(OrderCom_makeRelAngle(DEG2RAD(90)));
                    e += oc->push(OrderCom_makeDelay(0.500));
    e += oc->push(OrderCom_makeRelDist(EX_DIST-200));
                    e += oc->push(OrderCom_makeDelay(0.500));
            e += oc->push(OrderCom_makeRelAngle(DEG2RAD(-90)));
            e += oc->push(OrderCom_makeRelAngle(DEG2RAD(-90)));
            e += oc->push(OrderCom_makeRelAngle(DEG2RAD(-90)));
            e += oc->push(OrderCom_makeRelAngle(DEG2RAD(-90)));
                    e += oc->push(OrderCom_makeDelay(0.500));
    e += oc->push(OrderCom_makeRelDist(-(EX_DIST-200)));
                    e += oc->push(OrderCom_makeDelay(0.500));
            e += oc->push(OrderCom_makeRelAngle(DEG2RAD(90)));
                    e += oc->push(OrderCom_makeDelay(0.500));
    e += oc->push(OrderCom_makeRelDist(EX_DIST));
                    e += oc->push(OrderCom_makeDelay(0.500));
            e += oc->push(OrderCom_makeRelAngle(DEG2RAD(90)));
                    e += oc->push(OrderCom_makeDelay(0.500));
    e += oc->push(OrderCom_makeRelDist(-EX_DIST));

    return e;
}

