#include <ZeroRegs.h>

void setup() {
  Serial.begin(9600);
  while (! Serial) {}  // wait for serial monitor to attach
  ZeroRegOptions opts = { Serial, false };
  printZeroRegs(opts);
}

void loop() {
  // Do nothing
}
