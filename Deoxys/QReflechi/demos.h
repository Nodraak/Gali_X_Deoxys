#ifndef DEMOS_H_INCLUDED
#define DEMOS_H_INCLUDED

#include "common/Debug.h"
#include "common/OrdersFIFO.h"

OrdersFIFO *demo_init(void);
int demo_load(s_order_com *demo, OrdersFIFO *dest);
void demo_print(s_order_com *demo, int demo_size);

extern s_order_com demo_table[];
extern s_order_com demo_test_enc_turn[];
extern s_order_com demo_table_arm[];

#endif // DEMOS_H_INCLUDED
