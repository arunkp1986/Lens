#!/bin/bash

sudo ./insert_module.sh

gcc program_tmp.c -o program > output.out 2>&1

./program > output.out 2>&1
