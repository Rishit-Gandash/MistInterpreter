#!/bin/bash
set -x
sh ./clean.sh
sh ./build.sh
python3 test.py