#!/bin/bash
BOARDS=$(pio boards | grep SAMD21G18A | awk '{ print $1 }')

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
