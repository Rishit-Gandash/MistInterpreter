#!/bin/bash
set -x
mkdir -p build
gcc -Wextra -ggdb -Wall -c interpreter/interpreter.c -o build/interpreter.o
gcc -Wextra -ggdb -Wall build/interpreter.o ./tests/lexer/lexer.c -o build/lexer