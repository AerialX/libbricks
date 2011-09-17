#!/bin/bash

dir="`dirname $0`/.."
build="$1"
shift

mkdir -p "$dir/$build"
pushd "$dir/$build" >> /dev/null
cmake ../ "$@"
popd >> /dev/null
