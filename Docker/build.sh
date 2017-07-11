#!/usr/bin/env bash

cp --archive /opt/Deoxys/* /tmp/build/

prepare_build() {
    cd "/tmp/build/$1"

    rm -rf libs mbed-os BUILD
    ln -s /opt/mbed-os .
    ln -s /opt/mbed-libs .
    ln -s /opt/Deoxys/Galib .

    mkdir -p "/tmp/build/output/BUILD_$1"
    ln -s "/tmp/build/output/BUILD_$1" BUILD
}

#post_build() {
#    cd "/tmp/build/$1"
#    cp -r "/tmp/build/output/BUILD_$1" BUILD
#}

prepare_build QBouge
prepare_build QReflechi
prepare_build QEntreQSort

make build

#post_build QBouge

# todo copy .hex to /tmp/buil/output

