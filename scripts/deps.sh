#!/bin/bash

ROOT=$(git rev-parse --show-toplevel)

EXT=${ROOT}/external

function using-clang() {
    if [[ "${CXX}" == "clang"* ]]; then
        echo "yes"
    else
        echo "no"
    fi
}

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

LIBS="googletest boringssl"

function build-deps() {
    for lib in ${LIBS}; do
        pushd ${EXT}/${lib}
        cmake-build
        popd
    done    
}

function clean-deps() {
    for lib in ${LIBS}; do
        pushd ${EXT}/${lib}
        rm -rf build
        popd
    done    
}

function show-usage() {
    echo "Usage: ${0} build|clean|debug" >&2
}

function setup-env() {
    local have_clang=$(using-clang)
    if [[ "${have_clang}" == "yes" ]]; then
        export CXXFLAGS="-Wno-deprecated-declarations ${CXXFLAGS}"
    fi
}

function show-debug() {
    local using=$(using-clang)
    echo "result? ${using}"
    echo "CXX ${CXX}" >&2
}

case "$1" in
    build)
        setup-env
        build-deps
        ;;
    clean)
        setup-env
        clean-deps
        ;;
    debug)
        setup-env
        show-debug
        ;;
    *)
        show-usage
        exit 1
        ;;
esac
