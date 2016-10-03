
MBED_COMPILE_OPTIONS = -N out

.PHONY: all re build upload

all: build upload

build:
	mbed compile $(MBED_COMPILE_OPTIONS)
	echo " Yeah! \o/"

re:
	mbed compile -c $(MBED_COMPILE_OPTIONS)
	echo " Yeah! \o/"

upload:
	cp .build/NUCLEO_L476RG/GCC_ARM/out.bin /media/nodraak/NODE_L476RG/

