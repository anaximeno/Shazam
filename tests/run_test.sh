#!/bin/bash
DATETIME=$(date)
COMPILER=g++
TESTFILE=tests.cpp
CPP_VERSION=c++17
RUNNING_ARGS=""
OUTPUT_FILE=.test_result.shazam.tmp
COMPIL_ARGS="../src/shazam.cc ../include/external/hashlib2plus/*.cpp"

# The log of the test will be stored on the file bellow
# and can be reviwed using the cat command.
TEST_LOG_FILE=tests.log
LAST_LOG_FILE=.$TEST_LOG_FILE.tmp


PRINT_ERR() {
    echo -e "\nError Running -> $1"
}

COMPILE() {
    $COMPILER -std=$CPP_VERSION $TESTFILE -o $OUTPUT_FILE $COMPIL_ARGS
}

RUN_TEST() {
    ./$OUTPUT_FILE $RUNNING_ARGS > $LAST_LOG_FILE
}

SAVE_LOG() {
    echo -n "[$DATETIME] " >> $TEST_LOG_FILE && cat $LAST_LOG_FILE >> $TEST_LOG_FILE
    echo "" >> $TEST_LOG_FILE
}

echo -n "[1] Compiling... " && COMPILE

if [[ $? -eq 0 ]]
then
    echo -e "Done!\n"
    echo -n "[2] Running tests... "
    RUN_TEST

    if [[ $? -eq 0 ]]
    then
        echo -e "Done!\n"
        echo -n "[3] Cleaning... "
        rm $OUTPUT_FILE

        echo Done!

        # Store the last test log
        SAVE_LOG
        echo -n -e "\n[4] Result: " && cat $LAST_LOG_FILE
        rm $LAST_LOG_FILE
    else
        rm $OUTPUT_FILE
        PRINT_ERR "$OUTPUT_FILE $RUNNING_ARGS > $LAST_LOG_FILE"
    fi
else
    PRINT_ERR "$COMPILER -std=$CPP_VERSION $TESTFILE -o $OUTPUT_FILE"
fi
