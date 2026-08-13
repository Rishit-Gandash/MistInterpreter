#!/bin/bash
mkdir -p build &&
gcc -Wextra -ggdb -Wall ./tests/test_lexer.c -o build/lexer_test && ./build/lexer_test
