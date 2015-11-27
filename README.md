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
    printZeroRegs(SerialUSB);
}
```


Function Reference
------------------


### `void printZeroRegs(Stream &ser)`
Prints out configuration registers, as many as the library knows how.


### `void printZeroRegAC(Stream &ser)`
Prints out the configuration registers for the `AC` peripheral.


### `void printZeroRegADC(Stream &ser)`
Prints out the configuration registers for the `ADC` peripheral.


### `void printZeroRegDAC(Stream &ser)`
Prints out the configuration registers for the `DAC` peripheral.


### `void printZeroRegDMAC(Stream &ser)`
Prints out the configuration registers for the `DMAC` peripheral.


### `void printZeroRegDSU(Stream &ser)`
Prints out the configuration registers for the `DSU` peripheral.


### `void printZeroRegEIC(Stream &ser)`
Prints out the configuration registers for the `EIC` peripheral.


### `void printZeroRegEVSYS(Stream &ser)`
Prints out the configuration registers for the `EVSYS` peripheral.


### `void printZeroRegGCLK(Stream &ser)`
Prints out the configuration registers for the `GCLK` peripheral.


### `void printZeroRegI2S(Stream &ser)`
Prints out the configuration registers for the `I2S` peripheral.


### `void printZeroRegMTB(Stream &ser)`
Prints out the configuration registers for the `MTB` peripheral.


### `void printZeroRegNVMCTRL(Stream &ser)`
Prints out the configuration registers for the `NVMCTRL` peripheral.


### `void printZeroRegPACs(Stream &ser)`
Prints out the configuration registers for the `PAC` peripherals.


### `void printZeroRegPM(Stream &ser)`
Prints out the configuration registers for the `PM` peripheral.


### `void printZeroRegPORT(Stream &ser)`
Prints out the configuration registers for the `PORT` peripheral.


### `void printZeroRegRTC(Stream &ser)`
Prints out the configuration registers for the `RTC` peripheral.


### `void printZeroRegSBMATRIX(Stream &ser)`
Prints out the configuration registers for the `SBMATRIX` peripheral.


### `void printZeroRegSERCOM(Stream &ser, Sercom* sercom, uint8_t idx)`
Prints out the configuration registers for a `SERCOM` peripheral.


### `void printZeroRegSYSCTRL(Stream &ser)`
Prints out the configuration registers for the `SYSCTRL` peripheral.


### `void printZeroRegTC(Stream &ser, Tc* tc, uint8_t idx)`
Prints out the configuration registers for a `TC` peripheral.


### `void printZeroRegTCC(Stream &ser, Tcc* tcc, uint8_t idx)`
Prints out the configuration registers for a `TCC` peripheral.


### `void printZeroRegUSB(Stream &ser)`
Prints out the configuration registers for the `USB` peripheral.


### `void printZeroRegWDT(Stream &ser)`
Prints out the configuration registers for the `WDT` peripheral.


License
-------
This code is licensed under the MIT license.
See the `LICENSE` file for details.


