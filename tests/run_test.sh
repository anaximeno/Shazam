#!/bin/bash

COMPILER=g++
TESTFILE="tests.cpp"
CPP_VERSION=c++17
ARGS=""
OUTPUT_FILE=".test_result.shazam.tmp"

# To analyse the error then, you'll only need to use
# the command: cat test.last.log.txt
TEST_LOG_FILE="test.last.log.txt"

echo -n "[1] Compiling..."

$COMPILER -std=$CPP_VERSION $TESTFILE -o $OUTPUT_FILE

if [ -f $OUTPUT_FILE ]
then
    echo " Done!"
    echo ""
    echo -n "[2] Running tests..."
    ./$OUTPUT_FILE $ARGS > $TEST_LOG_FILE
    echo " Done!"
    echo ""
    echo -n "[3] Cleaning..."
    rm $OUTPUT_FILE
    echo " Done!"
    echo ""
    echo -n "[4] Result: "
    cat $TEST_LOG_FILE
else
    echo ""
    echo "Error!!"
fi
