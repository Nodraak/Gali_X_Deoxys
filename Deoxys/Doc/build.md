
# Build and Flash the Microcontroller

*Note: check that you have properly follwed all steps in `install_env.md`.*

The basic way to compile is to used mbed-cli: `mbed compile -t GCC_ARM -m NUCLEO_XXX`. Some options are required, others are optional.

Several options can be passed either through the file `.mbed` or `profile.json` or through the command line (using `.mbed` seems to be deprecated. The safest might be to go full command line with a Makefile).

The required options are:
* Toolchain: `-t GCC_ARM`. Because we have downloaded gcc-arm-embedded.
* Microcontroller: `-m NUCLEO_XXX`. This can be NUCLEO_L432KC, NUCLEO_F303K8, NUCLEO_F401RE, ... Or even a non Nucleo board. This must match the microcontroller you will flash, otherwise it won't work.

You are encouraged to use the Makefile:
* `make`: execute `make build` and then `make upload`
* `make build`: compile the sources
* `make upload`: flash the connected microcontroller with the executable

When compiling, the output file is in `./.build/NUCLEO_XXX/GCC_ARM/out.bin`. In orde to flash the microcontroller, just copy and paste this file. You might use the st-link utility, which save a lot of trouble (if no space is left on teh microcontroller and you were using the copy method, unplug an replug the board. Yes, this is a very silly bug).
