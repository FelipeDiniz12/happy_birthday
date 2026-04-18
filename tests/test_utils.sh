#!/usr/bin/env bash

# This is simply a helper file containing utility functions for the various test scripts
# It is not meant to be run directly, but sourced by the test scripts

REPOSITORY_BASE_DIR="$(dirname "$(realpath "${BASH_SOURCE[0]}")")/../"

# Variables to track test results
passed=0
failed=0
current_output=""
PID=

# Clean up existing database for fresh test
cleanup()
{
    rm -f *.db
}

send_input() {
    local input="$1"
    local label="$2"

    echo ">>> [$label] Input: $input"
    echo "$input" >&${PROC[1]}
    current_output=""
    sleep 0.5
}

read_output() {
    while IFS= read -r -t 0.3 line <&${PROC[0]}; do
        current_output+="$line"$'\n'
        echo "    $line"
    done
}

# Assert that output contains expected text
assert_contains() {
    local expected="$1"
    local test_name="$2"

    echo "DEBUG: $current_output"  # Debugging output to see what was captured

    if echo "$current_output" | grep -q "$expected"; then
        echo "    ✓ PASS: Found \"$expected\""
        ((passed++))
    else
        echo "    ✗ FAIL: Expected \"$expected\" but not found"
        ((failed++))
    fi
}

finish()
{
    echo ""
    echo "=== EXIT ==="
    send_input "exit" "exit"

    wait $PID 2>/dev/null

    # Print summary
    echo ""
    echo "========================================"
    echo "TEST SUMMARY"
    echo "========================================"
    echo "✓ Passed: $passed"
    echo "✗ Failed: $failed"
    echo "Total: $((passed + failed))"
    echo "Process $PID completed"
}

startup()
{
    cleanup
    # Start the program as a coprocess
    coproc PROC { stdbuf -o0 -e0 $REPOSITORY_BASE_DIR/happy_birthday 2>&1; }

    PID=$PROC_PID
    echo "Started happy_birthday with PID: $PID"
    echo ""

    # Reset test results
    passed=0
    failed=0
    current_output=""

    # Read and discard initial output (welcome banner and first prompt)
    sleep 0.2
    while IFS= read -r -t 0.3 line <&${PROC[0]}; do
        :  # Discard the line
    done
}