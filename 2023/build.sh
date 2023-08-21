#!/usr/bin/bash

set -xe

CC=clang

$CC -Wall -Wextra -o crono crono.c
