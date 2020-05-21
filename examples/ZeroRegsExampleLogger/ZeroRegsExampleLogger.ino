#include <ZeroRegs.h>
#include "ExampleLogPrint.h"

ExampleLogPrint logger(Serial);
bool serialShown = false;

void setup() {
    Serial.begin(9600);
}

void loop() {
    if (Serial) {
        if (! serialShown) {
            ZeroRegOptions opts = { logger, false };
            printZeroRegs(opts);
            serialShown = true;
        }
    } else {
        serialShown = false;
    }
}
