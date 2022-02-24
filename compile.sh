#!/bin/bash

COMPILER=g++
CPP_VERSION=c++17
FILE=main.cpp
INCLUDE_LIB=/usr/include/hashlib++/
LINK_LIB=lhl++
OUTPUT_FILE=main


echo "Compiling..."

$COMPILER -std=$CPP_VERSION $FILE -I$INCLUDE_LIB -$LINK_LIB -o $OUTPUT_FILE

echo "Compilation finished!"
echo "Now you can run the program using: ./main"