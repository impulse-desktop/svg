#!/bin/sh
set -eu

status=0
svg-sanitizers -j "$(nproc)" -k -Dgroup="$TEST_GROUP" -Dgroup_count="$TEST_GROUP_COUNT" tests || status=$?

# Failed units prevent build from publishing any requested outputs. Restore
# the already computed corpus scores separately, preserving the test status.
svg-build -j "$(nproc)" -k -Dgroup="$TEST_GROUP" -Dgroup_count="$TEST_GROUP_COUNT" score_shard
exit "$status"
