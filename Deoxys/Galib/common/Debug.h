#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

/*
    This class allows easy read and write, from and to an UART interface, whith
    several logging levels (not very used at the time of writting).
*/

#include "mbed.h"
#include "BufferedSerial.h"


#define DEBUG_SPEED             115200
#define INTERFACE_BUFFER_SIZE   256                     // underlying library buffer size (default is 256)
#define BUFFER_SIZE             INTERFACE_BUFFER_SIZE   // all string buffers in a Debug object


class Debug {

public:
    typedef enum _Interface {
        DEBUG_PC,
#ifdef IAM_QREFLECHI
        DEBUG_XBEE,
#endif
        DEBUG_LAST,
        DEBUG_ALL
    } Interface;

    typedef enum _Level {
        DEBUG_INITIALISATION, // this adds a 5 ms delay after the print to ensure it is transmitted (70 chars at 115k bauds)
        DEBUG_DEBUG,    // all (io + internals) -> gui          -> on
        DEBUG_INFO,     // basic info (inputs and outputs)      -> info
        DEBUG_ERROR     // minimum                              -> off
    } Level;

    Debug(void);

    /*
        Standard and low level version of printf. You can use both.
    */
    void printf(const char* format, ...);
    void printf(Level level, const char* format, ...);

    /*
        Read a line from an interface (by default, try all of them)
        Returns -1 if nothing is read, the length of the string read otherwise.
    */
    int get_line(char *buffer, int buffer_length, Interface interface = DEBUG_ALL);

    /*
        Set the level that should be printed, and the current level of the prints.
    */
    void set_print_level(Level level);
    void set_current_level(Level level);

protected:
    BufferedSerial interfaces_[DEBUG_LAST];
    const char *interfaces_str_[DEBUG_LAST];
    Level print_level_;
    Level current_level_;
};

void debug_pre_init(Debug *debug);

#endif
