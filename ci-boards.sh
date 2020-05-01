#!/bin/bash
BOARDS=$(pio boards --json-output | jq --raw-output '.[] | select(.mcu | startswith("SAMD21")) | select(.frameworks[] | select(. == "arduino")) | .id' | grep -v -f <(cut -d' ' -f1 ci-boards-failing.txt) | sort)

FAILED=""
for BOARD in $BOARDS; do
    echo ============================================================ $BOARD
    pio ci examples/ZeroRegsExample --lib src --board $BOARD || FAILED="$FAILED $BOARD"
done
if [[ -n "$FAILED" ]]; then
    echo ============================================================
    echo FAILED BOARDS: $FAILED
    exit 1
fi
