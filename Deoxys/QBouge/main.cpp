
#include "mbed.h"

#include "common/Debug.h"
#include "common/Messenger.h"
#include "common/OrdersFIFO.h"
#include "common/com.h"
#include "common/main_sleep.h"
#include "common/mem_stats.h"
#include "common/utils.h"
#include "QBouge/MotionController.h"

#include "common/test.h"
#include "QBouge/test_mc.h"

#include "config.h"
#include "pinout.h"


MotionController *mc = NULL;
bool request_next_order = false;
void asserv_main(void);

void pre_init(Debug *debug)
{
#ifdef IAM_QBOUGE
    debug->printf("IAM_QBOUGE\n");
#endif
#ifdef IAM_QREFLECHI
    debug->printf("IAM_QREFLECHI\n");
#endif

    debug->printf("Reset source:\n");

    if (__HAL_RCC_GET_FLAG(RCC_FLAG_OBLRST))
        debug->printf("\tRCC_FLAG_OBLRST    Option Byte Load reset\n");
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST))
        debug->printf("\tRCC_FLAG_PINRST    Pin reset.\n");
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST))
        debug->printf("\tRCC_FLAG_PORRST    POR/PDR reset.\n");
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST))
        debug->printf("\tRCC_FLAG_SFTRST    Software reset.\n");
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST))
        debug->printf("\tRCC_FLAG_IWDGRST   Independent Watchdog reset.\n");
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST))
        debug->printf("\tRCC_FLAG_WWDGRST   Window Watchdog reset.\n");
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST))
        debug->printf("\tRCC_FLAG_LPWRRST   Low Power reset.\n");
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_V18PWRRST))
        debug->printf("\tRCC_FLAG_V18PWRRST Reset flag of the 1.8 V domain\n");  // specifiic to F303x8
    debug->printf("\t-\n");

    debug->printf("\n");
}


void interrupt_priorities_init(void)
{
    NVIC_SetPriorityGrouping(0);

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
}


int main(void)
{
    Debug *debug = NULL;
    CanMessenger *messenger = NULL;
    Timer *loop = NULL;
    Ticker *asserv_ticker = NULL;

    PwmOut ml(MOTOR_L_PWM);
    ml.period(0.001 * 0.05);
    ml.write(0);
    PwmOut mr(MOTOR_R_PWM);
    mr.period(0.001 * 0.05);
    mr.write(0);

    PwmOut buzzer_(A7);

    buzzer_.period(1./2000);
    buzzer_.write(0.50);
    wait_ms(200);
    buzzer_.period(1./3000);
    buzzer_.write(0.50);
    wait_ms(200);
    buzzer_.period(1./4000);
    buzzer_.write(0.50);
    wait_ms(400);
    buzzer_.period_us(1);

    wait_ms(100);  // wait for the UART to clean up

    /*
        Initializing
    */

    debug = new Debug;
    pre_init(debug);

    debug->printf("Initializing\n");

    mem_stats_dynamic(debug);
    wait_ms(100);
    mem_stats_objects(debug);
    wait_ms(100);
    mem_stats_settings(debug);
    wait_ms(100);
    test_run_all(debug);
    wait_ms(100);

    debug->printf("CanMessenger...\n");
    wait_ms(100);
    messenger = new CanMessenger;
    debug->printf("Timer...\n");
    wait_ms(100);
    loop = new Timer;
    loop->start();

    debug->printf("MotionController...\n");
    wait_ms(100);
    mc = new MotionController;

    debug->printf("Ticker...\n");
    wait_ms(100);
    asserv_ticker = new Ticker;
    asserv_ticker->attach(mc, &MotionController::asserv, ASSERV_DELAY);

    debug->printf("interrupt_priorities...\n");
    interrupt_priorities_init();

    mem_stats_dynamic(debug);
    wait_ms(100);

    debug->printf("Initialisation done.\n\n");

    // todo wait for tirette (can msg)

    /*
        Go!
    */

    Timer match;
    match.start();

    while (true)
    {
        debug->printf("[timer/match] %.3f\n", match.read());

        loop->reset();

        com_handle_serial(debug, messenger, mc);
        com_handle_can(debug, messenger, mc);

        if (mc->should_request_next_order(debug))
        {
            debug->printf("[CAN] send next_order_request\n");
            messenger->send_msg_CQB_next_order_request(ORDERS_COUNT - mc->orders_->size());
        }

        mc->debug(debug);

        main_sleep(debug, loop);
    }

    /*
        Cleanup
    */

    debug->printf("Cleaning...\n");
    Thread::wait(100);

    asserv_ticker->detach();
    delete asserv_ticker;
    delete loop;
    delete messenger;
    delete mc;
    delete debug;

    return 0;
}
