
#include "mbed.h"

#include "common/Debug.h"
#include "common/Messenger.h"
#include "common/com.h"
#include "common/main_sleep.h"
#include "common/mem_stats.h"
#include "common/utils.h"
#include "QEntreQSort/RoboticArm.h"

#include "common/test.h"

#include "config.h"
#include "pinout.h"

Debug *g_debug = NULL;

int main(void)
{
    Debug *debug = NULL;
    CanMessenger *messenger = NULL;
    Timer *loop = NULL;

    PwmOut buzzer_(BUZZER_PIN);

    buzzer_.period(1./2000);
    buzzer_.write(0.50);
    wait_ms(200);
    buzzer_.period(1./2000);
    buzzer_.write(0.50);
    wait_ms(200);
    buzzer_.period(1./2000);
    buzzer_.write(0.50);
    wait_ms(400);

    /*
        Initializing
    */

    debug = new Debug;
g_debug = debug;
    debug_pre_init(debug);

    debug->printf("Initializing\n");

    debug->printf("CanMessenger...\n");
    messenger = new CanMessenger;

    mem_stats_objects(debug);
    mem_stats_settings(debug);
    test_run_all(debug);

    debug->printf("Timer...\n");
    loop = new Timer;
    loop->start();

    debug->printf("AX12_arm...\n");
    AX12_arm ax12_arm(1, 5, 9, AX12_PIN_SERVO, AX12_PIN_VALVE);

    mem_stats_dynamic(debug);

    debug->printf("Initialisation done.\n\n");
    debug->set_current_level(Debug::DEBUG_DEBUG);

    // todo
    // while (true)
    {
        // if t > 1 sec
            // send can ping

        // if receive pong
            // break
    }

    // todo wait for tirette (can msg)

    // while (1)
        // ;

    /*
        Go!
    */

    int pos[5] = {630, 50, 330};

    Timer t;
    t.start();

    while (true)
    {
        char buffer[BUFFER_SIZE], *ptr = NULL;
        if (debug->get_line(buffer, BUFFER_SIZE) != -1)
        {
            if (buffer[0] == 'g')
            {
                debug->printf("*** go !!! ***\n");
                ax12_arm.do_sequence();
            }
            else if (buffer[0] == 's')
            {
                char *ptr = buffer+2;
                int a;

                a = atoi(ptr);

                debug->printf("set speed %d\n", a);
                ax12_arm.write_speed_all(a);
            }
            else if (buffer[0] == 'p')
            {
                char *ptr = buffer;
                int a, b;

                a = atoi(ptr);
                while (*ptr != ' ')
                    ++ptr;
                ++ptr;
                b = atoi(ptr);

                pos[a] = b;
                printf("have pos %d %d %d\n", pos[0], pos[1], pos[2]);

                ax12_arm.write_pos_all(pos[0], pos[1], pos[2]);
            }
            else
                debug->printf("unknown cmd\n");
        }

        // ax12_arm.read_pos_all();
        wait_ms(100);
    }

ax12_arm.write_pos_all(660, 613, 330);
wait_ms(1000);
ax12_arm.write_pos_all(700-100, 920-50, 317);
wait_ms(1000);

    Timer match;
    match.start();

    while (true)
    {
        loop->reset();
        debug->printf("[timer/match] %.3f\n", match.read());
        // ax12_arm.read_pos_all();

        com_handle_can(debug, messenger, &ax12_arm);

        main_sleep(debug, loop);
    }

    /*
        Cleanup
    */

    debug->set_current_level(Debug::DEBUG_INITIALISATION);
    debug->printf("Cleaning...\n");

    delete messenger;
    delete debug;

    return 0;
}
