#!/bin/bash

pushd () {
    command pushd "$@" > /dev/null
}

popd () {
    command popd "$@" > /dev/null
}

function add-env-vars() {
    pushd $(dirname ${BASH_SOURCE[0]})
    export SCRIPTS_DIR=$(pwd)
    popd
    pushd ${SCRIPTS_DIR}/..
    export SCORE_ROOT=$(pwd)
    popd
}

add-env-vars

function check-build-dir() {
    local build_dir="$1"
    if [[ "${build_dir}" = "" ]]; then
        echo "specify build directory" >&2
        exit 1
    elif [[ ! -d ${build_dir} ]]; then
        echo "build directory \"${build_dir}\" doesn't exist or isn't a directory" >&2
        exit 1
    fi
    echo "${build_dir}"
}

function find-lib-dirs-in-build-dir() {
    local build_dir="$1"
    if [[ ! $(check-build-dir ${build_dir}) ]]; then
        exit 1
    fi
    pushd ${build_dir}
    for name in $(ls); do
        if [[ "${name}" == "score_"* && -d ${name} ]]; then
            echo "${build_dir}/${name}"
        fi
    done
    popd
}


function find-unit-tests-in-build-dir() {
    local build_dir="$1"
    local lib_dirs=$(find-lib-dirs-in-build-dir ${build_dir})
    if [[ "${lib_dirs}" == "" ]]; then
        exit 1
    fi
    for lib_dir in ${lib_dirs}; do
        for fname in $(ls ${lib_dir}); do
            if [[ "${fname}" == *"_unit_tests" ]]; then
                echo "${lib_dir}/${fname}"
            fi
        done
    done
}

function find-integration-tests-in-build-dir() {
    local build_dir="$1"
    local lib_dirs=$(find-lib-dirs-in-build-dir ${build_dir})
    if [[ "${lib_dirs}" == "" ]]; then
        exit 1
    fi
    for lib_dir in ${lib_dirs}; do
        for fname in $(ls ${lib_dir}); do
            if [[ "${fname}" == *"_integration_tests" ]]; then
                echo "${lib_dir}/${fname}"
            fi
        done
    done
}

DEFAULT_BUILD_DIR="${SCORE_ROOT}/build"

function get-build-dir-or-default() {
    local build_dir="$1"
    if [[ "${build_dir}" == "" ]]; then
        echo ${DEFAULT_BUILD_DIR}
    else
        echo "${build_dir}"
    fi
}
