
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
        There are 16 levels of priority (4 bits).
        0 is highest, 15 is lowest priority.
    */

    NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);  // 4 bits for pre-emption priority, 0 bit for subpriority

#ifdef IAM_QBOUGE
    // encoders input (DigitalIn interrupt)
    NVIC_SetPriority(EXTI0_IRQn, 1);
    NVIC_SetPriority(EXTI1_IRQn, 1);
    NVIC_SetPriority(EXTI2_IRQn, 1);
    NVIC_SetPriority(EXTI3_IRQn, 1);

    // asserv (mbed ticker (ISR) via TIM2)
    NVIC_SetPriority(TIM2_IRQn, 3);
#endif

    // pwm outputs (mbed PwmOut via TIM1)
    NVIC_SetPriority(TIM1_BRK_TIM15_IRQn, 5);
    NVIC_SetPriority(TIM1_UP_TIM16_IRQn, 5);
    NVIC_SetPriority(TIM1_TRG_COM_IRQn, 5);
    NVIC_SetPriority(TIM1_CC_IRQn, 5);

    // CAN bus
    NVIC_SetPriority(CAN1_TX_IRQn, 10);     // transmit
    NVIC_SetPriority(CAN1_RX0_IRQn, 10);    // receive
    NVIC_SetPriority(CAN1_RX1_IRQn, 10);    // receive
    NVIC_SetPriority(CAN1_SCE_IRQn, 10);    // on error interrupts

    // U(S)ART
    NVIC_SetPriority(USART1_IRQn, 11);
    NVIC_SetPriority(USART2_IRQn, 11);
}
