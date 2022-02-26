#!/bin/bash

COMPILER=g++
CPP_VERSION=c++17
FILE=main.cpp
OUTPUT_FILE=main

# Now trying to compile the program:
echo -n "Compiling..."

$COMPILER -std=$CPP_VERSION $FILE -o $OUTPUT_FILE $@

if [ -f $OUTPUT_FILE ]
then
    echo " Done!"
    echo "Run it using: ./main"
else
    echo ""
    echo "Error!!"
fi
