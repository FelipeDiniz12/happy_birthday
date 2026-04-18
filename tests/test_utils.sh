#!/usr/bin/env bash

# Portable test utilities for happy_birthday
# Uses input-file redirection instead of coproc for CI compatibility

REPOSITORY_BASE_DIR="$(dirname "$(realpath "${BASH_SOURCE[0]}")")/../"

passed=0
failed=0

cleanup() {
    rm -f *.db
}

run_scenario() {
    local scenario_name="$1"
    local input_data="$2"
    local tmp_input="/tmp/happy_birthday_test_input_$$.txt"

    cleanup
    echo ""
    echo "=========================================================================="
    echo "SCENARIO: $scenario_name"
    echo "=========================================================================="

    printf '%s\n' "$input_data" > "$tmp_input"

    if command -v unbuffer >/dev/null 2>&1; then
        output=$(unbuffer "$REPOSITORY_BASE_DIR/happy_birthday" < "$tmp_input" 2>&1)
    else
        output=$(stdbuf -oL "$REPOSITORY_BASE_DIR/happy_birthday" < "$tmp_input" 2>&1)
    fi

    rm -f "$tmp_input"

    echo "$output"
    echo ""
    echo "$output"
}

assert_contains() {
    local output="$1"
    local expected="$2"
    local test_name="$3"

    if echo "$output" | grep -q "$expected"; then
        echo "✓ PASS: $test_name"
        ((passed++))
    else
        echo "✗ FAIL: $test_name (expected: \"$expected\")"
        ((failed++))
    fi
}

print_summary() {
    echo ""
    echo "========================================"
    echo "TEST SUMMARY"
    echo "========================================"
    echo "✓ Passed: $passed"
    echo "✗ Failed: $failed"
    echo "Total: $((passed + failed))"
}
