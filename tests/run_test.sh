#!/bin/bash

COMPILER=g++
TESTFILE="tests.cpp"
CPP_VERSION=c++17
ARGS=""
OUTPUT_FILE=".test_result.tmp"

# To analyse the error then, you'll only need to use
# the command: cat test.last.log.txt
TEST_LOG_FILE="test.last.log.txt"

echo "[1] Compiling..."
echo ""
$COMPILER -std=$CPP_VERSION $TESTFILE -o $OUTPUT_FILE
echo "[2] Running tests..."
./$OUTPUT_FILE $ARGS > $TEST_LOG_FILE
echo ""
echo "[3] Cleaning..."
rm $OUTPUT_FILE
echo ""
echo "[4] Done!"
echo ""
echo ""
echo -n "[5] Result: "
cat $TEST_LOG_FILE