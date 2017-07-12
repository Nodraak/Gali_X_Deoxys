#!/usr/bin/env bash

set -e

# Arg1: folder QBouge QReflechi or QEntreQSort
# Arg2: board L432KC or L476RG
build() {
    cd "/tmp/build/$1"

    # clean existing simlinks
    rm -rf libs mbed-os BUILD
    # create proper working simlinks
    ln -s /opt/mbed-os .
    ln -s /opt/mbed-libs .
    ln -s /opt/Deoxys/Galib .

    # link build folder to host, to gather the binary output and cache the
    # temporary build objects (.o files)
    mkdir -p "/tmp/build/output/BUILD_$1"
    ln -s "/tmp/build/output/BUILD_$1" BUILD

    make build

    cp "BUILD/NUCLEO_$2/GCC_ARM/out.bin" "/tmp/build/output/$1.hex"
}

main() {
    cp --archive /opt/Deoxys/* /tmp/build/

    build QBouge L432KC
    build QReflechi L432KC
    build QEntreQSort L476RG
}

main
