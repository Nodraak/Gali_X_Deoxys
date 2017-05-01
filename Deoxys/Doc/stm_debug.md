
# Debugging STM32 boards

There are two ways of debug (in addition to the good old printf):

* Fault handler: execute some code when a fault is detected: cf common/fault_handler.cpp
* Gdb: be able to read registers value and single step: read ahead this file

The idea is to use the classic Gdb, and connecting to the microcontroller with Openocd.
Openocd receive commands from gdb and is the interface to the microcontroller.

## Prerequisites

Make sure you have:
    * arm-none-eabi-gdb (alongside your compiler arm-none-eabi-gcc)
    * openocd

A few notes:

* Openocd's doc is at <http://openocd.org/documentation/> (go read it, it's worth it)
* Openocd's config files are in `/usr/share/openocd/scripts` (explore this folder, it's worth it too)
* Gdb's doc can be accessed by typing "help" when it is running.

## Starting everything

Start Openocd:

```
$ openocd -f board/st_nucleo_f401re.cfg -c "gdb_memory_map enable" -c "gdb_flash_program enable"
```

Start Gdb (without closing Openocd):

```
$ arm-none-eabi-gdb
```

In Gdb, type:

```
target remote localhost:3333  # connect to Openocd
monitor reset init  # reset the microcontroller
file path/to/the/elf/file/for/example/.build/NUCLEO_XXX/GCC_ARM/out.elf  # use this file
```

You can then use Gdb as usual:
* To start type `continue` (or `c`)
* When not running, you can configure Gdb:
    * To set a breakpoint, type `break some_function_or_symbol_or_address`

Note that you can still read the serial (over USB or Xbee or whatever) output of the microcontroller.

Don't forget `Openocd/*` and `.gdbinit` configuration files, as they can change the behaviour of theses tools.
