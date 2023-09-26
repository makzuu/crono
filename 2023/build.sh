#!/usr/bin/bash

set -xe

CC="clang"
CFLAGS="-Wall -Wextra -pedantic"
LIBS="`sdl2-config --cflags --libs` -lSDL2_ttf"

$CC $CFLAGS -o crono crono.c $LIBS
