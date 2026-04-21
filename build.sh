#!/bin/sh

rm -Rf build
cmake -S . -B build
cd build/
make
