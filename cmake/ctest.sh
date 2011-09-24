#!/bin/bash

build="$1"
shift

pushd "$build/test" >> /dev/null
GTEST_COLOR=1 ctest -V $*
popd >> /dev/null
