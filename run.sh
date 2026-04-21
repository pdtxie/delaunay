#!/bin/sh

rm -Rf build
meson setup build
meson compile -C build
