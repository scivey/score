#!/bin/bash

. $(dirname ${BASH_SOURCE[0]})/common.sh

function run-built-unit-tests() {
    local tests=$(find-unit-tests-in-build-dir "$1")
    if [[ "${tests}" == "" ]]; then
        exit 1
    fi
    for name in ${tests}; do
        echo "${name}"
        ${name}
    done
}

BUILD_DIR=$(get-build-dir-or-default "$1")
run-built-unit-tests ${BUILD_DIR}
