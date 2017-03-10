
#include "common/OrdersFIFO.h"

#include "config.h"

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


s_order_com demo_table[DEMO_TABLE_SIZE] = {
    OrderCom_makeAbsPos(600-30-130-100, 1000),
    OrderCom_makeAbsPos(600-30-130, 1000),
    OrderCom_makeDelay(5.1),                // devant la starting zone

    OrderCom_makeRelDist(200),
    OrderCom_makeAbsPos(990, 630),
    OrderCom_makeDelay(5.2),                // milieu du terrain

    OrderCom_makeAbsPos(1300, 270),
    OrderCom_makeAbsAngle(DEG2RAD(-85)),
    OrderCom_makeRelDist(70),
    OrderCom_makeDelay(5.3),                // fusee

    OrderCom_makeAbsPos(1200, 550),
    OrderCom_makeAbsAngle(DEG2RAD(0)),
    OrderCom_makeAbsPos(2000-150-130-10, 800-60),
    OrderCom_makeDelay(5.4),                // cylindre du fond

    OrderCom_makeAbsPos(1200, 600),
    OrderCom_makeAbsPos(1310, 770),
    OrderCom_makeDelay(5.5),                // devant le palmier

    OrderCom_makeRelDist(-150),
    OrderCom_makeAbsAngle(DEG2RAD(-45)),
    OrderCom_makeAbsPos(800, 1000),  // ~= RelDist(-450)
    OrderCom_makeAbsAngle(DEG2RAD(0)),
    OrderCom_makeAbsPos(600-30-130, 1000),
    OrderCom_makeAbsPos(MC_START_X+100, MC_START_Y),
    OrderCom_makeAbsPos(MC_START_X, MC_START_Y),
    OrderCom_makeDelay(5.6)                 // back home
};


extern s_order_com demo_test_enc_turn[DEMO_TEST_ENC_TURN_SIZE] = {
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
    OrderCom_makeRelAngle(DEG2RAD(120))
};



int demo_load(OrdersFIFO *oc, s_order_com *demo, int demo_size)
{
    int e = 0, i = 0;

    for (i = 0; i < demo_size; ++i)
        e += oc->push(demo[i]);

    return e;
}
