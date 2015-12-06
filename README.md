arduino ZeroRegs
================
prints the configuration registers for the Arduino Zero (and similar boards)

**Intended Audience:** advanced users who have already looked at the SAM D21 datasheet


Simple Example
--------------
```cpp
#include <ZeroRegs.h>
void setup() {
    SerialUSB.begin(9600);
    while (! SerialUSB) {}  // wait for serial monitor to attach
    ZeroRegOptions opts = { SerialUSB, false };
    printZeroRegs(opts);
}
```


Function Reference
------------------


### struct ZeroRegOptions

| type | name | description |
| ---- | ---- | ---- |
| `Stream&` | `ser` | stream used for printing output |
| `bool` | `showDisabled` | whether to show itmes which are disabled |


### void printZeroRegs(ZeroRegOptions &opts)
Prints out configuration registers, as many as the library knows how.


### void printZeroRegAC(ZeroRegOptions &opts) -- **NOT YET IMPLEMENTED**
Prints out the configuration registers for the `AC` peripheral.


### void printZeroRegADC(ZeroRegOptions &opts) -- **NOT YET IMPLEMENTED**
Prints out the configuration registers for the `ADC` peripheral.


### void printZeroRegDAC(ZeroRegOptions &opts) -- **NOT YET IMPLEMENTED**
Prints out the configuration registers for the `DAC` peripheral.


### void printZeroRegDMAC(ZeroRegOptions &opts) -- **NOT YET IMPLEMENTED**
Prints out the configuration registers for the `DMAC` peripheral.


### void printZeroRegDSU(ZeroRegOptions &opts) -- **NOT YET IMPLEMENTED**
Prints out the configuration registers for the `DSU` peripheral.


### void printZeroRegEIC(ZeroRegOptions &opts)
Prints out the configuration registers for the `EIC` peripheral.


### void printZeroRegEVSYS(ZeroRegOptions &opts)
Prints out the configuration registers for the `EVSYS` peripheral.


### void printZeroRegGCLK(ZeroRegOptions &opts)
Prints out the configuration registers for the `GCLK` peripheral.


### void printZeroRegI2S(ZeroRegOptions &opts) -- **NOT YET IMPLEMENTED**
Prints out the configuration registers for the `I2S` peripheral.


### void printZeroRegMTB(ZeroRegOptions &opts) -- **NOT YET IMPLEMENTED**
Prints out the configuration registers for the `MTB` peripheral.


### void printZeroRegNVMCTRL(ZeroRegOptions &opts)
Prints out the configuration registers for the `NVMCTRL` peripheral.


### void printZeroRegPACs(ZeroRegOptions &opts)
Prints out the configuration registers for the `PAC` peripherals.


### void printZeroRegPM(ZeroRegOptions &opts)
Prints out the configuration registers for the `PM` peripheral.


### void printZeroRegPORT(ZeroRegOptions &opts)
Prints out the configuration registers for the `PORT` peripheral.


### void printZeroRegRTC(ZeroRegOptions &opts)
Prints out the configuration registers for the `RTC` peripheral.


### void printZeroRegSBMATRIX(ZeroRegOptions &opts)
Prints out the configuration registers for the `SBMATRIX` peripheral.


### void printZeroRegSERCOM(ZeroRegOptions &opts, Sercom* sercom, uint8_t idx)
Prints out the configuration registers for a `SERCOM` peripheral.


### void printZeroRegSYSCTRL(ZeroRegOptions &opts)
Prints out the configuration registers for the `SYSCTRL` peripheral.


### void printZeroRegTC(ZeroRegOptions &opts, Tc* tc, uint8_t idx) -- **NOT YET IMPLEMENTED**
Prints out the configuration registers for a `TC` peripheral.


### void printZeroRegTCC(ZeroRegOptions &opts, Tcc* tcc, uint8_t idx) -- **NOT YET IMPLEMENTED**
Prints out the configuration registers for a `TCC` peripheral.


### void printZeroRegUSB(ZeroRegOptions &opts) -- **NOT YET IMPLEMENTED**
Prints out the configuration registers for the `USB` peripheral.


### void printZeroRegWDT(ZeroRegOptions &opts)
Prints out the configuration registers for the `WDT` peripheral.


License
-------
This code is licensed under the MIT license.
See the `LICENSE` file for details.


