#!/usr/bin/bash

set -xe

CC=clang

$CC -Wall -Wextra -o crono crono.c `sdl2-config --cflags --libs` -lSDL2_ttf
