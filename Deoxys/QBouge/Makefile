

BOARD=F303K8
# BOARD=F401RE
# BOARD=L476RG

MBED_DEFINES = -DMBED_HEAP_STATS_ENABLED=1 -DMBED_STACK_STATS_ENABLED=1

# INFINITE_LOOP_AFTER_TESTS
# DISABLE_TESTS
CUSTOM_DEFINES = -DIAM_QBOUGE -DDISABLE_TESTS

MBED_COMPILE_OPTIONS = -N out -m NUCLEO_$(BOARD) -t GCC_ARM -v $(MBED_DEFINES) $(CUSTOM_DEFINES)

.PHONY: all re build upload

all: build upload

build:
	mbed compile $(MBED_COMPILE_OPTIONS)
	echo " Yeah! \o/"

soft_re:
	rm -rf .build/NUCLEO_$(BOARD)/GCC_ARM/libs/Galib
	rm -f .build/NUCLEO_$(BOARD)/GCC_ARM/main*
	mbed compile $(MBED_COMPILE_OPTIONS)
	echo " Yeah! \o/"

re:
	mbed compile -c $(MBED_COMPILE_OPTIONS)
	echo " Yeah! \o/"

upload:
	st-flash --reset write .build/NUCLEO_$(BOARD)/GCC_ARM/out.bin 0x8000000
	# cf. https://github.com/texane/stlink
	# cp .build/NUCLEO_$(BOARD)/GCC_ARM/out.bin /media/nodraak/NODE_$(BOARD)/

debug_openocd:
	# openocd -f board/st_nucleo_f401re.cfg -c "gdb_memory_map enable" -c "gdb_flash_program enable"
	openocd -f ../Openocd/st_nucleo_f303k8.cfg -c "gdb_memory_map disable"
# -c "gdb_flash_program enable"

debug_gdb:
	/home/nodraak/Documents/Etudes/ECE_ING4/Deoxys/stm32-mbed/gcc-arm-none-eabi-5_4-2016q2/bin/arm-none-eabi-gdb
