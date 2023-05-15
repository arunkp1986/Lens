#!/bin/bash
CWD=$(pwd)

cd ../pagetable-extract/page_table_module
make clean
rmmod ptextract
make
insmod ptextract.ko
cd $CWD
