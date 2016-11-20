#!/bin/bash

. $(dirname ${BASH_SOURCE[0]})/common.sh

function run-built-unit-tests() {
    local tests=$(find-unit-tests-in-build-dir "$1")
    if [[ "${tests}" == "" ]]; then
        exit 1
    fi
    local failures=""
    for name in ${tests}; do
        echo "${name}"
        ${name}
        local rc="$?"
        if [[ ! $rc == "0" ]]; then
            failures="${name} ${failures}"
        fi
    done
    if [[ ! "${failures}" == "" ]]; then
        echo "FAILED: ${failures}"
    fi
}

BUILD_DIR=$(get-build-dir-or-default "$1")
run-built-unit-tests ${BUILD_DIR}
