#!/bin/bash
DATETIME=$(date)
COMPILER=g++
TESTFILE=tests.cpp
CPP_VERSION=c++17
RUNNING_ARGS=""
OUTPUT_FILE=.test_result.shazam.tmp

# To analyse the error then, you'll only need to use
# the command: cat test.last.log.txt
TEST_LOG_FILE=tests.log
LAST_LOG_FILE=.$TEST_LOG_FILE.tmp


echo -n "[1] Compiling... "
$COMPILER -std=$CPP_VERSION $TESTFILE -o $OUTPUT_FILE

if [ -f $OUTPUT_FILE ]
then
    echo "Done!"
    echo ""
    echo -n "[2] Running tests... "
    ./$OUTPUT_FILE $RUNNING_ARGS > $LAST_LOG_FILE

    if [ -f $LAST_LOG_FILE ]
    then
        echo "Done!"
        echo ""
        echo -n "[3] Cleaning... "
        rm $OUTPUT_FILE
        echo "Done!"
        # Store the last test log 
        echo -n "[$DATETIME] " >> $TEST_LOG_FILE && cat $LAST_LOG_FILE >> $TEST_LOG_FILE
        echo "" >> $TEST_LOG_FILE
        echo ""
        echo -n "[4] Result: " && cat $LAST_LOG_FILE
        rm $LAST_LOG_FILE
    else
        rm $OUTPUT_FILE
        echo ""
        echo "Error Running -> ./$OUTPUT_FILE $RUNNING_ARGS > $LAST_LOG_FILE"
    fi
else
    echo ""
    echo "Error Running -> $COMPILER -std=$CPP_VERSION $TESTFILE -o $OUTPUT_FILE"
fi
