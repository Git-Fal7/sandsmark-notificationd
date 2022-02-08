#!/bin/sh


if [ -d ./build/ ]; then
 rm -rf ./build/
fi

mkdir -p build
cd build/
cmake .. -DCMAKE_INSTALL_PREFIX=/usr
make
