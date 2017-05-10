
# Install and Configure the environnement

*Note: cf. Eceborg's CleUsbSarahConnor*

* Compiler:
    * https://launchpad.net/gcc-arm-embedded
    * Some doc for windows: http://gnuarmeclipse.github.io/toolchain/install/
    * Check it is working by locating and executing `arm-none-eabi-gcc` and `arm-none-eabi-ld`
    * If you have some weird errors on Linux about "No such file or directory", run apt-get install libc6-i386
* Mbed-cli (used to compile a mbed-os-powered projet):
    * Easiest (automatically):
        * Install pip
        * Then, `pip install mbed-cli`
    * Otherwise (manually):
        * https://github.com/ARMmbed/mbed-cli
    * Check it is working by executing `mbed -h`
* Dependencies: in your project folder, at the root of your source files (or your Makefile):
    * Mbed-os: clone https://github.com/ARMmbed/mbed-os/ OR create a symlink to the already-cloned mbed-os folder. This is an hardware abstraction with some lib for your microcontroller.
    * Mbed libs:
        * MyBuffer        https://developer.mbed.org/users/sam_grove/code/Buffer/
        * BufferedSerial  https://developer.mbed.org/users/sam_grove/code/BufferedSerial/
        * PID             https://developer.mbed.org/users/aberk/code/PID/
    * Copy the file `mbed_settings.py` and edit it to set the compiler search path
    * Create an empty `.mbed` file (all required options will be passed through the command line when compiling)
    * Install mbed dependencies : pip install -r mbed-os/requirements.txt
    * Symlink Galib and all the mbed libs you downloaded, or mbed-cli won't find them when compiling.
    * Read Mbed-cli/README.md if you want to known everything about the tool we will use to compile
* debugger ?? -> openocd
* stlink ??
    git clone https://github.com/texane/stlink.git stlink
    mkdir build && cd build && cmake .. && make && make install
* More doc:
    * Quick start (blinky): https://docs.mbed.com/docs/mbed-os-handbook/en/5.1/getting_started/blinky_cli/
    * Mbed OS 3 User Guide: https://docs.mbed.com/docs/getting-started-mbed-os/en/latest/
    * Mbed homepage: https://developer.mbed.org/handbook/Homepage
