#!/bin/bash

ROOT=$(git rev-parse --show-toplevel)

EXT=${ROOT}/external


function cmake-build() {
    if [[ ! -d "build" ]]; then
        mkdir build
        pushd build
        cmake ../ && make -j8
        popd
    fi
}

function copy-re2-headers() {
    pushd ${EXT}/re2/build
    if [[ ! -d include ]]; then
        mkdir -p include/re2
        cp ../re2/*.h include/re2
    fi
    popd
}

LIBS="googletest boringssl re2"

for lib in ${LIBS}; do
    pushd ${EXT}/${lib}
    cmake-build
    popd
done

copy-re2-headers
