
#include "mbed.h"

/*
    This is some Cortex F3 low level magic.

    If you don't known what you are looking for, just close this file and look
    somewhere else.
    If you known that you are looking for this, keep reading.

---

    When a fault occur (division by zero, segfault, ...), the processor calls an
    interrupt: HardFault, NMI, MemManage, MemMang, BusFault, UsageFault, etc.

    When one of theses fault occur, fault_handler() is called (see all the
    *Handler() below) and prints as much informaation as it can to help you
    locate the problem.
*/

void fault_handler(uint32_t *top_of_stack, const char *msg)
{
    uint32_t r0 = top_of_stack[0];
    uint32_t r1 = top_of_stack[1];
    uint32_t r2 = top_of_stack[2];
    uint32_t r3 = top_of_stack[3];
    uint32_t r12 = top_of_stack[4];
    uint32_t lr = top_of_stack[5];  // Link register
    uint32_t pc = top_of_stack[6];  // Program counter
    uint32_t xpsr = top_of_stack[7];  // Program status register

#define SCB_BASE_ADDR   0xE000ED00

#define CFSR_ADDR       (SCB_BASE_ADDR+0x28)
#define HFSR_ADDR       (SCB_BASE_ADDR+0x2C)
#define MMFAR_ADDR      (SCB_BASE_ADDR+0x34)
#define BFAR_ADDR       (SCB_BASE_ADDR+0x38)

    uint32_t CFSR = *(uint32_t*)CFSR_ADDR;
    uint16_t UFSR = CFSR >> 16;
    uint8_t BFSR = ((CFSR >> 8) & 0xFF);
    uint8_t MMFSR = (CFSR & 0xFF);

    uint32_t HFSR = *(uint32_t*)HFSR_ADDR;
    uint32_t MMFAR = *(uint32_t*)MMFAR_ADDR;
    uint32_t BFAR = *(uint32_t*)BFAR_ADDR;

    bool DIVBYZERO = UFSR & (0x1 << 9);

    bool BFARVALID = BFSR & (0x1 << 7);  // BFAR holds a valid fault address
    bool PRECISERR = BFSR & (0x1 << 1);  /* A data bus error has occurred, and the PC value stacked for the exception
                                            return points to the instruction that caused the fault. */
    bool forced_hard_fault = HFSR & (0x1 << 30);

    char buf[512];

    sprintf(buf,
        "\n\n********************\n\n=> %s\n\n"
// cpu registers
        "r0   %04x %04x\n"
        "r1   %04x %04x\n"
        "r2   %04x %04x\n"
        "r3   %04x %04x\n"
        "r12  %04x %04x\n"
        "lr   %04x %04x\n"
        "pc   %04x %04x\n"
        "xpsr %04x %04x\n\n"
// memory specialy mapped registers
        "(CFSR  %04x %04x)\n"
        "UFSR %04x\n"
        "BFSR %02x\n"
        "MMFSR %02x\n"
        "HFSR  %04x %04x\n"
        "MMFAR %04x %04x\n"
        "BFAR  %04x %04x\n\n"
// flags
        "DIVBYZERO %d\n"
        "BFARVALID %d\n"            // BFAR holds the fault address
        "PRECISERR %d\n"            // The PC value stacked for the exception return points to the instruction that caused the fault
        "forced_hard_fault %d\n"    // The fault escalated to a hard fault

        "\n***\n",

        msg,

        (uint16_t)(r0 >> 16),   (uint16_t)(r0 & 0xFFFF),
        (uint16_t)(r1 >> 16),   (uint16_t)(r1 & 0xFFFF),
        (uint16_t)(r2 >> 16),   (uint16_t)(r2 & 0xFFFF),
        (uint16_t)(r3 >> 16),   (uint16_t)(r3 & 0xFFFF),
        (uint16_t)(r12 >> 16),  (uint16_t)(r12 & 0xFFFF),
        (uint16_t)(lr >> 16),   (uint16_t)(lr & 0xFFFF),
        (uint16_t)(pc >> 16),   (uint16_t)(pc & 0xFFFF),
        (uint16_t)(xpsr >> 16), (uint16_t)(xpsr & 0xFFFF),

        (uint16_t)(CFSR >> 16), (uint16_t)(CFSR & 0xFFFF),
        UFSR, BFSR, MMFSR,
        (uint16_t)(HFSR >> 16), (uint16_t)(HFSR & 0xFFFF),
        (uint16_t)(MMFAR >> 16), (uint16_t)(MMFAR & 0xFFFF),
        (uint16_t)(BFAR >> 16), (uint16_t)(BFAR & 0xFFFF),

        DIVBYZERO,
        BFARVALID,
        PRECISERR,
        forced_hard_fault
    );

    printf(buf);
    error(buf);

    while (1)
        ;
}

extern "C" void HardFault_Handler(uint32_t *top_of_stack) {
    fault_handler(top_of_stack, "Hard Fault");
}
extern "C" void NMI_Handler(uint32_t *top_of_stack) {
    fault_handler(top_of_stack, "NMI Fault");
}
extern "C" void MemManage_Handler(uint32_t *top_of_stack) {
    fault_handler(top_of_stack, "MemManage Fault");
}
extern "C" void MemMang_Handler(uint32_t *top_of_stack) {
    fault_handler(top_of_stack, "MemMang Fault");
}
extern "C" void BusFault_Handler(uint32_t *top_of_stack) {
    fault_handler(top_of_stack, "BusFault Fault");
}
extern "C" void UsageFault_Handler(uint32_t *top_of_stack) {
    fault_handler(top_of_stack, "UsageFault Fault");
}
