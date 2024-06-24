#!/bin/zsh

TEST_NUM=$1;
echo "Running test $TEST_NUM"

if [ -z "$TEST_NUM" ]; then
    echo "Usage: ./run_test.sh <test_num>"
    exit 1
fi

make clean


if [ "$TEST_NUM" = "1" ]; then 
    make test_sm;
    clear;
    ./test_sm
fi

if [ "$TEST_NUM" = "2" ]; then 
    make test_pipe;
    clear;
    ./test_pipe
fi

