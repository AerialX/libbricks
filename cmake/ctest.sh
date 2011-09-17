#!/bin/bash

dir="`dirname $0`/.."
build=$1
shift

pushd "$dir/$build/test" >> /dev/null
GTEST_COLOR=1 ctest $* -V
popd >> /dev/null