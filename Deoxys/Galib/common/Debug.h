#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#include "mbed.h"
#include "BufferedSerial.h"


#define DEBUG_SPEED             115200
#define INTERFACE_BUFFER_SIZE   256  // library default is 256
#define BUFFER_SIZE             INTERFACE_BUFFER_SIZE


class Debug {

public:
    typedef enum _Interface {
        DEBUG_PC,
        DEBUG_XBEE,
        DEBUG_LAST,
        DEBUG_ALL
    } Interface;

    typedef enum _Level {
        DEBUG_DEBUG,    // all (io + internals) -> gui          -> on
        DEBUG_INFO,     // basic info (inputs and outputs)      -> info
        DEBUG_ERROR     // minimum                              -> off
    } Level;

    Debug(void);
    void printf(const char* format, ...);
    void printf(Level level, const char* format, ...);
    int get_line(char *buffer, int buffer_length, Interface interface = DEBUG_ALL);
    void set_level(Level level);

protected:
    BufferedSerial interfaces_[DEBUG_LAST];
    const char *interfaces_str_[DEBUG_LAST];
    Level level_;
};

#endif
