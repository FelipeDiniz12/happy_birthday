#!/usr/bin/env bash
CWD=`pwd`

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
done