
MBED_COMPILE_OPTIONS_MINI = -N out -m NUCLEO_F303K8 -t GCC_ARM -D BOARD_MINI
MBED_COMPILE_OPTIONS_MEDIUM = -N out -m NUCLEO_F401RE -t GCC_ARM -D BOARD_MEDIUM

all: build_mini build_medium upload_mini upload_medium

build_mini:
	echo "build_mini"
	mbed compile $(MBED_COMPILE_OPTIONS_MINI)

build_medium:
	echo "build_medium"
	mbed compile $(MBED_COMPILE_OPTIONS_MEDIUM)

upload_mini:
	echo "upload_mini"
	cp .build/NUCLEO_F303K8/GCC_ARM/out.bin /media/nodraak/NODE_F303K8/

upload_medium:
	echo "upload_medium"
	cp .build/NUCLEO_F401RE/GCC_ARM/out.bin /media/nodraak/NODE_F401RE/
