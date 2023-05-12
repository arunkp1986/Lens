#!/bin/bash
#kernel module make
#insmod
gcc program_tmp.c -o program

./program > output.out 2>&1
