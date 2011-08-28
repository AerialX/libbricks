#!/bin/bash

dir="`dirname $0`"

mkdir -p "$dir/build"
pushd "$dir/build/" >> /dev/null
cmake "$dir" $*
popd >> /dev/null
