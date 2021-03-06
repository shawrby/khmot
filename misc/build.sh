#!/bin/bash
set -e

[ ! -d build ] && mkdir build
cd build

export CC=clang
export CXX=clang++
cmake -DDISABLE_ROS=TRUE -DCLANG_TIDY=clang-tidy ../khmot
make
make test

cd -
