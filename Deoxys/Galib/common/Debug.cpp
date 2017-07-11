
#include <cstdarg>

#include "mbed.h"
#include "BufferedSerial.h"

#include "common/sys.h"
#include "pinout.h"

#include "common/Debug.h"


Debug *g_debug = NULL;


void debug_pre_init(Debug *debug)
{
#ifdef IAM_QBOUGE
    debug->printf("IAM_QBOUGE\n");
#endif
#ifdef IAM_QREFLECHI
    debug->printf("IAM_QREFLECHI\n");
#endif
#ifdef IAM_QENTRESORT
    debug->printf("IAM_QENTRESORT\n");
#endif

    sys_print_reset_source(debug);
}


Debug::Debug(void) {
    int i = 0;

    interfaces_[DEBUG_PC] = new BufferedSerial(USBTX, USBRX, INTERFACE_BUFFER_SIZE);
    interfaces_str_[DEBUG_PC] = "pc";
#ifdef IAM_QREFLECHI
    interfaces_[DEBUG_XBEE] = new BufferedSerial(XBEE_TX, XBEE_RX, INTERFACE_BUFFER_SIZE);
    interfaces_str_[DEBUG_XBEE] = "xbee";
#endif

    print_level_ = DEBUG_INITIALISATION;
    current_level_ = DEBUG_INITIALISATION;

    wait_ms(100);  // wait for the various UART buffers to clean up (xbee)

    for (i = 0; i < DEBUG_LAST; ++i)
        interfaces_[i]->baud(DEBUG_SPEED);

    this->printf("\n\n========================================\n\n");
    this->printf("Hello, world! (all)\n");

    for (i = 0; i < DEBUG_LAST; ++i)
        interfaces_[i]->printf("Hello, world! (%s)\n", interfaces_str_[i]);

    this->printf("\n");
}

void Debug::printf(const char* format, ...) {
    char buffer[BUFFER_SIZE] = "";
    va_list args;

    va_start(args, format);
    vsnprintf(buffer, BUFFER_SIZE, format, args);
    this->printf(current_level_, "%s", buffer);
    va_end(args);
}

void Debug::printf(Level level, const char* format, ...) {
    int i = 0;
    char buffer[BUFFER_SIZE] = "";
    va_list args;

    if (level < print_level_)
        return;

    va_start(args, format);
    vsnprintf(buffer, BUFFER_SIZE, format, args);
    va_end(args);

    for (i = 0; i < DEBUG_LAST; ++i)
        interfaces_[i]->printf("%s", buffer);

    if (level == DEBUG_INITIALISATION)
        wait_ms(5);
}

int _get_line(BufferedSerial *interface, char *buffer, int buffer_length) {
    int i = 0;
    Timer t;
    t.start();

#define READ_TIMEOUT 2000  // us

    while (1)
    {
        /*
            No more space in buffer.
            -1 because leave room for '\0'.
        */
        if (i == buffer_length-1)
            break;

        if (!interface->readable())
        {
            /*
                If we have already read something (i != 0), wait for a full line
                ('\n') until a timeout to prevent infinite loop.
                Dont getc() now, or it will block and we could be trapped.
                Try interface->readable() again.

                If we have not read anything or if we have waited too long,
                just quit.
            */
            if ((i == 0) || (t.read_us() > READ_TIMEOUT))
                break;
        }
        else
        {
            buffer[i] = interface->getc();
            if ((buffer[i] == '\r') || (buffer[i] == '\n'))
                break;
            i ++;
        }
    }

    buffer[i] = '\0';
    return i;
}

int Debug::get_line(char *buffer, int buffer_length, Interface interface) {
    int ret = 0, i = 0;

    for (i = 0; i < DEBUG_LAST; ++i)
    {
        if ((interface == i) || (interface == DEBUG_ALL))
        {
            ret = _get_line(interfaces_[i], buffer, buffer_length);
            if (ret != 0)
                return ret;
        }
    }

    return -1;  // nothing to read
}

void Debug::set_print_level(Level level) {
    print_level_ = level;
}

void Debug::set_current_level(Level level) {
    current_level_ = level;
}
