#!/bin/bash

pushd $(dirname ${BASH_SOURCE[0]})
SCRIPTS=$(pwd)
popd
ROOT=${SCRIPTS}/..

pushd ${ROOT}
mkdir -p tmp/kgrind
pushd tmp/kgrind
# rm cachegrind.out.*
valgrind --tool=callgrind ../../build/kgrind_threadpool
popd
popd
