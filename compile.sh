#!/bin/bash

COMPILER=g++
CPP_VERSION=c++17
FILE=main.cpp
OUTPUT_FILE=main
PRE_OUT_FILE=.$OUTPUT_FILE.pre.out

echo -n "Compiling... "

# Running #########################################>>
$COMPILER -std=$CPP_VERSION $FILE -o $PRE_OUT_FILE $@

if [ $? -eq 0 ]
then
    mv $PRE_OUT_FILE $OUTPUT_FILE
    echo "Done!"
    echo "Run it using: ./$OUTPUT_FILE"
else
    echo "Error Running: '$COMPILER -std=$CPP_VERSION $FILE -o $OUTPUT_FILE $@'"
fi
