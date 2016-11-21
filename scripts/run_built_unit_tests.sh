#!/bin/bash

. $(dirname ${BASH_SOURCE[0]})/common.sh

function run-built-unit-tests() {
    local tests=$(find-unit-tests-in-build-dir "$1")
    if [[ "${tests}" == "" ]]; then
        exit 1
    fi

    local tmp_root="${SCORE_ROOT}/tmp"
    local xml_target="${tmp_root}/gtest-xml"
    rm -rf ${xml_target} && mkdir -p ${xml_target}
    local failures=""
    for name in ${tests}; do
        echo "${name}"
        local base_name=$(basename ${name})
        ${name} --gtest_output=xml:${xml_target}/${base_name}.xml
        local rc="$?"
        if [[ ! $rc == "0" ]]; then
            failures="${name} ${failures}"
        fi
    done
    if [[ ! "${failures}" == "" ]]; then
        echo "FAILED: ${failures}"
    fi
    pushd ${SCORE_ROOT}
    ./scripts/gtest_reporter.py ${xml_target}
    popd
}

BUILD_DIR=$(get-build-dir-or-default "$1")
run-built-unit-tests ${BUILD_DIR}
