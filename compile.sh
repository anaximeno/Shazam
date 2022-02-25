#!/bin/bash

COMPILER=g++
CPP_VERSION=c++17
FILE=main.cpp
OUTPUT_FILE=main


echo "Compiling..."

$COMPILER -std=$CPP_VERSION $FILE -o $OUTPUT_FILE $@

echo "Compilation finished!"
echo "Now you can run the program using: ./main"
