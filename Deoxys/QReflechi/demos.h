#ifndef DEMOS_H_INCLUDED
#define DEMOS_H_INCLUDED

#include "common/OrdersFIFO.h"

int demo_1(OrdersFIFO *oc);
int demo_2(OrdersFIFO *oc);
int demo_jpo(OrdersFIFO *oc);
int demo_360(OrdersFIFO *oc);

int demo_load(OrdersFIFO *oc, s_order_com *demo, int demo_size);

#define DEMO_TABLE_SIZE  25
extern s_order_com demo_table[DEMO_TABLE_SIZE];

#define DEMO_TEST_ENC_TURN_SIZE  60
extern s_order_com demo_test_enc_turn[DEMO_TEST_ENC_TURN_SIZE];

#endif // DEMOS_H_INCLUDED
