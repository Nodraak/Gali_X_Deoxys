
#include "common/Debug.h"

#include "common/sys.h"


void sys_print_reset_source(Debug *debug)
{
    debug->printf("Reset source:\n");

#ifdef TARGET_NUCLEO_F303K8
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_OBLRST))
        debug->printf("\tRCC_FLAG_OBLRST    Option Byte Load reset\n");
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST))
        debug->printf("\tRCC_FLAG_PINRST    Pin reset\n");
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST))
        debug->printf("\tRCC_FLAG_PORRST    POR/PDR reset\n");
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST))
        debug->printf("\tRCC_FLAG_SFTRST    Software reset\n");
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST))
        debug->printf("\tRCC_FLAG_IWDGRST   Independent Watchdog reset\n");
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST))
        debug->printf("\tRCC_FLAG_WWDGRST   Window Watchdog reset\n");
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST))
        debug->printf("\tRCC_FLAG_LPWRRST   Low Power reset\n");
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_V18PWRRST))
        debug->printf("\tRCC_FLAG_V18PWRRST Reset flag of the 1.8 V domain\n");  // specifiic to F303x8
    debug->printf("\t-\n");
#endif

#ifdef TARGET_NUCLEO_L432KC
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
#endif

    debug->printf("\n");
}


void sys_interrupt_priorities_init(Debug *debug)
{
    NVIC_SetPriorityGrouping(0);

#ifdef TARGET_NUCLEO_F303K8
    NVIC_SetPriority(EXTI0_IRQn, 1);
    NVIC_SetPriority(EXTI1_IRQn, 1);
    NVIC_SetPriority(EXTI2_TSC_IRQn, 2);
    NVIC_SetPriority(EXTI3_IRQn, 2);

    NVIC_SetPriority(TIM2_IRQn, 5); // asserv

    NVIC_SetPriority(CAN_TX_IRQn, 10);
    NVIC_SetPriority(CAN_RX0_IRQn, 10);
    NVIC_SetPriority(CAN_RX1_IRQn, 10);
    NVIC_SetPriority(CAN_SCE_IRQn, 10);

    NVIC_SetPriority(USART1_IRQn, 11);
    NVIC_SetPriority(USART2_IRQn, 11);
    NVIC_SetPriority(USART3_IRQn, 11);
#endif

#ifdef TARGET_NUCLEO_L432KC
    NVIC_SetPriority(EXTI0_IRQn, 1);
    NVIC_SetPriority(EXTI1_IRQn, 1);
    NVIC_SetPriority(EXTI2_IRQn, 2);
    NVIC_SetPriority(EXTI3_IRQn, 2);

    NVIC_SetPriority(TIM1_BRK_TIM15_IRQn, 3); // pwm
    NVIC_SetPriority(TIM1_UP_TIM16_IRQn, 3); // pwm
    NVIC_SetPriority(TIM1_TRG_COM_IRQn, 3); // pwm
    NVIC_SetPriority(TIM1_CC_IRQn, 3); // pwm

    NVIC_SetPriority(TIM2_IRQn, 5); // asserv

    NVIC_SetPriority(CAN1_TX_IRQn, 10);
    NVIC_SetPriority(CAN1_RX0_IRQn, 10);
    NVIC_SetPriority(CAN1_RX1_IRQn, 10);
    NVIC_SetPriority(CAN1_SCE_IRQn, 10);

    NVIC_SetPriority(USART1_IRQn, 11);
    NVIC_SetPriority(USART2_IRQn, 11);
#endif
}
