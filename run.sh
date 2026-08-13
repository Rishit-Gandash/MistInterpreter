#!/bin/bash
mkdir -p build &&
gcc -Wextra -ggdb -Wall interpreter.c -o build/binter && ./build/binter
