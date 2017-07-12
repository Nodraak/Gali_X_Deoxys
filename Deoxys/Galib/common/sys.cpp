
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

// weird, this version does not work (but it should):
// NVIC_SetPriority(IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), PreemptPriority, 0))
#define _SetPriority(IRQn, PreemptPriority) NVIC_SetPriority(IRQn, PreemptPriority)

#if defined(IAM_QBOUGE) || defined(IAM_QENTRESORT)
/*
    DigitalIn interrupt
       CQB: encoders input
       CQES: SRF echo
*/
    _SetPriority(EXTI0_IRQn, 1);
    _SetPriority(EXTI1_IRQn, 1);
    _SetPriority(EXTI2_IRQn, 1);
    _SetPriority(EXTI3_IRQn, 1);
#endif

    // pwm outputs (mbed PwmOut via TIM1)
    _SetPriority(TIM1_BRK_TIM15_IRQn, 4);
    _SetPriority(TIM1_UP_TIM16_IRQn, 4);
    _SetPriority(TIM1_TRG_COM_IRQn, 4);
    _SetPriority(TIM1_CC_IRQn, 4);

#ifdef IAM_QBOUGE
    // asserv (mbed ticker (ISR) via TIM2)
    _SetPriority(TIM2_IRQn, 5);
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


void sys_debug_can(void)
{
#undef BASE_ADDR_CAN1
#undef CAN_MCR
#undef CAN_MCR_ABOM
#undef CAN_ESR
#undef CAN_ESR_REC
#undef CAN_ESR_TEC
#undef CAN_ESR_LEC
#undef CAN_ESR_BOFF

#define BASE_ADDR_CAN1 0x40006400

#define CAN_MCR         (BASE_ADDR_CAN1+0x00)
#define CAN_MCR_ABOM    (((*(uint32_t*)CAN_MCR) >> 6) & 0b1)    // Bit 6 ABOM: Automatic bus-off management

#define CAN_ESR        (BASE_ADDR_CAN1+0x18)
#define CAN_ESR_REC    (((*(uint32_t*)CAN_ESR) >> 24) & 0b11111111) // Bits 31:24 REC[7:0] : Receive error counter
#define CAN_ESR_TEC    (((*(uint32_t*)CAN_ESR) >> 16) & 0b11111111) // Bits 23:16 TEC[7:0] : Least significant byte of the 9-bit transmit error counter
#define CAN_ESR_LEC    (((*(uint32_t*)CAN_ESR) >> 4) & 0b111)       // Bits 6:4 LEC[2:0] : Last error code
#define CAN_ESR_BOFF   (((*(uint32_t*)CAN_ESR) >> 2) & 0b1)         // Bit 2 BOFF : Bus-off flag

    g_debug->printf("[CAN/Error] abom %d - rec tec %d %d - lec boff %d %d\n", CAN_MCR_ABOM, CAN_ESR_REC, CAN_ESR_TEC, CAN_ESR_LEC, CAN_ESR_BOFF);

/*
LEC
    000: No Error
    001: Stuff Error
    010: Form Error
    011: Acknowledgment Error
    100: Bit recessive Error
    101: Bit dominant Error
    110: CRC Error
    111: Set by software
*/

    // if (CAN_ESR_BOFF)
    // {
    //     g_debug->printf("Bus off, reset\n");
    //     *(uint32_t*)CAN_MCR |= 0x1 << 15;  // Bit 15 RESET: bxCAN software master reset
    // }
}


void ticker_lock(void) {
    NVIC_DisableIRQ(TIM2_IRQn);
}

void ticker_unlock(void) {
    NVIC_EnableIRQ(TIM2_IRQn);
}
