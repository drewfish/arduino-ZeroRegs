#!/bin/bash
BOARDS=$(pio boards | grep SAMD21G18A | awk '{ print $1 }' | grep -v adafruit_circuitplayground_m0 | grep -v moteino_zero)
##### 2018-Aug-19:  adafruit_circuitplayground_m0 fails with a bunch of
# fatal error: sam.h: No such file or directory
##### 2018-Aug-19:  moteino_zero fails with a bunch of
# fatal error: variant.h: No such file or directory

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
