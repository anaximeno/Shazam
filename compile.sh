#!/bin/bash

COMPILER=g++
CPP_VERSION=c++17
FILE=main.cpp
OUTPUT_FILE=main


echo -n "Compiling..."

$COMPILER -std=$CPP_VERSION $FILE -o $OUTPUT_FILE $@

echo " Done!"

echo "If no errors have occured, run the program using: ./main"
