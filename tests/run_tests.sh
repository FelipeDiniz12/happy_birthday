#!/usr/bin/env bash
CWD=`pwd`
cd $CWD/tests

TESTS=`find $CWD -type f -name "*.sh"`

for test in $TESTS; do
    if [[ "$test" == *"test_utils.sh"* ]]; then
        continue
    fi
    if [[ "$test" == *"run_tests.sh"* ]]; then
        continue
    fi
    echo "========================================"
    echo "Running test: $test"
    echo "========================================"
    bash "$test"
    if [ $? -ne 0 ]; then
        echo "Test $test failed."
        exit 1
    else
        echo "Test $test passed."
    fi
done