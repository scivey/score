#!/bin/bash

ROOT=$(git rev-parse --show-toplevel)

pushd ${ROOT}/external/googletest

if [[ ! -d "build" ]]; then
    mkdir build
    pushd build
    cmake ../ && make -j8
    popd
fi

popd
