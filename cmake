#!/bin/bash

dir="`dirname $0`"

mkdir -p "$dir/build"
pushd "$dir/build/"
cmake "$dir" $*
popd
