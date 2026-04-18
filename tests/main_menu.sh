#!/usr/bin/env bash
source test_utils.sh

startup

# Scenario 1: Successful Registration
echo "=== SCENARIO 1: Successful Registration ==="
send_input "register" "register"
send_input "user1" "username"
send_input "pass123" "password"

send_input "pass122" "confirm_error"
read_output
assert_contains "Passwords differ" "password mismatch"

send_input "pass123" "confirm"
read_output
assert_contains "was registered" "registration success"

# Scenario 2: Successful Login
echo ""
echo "=== SCENARIO 2: Successful Login ==="
send_input "login" "login"
send_input "user1" "username"

send_input "pass12" "password_wrong"
read_output
assert_contains "Wrong password" "login failure"

send_input "pass123" "password"
read_output
assert_contains "Successfull authentication" "login success"

# Scenario 3: Try to register duplicate
echo ""
echo "=== SCENARIO 3: Duplicate User Registration ==="
send_input "register" "register again"
read_output
send_input "user1" "duplicate username"
read_output
send_input "pass123" "password"
read_output
send_input "pass123" "confirm"
read_output

assert_contains "already exists" "duplicate check"

finish
exit $failed
