#!/usr/bin/env bash
source test_utils.sh

output=$(run_scenario "Successful Registration with Password Mismatch" "register
user1
pass123
pass122
pass123
exit")
assert_contains "$output" "Passwords differ" "password mismatch"
assert_contains "$output" "was registered" "registration success"

output=$(run_scenario "Successful Login" "register
testuser
pass123
pass123
login
testuser
pass123
exit")
assert_contains "$output" "was registered" "login setup"
assert_contains "$output" "Successfull authentication" "login success"

output=$(run_scenario "Duplicate User Registration" "register
duplicateuser
pass123
pass123
register
duplicateuser
pass123
pass123
exit")
assert_contains "$output" "already exists" "duplicate user detection"

print_summary
exit $failed
