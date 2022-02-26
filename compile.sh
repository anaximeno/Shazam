#!/bin/bash

COMPILER=g++
CPP_VERSION=c++17
FILE=main.cpp
OUTPUT_FILE=main

# This pre out file can be used to verify if a new build
# was created while preservating the last build if not.
PRE_OUT_FILE=.$OUTPUT_FILE.pre.out

# Now trying to compile the program:
echo -n "Compiling... "

# Running #########################################>>
$COMPILER -std=$CPP_VERSION $FILE -o $PRE_OUT_FILE $@
###################################################>>


if [ -f $PRE_OUT_FILE ]
then
    # Running #################>>
    mv $PRE_OUT_FILE $OUTPUT_FILE
    ###########################>>
    
    echo "Done!"
    echo "Run it using: ./$OUTPUT_FILE"
else
    echo ""
    echo "Error Running -> $COMPILER -std=$CPP_VERSION $FILE -o $PRE_OUT_FILE $@"
fi
