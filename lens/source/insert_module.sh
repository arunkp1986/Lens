#!/bin/bash
CWD=$(pwd)

cd ../kernel_module
make clean
rmmod ptextract
make
insmod ptextract.ko
cd $CWD
