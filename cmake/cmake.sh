#!/bin/bash

dir="$1"
build="$2"
shift 2

mkdir -p "$build"
pushd "$build" >> /dev/null
cmake "$dir" "$@"
popd >> /dev/null
