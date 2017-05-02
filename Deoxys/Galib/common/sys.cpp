
#include "common/Debug.h"

#include "common/sys.h"


void sys_print_reset_source(Debug *debug)
{
    debug->printf("Reset source:\n");

    if (__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST))
        debug->printf("\tRCC_FLAG_BORRST    BOR reset\n");
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_OBLRST))
        debug->printf("\tRCC_FLAG_OBLRST    Option Byte Load reset\n");
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST))
        debug->printf("\tRCC_FLAG_PINRST    Pin reset\n");
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_FWRST))
        debug->printf("\tRCC_FLAG_FWRST     FIREWALL reset\n");
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_RMVF))
        debug->printf("\tRCC_FLAG_RMVF      Remove reset Flag\n");
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST))
        debug->printf("\tRCC_FLAG_SFTRST    Software reset\n");
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST))
        debug->printf("\tRCC_FLAG_IWDGRST   Independent Watchdog reset\n");
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST))
        debug->printf("\tRCC_FLAG_WWDGRST   Window Watchdog reset\n");
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST))
        debug->printf("\tRCC_FLAG_LPWRRST   Low Power reset\n");
    debug->printf("\t-\n");

    debug->printf("\n");
}


void sys_interrupt_priorities_init(void)
{
    /*
        There are 16 levels of (pre-emption) priority (4 bits) and 0 level of subpriority.
        0 is highest, 15 is lowest priority.
    */

    NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);  // 4 bits for pre-emption priority, 0 bit for subpriority

#define _SetPriority(IRQn, PreemptPriority) NVIC_SetPriority(IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), PreemptPriority, 0));

#ifdef IAM_QBOUGE
    // encoders input (DigitalIn interrupt)
    _SetPriority(EXTI0_IRQn, 1);
    _SetPriority(EXTI1_IRQn, 1);
    _SetPriority(EXTI2_IRQn, 1);
    _SetPriority(EXTI3_IRQn, 1);
#endif

    // pwm outputs (mbed PwmOut via TIM1)
    _SetPriority(TIM1_BRK_TIM15_IRQn, 2);
    _SetPriority(TIM1_UP_TIM16_IRQn, 2);
    _SetPriority(TIM1_TRG_COM_IRQn, 2);
    _SetPriority(TIM1_CC_IRQn, 2);

#ifdef IAM_QBOUGE
    // asserv (mbed ticker (ISR) via TIM2)
    _SetPriority(TIM2_IRQn, 3);
#endif

    // CAN bus
    _SetPriority(CAN1_TX_IRQn, 10);     // transmit
    _SetPriority(CAN1_RX0_IRQn, 10);    // receive fifo0
    _SetPriority(CAN1_RX1_IRQn, 10);    // receive fifo1
    _SetPriority(CAN1_SCE_IRQn, 10);    // error

    // U(S)ART
    _SetPriority(USART1_IRQn, 11);
    _SetPriority(USART2_IRQn, 11);
}
