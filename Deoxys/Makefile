

BOARD=F303K8
# BOARD=L476RG

MBED_DEFINES = -DMBED_HEAP_STATS_ENABLED=1 -DMBED_STACK_STATS_ENABLED=1

# INFINITE_LOOP_AFTER_TESTS
# DISABLE_TESTS
CUSTOM_DEFINES =

MBED_COMPILE_OPTIONS = -N out -m NUCLEO_$(BOARD) -t GCC_ARM -v $(MBED_DEFINES) $(CUSTOM_DEFINES)

.PHONY: all re build upload

all: build upload

build:
	mbed compile $(MBED_COMPILE_OPTIONS)
	echo " Yeah! \o/"

re:
	mbed compile -c $(MBED_COMPILE_OPTIONS)
	echo " Yeah! \o/"

upload:
	st-flash --reset write .build/NUCLEO_$(BOARD)/GCC_ARM/out.bin 0x8000000
	# cf. https://github.com/texane/stlink
