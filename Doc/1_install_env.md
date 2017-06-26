
# Install and Configure the environnement

* Compiler:
    * https://launchpad.net/gcc-arm-embedded
    * Some doc for windows: http://gnuarmeclipse.github.io/toolchain/install/
    * Check it is working by locating and executing `arm-none-eabi-gcc`
    * If you have some weird errors on Linux about "No such file or directory", run `apt-get install libc6-i386`
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
        * *Note: this is outdated. BufferedSerial is now included in Mbed - no tested yet though- and PID has been forked into Deoxys code.*
        * ~~MyBuffer        https://developer.mbed.org/users/sam_grove/code/Buffer/~~
        * ~~BufferedSerial  https://developer.mbed.org/users/sam_grove/code/BufferedSerial/~~
        * ~~PID             https://developer.mbed.org/users/aberk/code/PID/~~
    * Add the compiler arm-none-eabi-gcc to your `$PATH` OR set its path in `mbed_settings.py`
    * ~~Create an empty `.mbed` file (all required options will be passed through the command line when compiling)~~ Not really needed, this is just to make mbed-cli dont bother us.
    * Install mbed dependencies : `pip install -r mbed-os/requirements.txt`
    * Symlink `Galib/` and all the mbed libs you downloaded, or mbed-cli won't find them when compiling.
        * In QBouge/: `ln -s ../Galib .` and `ln -s path/to/mbed-os .`
    * Read Mbed-cli/README.md if you want to known everything about the tool we will use to compile
* Debugger: check out openocd. Not all microcontrollers are supported though.
* stlink: I don't remember is this one is usefull. I believe it just make our lives easier, and storage and bandwidth are quite cheap nowdays.
    * `git clone https://github.com/texane/stlink.git stlink`
    * `mkdir build && cd build && cmake .. && make && make install`
* More doc:
    * Quick start (blinky): https://docs.mbed.com/docs/mbed-os-handbook/en/latest/getting_started/blinky_cli/
    * Mbed OS ~3~ 5 User Guide: https://docs.mbed.com/docs/getting-started-mbed-os/en/latest/ (mbed-os 5 = mbed-os 2 + mbed-os 3)
    * Mbed homepage: https://developer.mbed.org/handbook/Homepage

