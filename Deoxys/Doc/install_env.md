
# Install and Configure the environnement

*Note: cf. Eceborg's CleUsbSarahConnor*

* Compiler:
    * https://launchpad.net/gcc-arm-embedded
    * some doc for windows: http://gnuarmeclipse.github.io/toolchain/install/
    * check it is working by locating and executing `arm-none-eabi-gcc` and `arm-none-eabi-ld`
* Mbed-cli (used to compile a mbed-os-powered projet):
    * easiest (automatically):
        * install pip
        * Then, `pip install mbed-cli`
    * otherwise (manually):
        * https://github.com/ARMmbed/mbed-cli
    * check it is working by executing `mbed -h`
* Dependencies: in your project folder, near your source files:
    * Mbed-os: clone https://github.com/ARMmbed/mbed-os/ OR create a symlink to the already-cloned mbed-os folder. This is an hardware abstraction with some lib for your microcontroller.
    * Libs:
        * MyBuffer        https://developer.mbed.org/users/sam_grove/code/Buffer/
        * BufferedSerial  https://developer.mbed.org/users/sam_grove/code/BufferedSerial/
        * PID             https://developer.mbed.org/users/aberk/code/PID/
    * copy the file `mbed_settings.py` and edit it to set the compiler search path
    * create an empty `.mbed` file (all required options will be passed through the command line when compiling)
    * read Mbed-cli/README.md if you want to known everything about the tool we will use to compile
* More doc:
    * Quick start (blinky): https://docs.mbed.com/docs/mbed-os-handbook/en/5.1/getting_started/blinky_cli/
    * mbed OS 3 User Guide: https://docs.mbed.com/docs/getting-started-mbed-os/en/latest/
    * mbed homepage: https://developer.mbed.org/handbook/Homepage

