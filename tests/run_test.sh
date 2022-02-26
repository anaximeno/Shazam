#!/bin/bash

COMPILER=g++
TESTFILE=tests.cpp
CPP_VERSION=c++17
ARGS=""
OUTPUT_FILE=.test_result.shazam.tmp

# To analyse the error then, you'll only need to use
# the command: cat test.last.log.txt
TEST_LOG_FILE=test.last.log.txt


echo -n "[1] Compiling... "

# Running #########################################>>
$COMPILER -std=$CPP_VERSION $TESTFILE -o $OUTPUT_FILE
###################################################>>


if [ -f $OUTPUT_FILE ]
then
    echo "Done!"
    echo ""
    echo -n "[2] Running tests... "

    # Running #########################>>
    ./$OUTPUT_FILE $ARGS > $TEST_LOG_FILE
    ###################################>>

    if [ -f $TEST_LOG_FILE ]
    then
        echo "Done!"
        echo ""
        echo -n "[3] Cleaning... "

        # Running ###>>
        rm $OUTPUT_FILE
        #############>>

        echo "Done!"
        echo ""
        echo -n "[4] Result: "

        # Running ######>>
        cat $TEST_LOG_FILE
        ################>>
    else
        # Running ###>>
        rm $OUTPUT_FILE
        #############>>

        echo ""
        echo "Error Running -> ./$OUTPUT_FILE $ARGS > $TEST_LOG_FILE"
    fi
else
    echo ""
    echo "Error Running -> $COMPILER -std=$CPP_VERSION $TESTFILE -o $OUTPUT_FILE"
fi
