arduino ZeroRegs
================
This library prints the registers for the Arduino Zero (and similar boards).
It specifically is designed to print the values of the _configuration_ register
but not the _status_ registers.

**Intended Audience:** advanced users who have already looked at the SAM D21 datasheet.

[![Build Status](https://travis-ci.org/drewfish/arduino-ZeroRegs.svg?branch=master)](https://travis-ci.org/drewfish/arduino-ZeroRegs)

Examples
--------
This will show the registers when the device boots:
```cpp
#include <ZeroRegs.h>
void setup() {
    SerialUSB.begin(9600);
    while (! SerialUSB) {}  // wait for serial monitor to attach
    ZeroRegOptions opts = { SerialUSB, false };
    printZeroRegs(opts);
}
```


This will show the registers every time USB is connected:
```cpp
#include <ZeroRegs.h>
bool serialShown = false;
void setup() {
    SerialUSB.begin(9600);
}
void loop() {
    if (SerialUSB) {
        if (! serialShown) {
            ZeroRegOptions opts = { SerialUSB, false };
            printZeroRegs(opts);
            serialShown = true;
        }
    } else {
        serialShown = false;
    }
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
Prints out configuration registers, as many as this library knows how.


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

example output:
```text
--------------------------- EIC
EXTINT07:  BOTH
```


### void printZeroRegEVSYS(ZeroRegOptions &opts)
Prints out the configuration registers for the `EVSYS` peripheral.


### void printZeroRegGCLK(ZeroRegOptions &opts)
Prints out the configuration registers for the `GCLK` peripheral.

example output:
```text
--------------------------- GCLK
GCLK_MAIN:  GEN00 (always)
GCLK_DFLL48M_REF:  GEN01
GCLK_RTC:  GEN02
GCLK_EIC:  GEN00
GCLK_USB:  GEN00
GCLK_SERCOM0_CORE:  GEN00
GCLK_SERCOM1_CORE:  GEN00
GCLK_SERCOM3_CORE:  GEN00
GCLK_ADC:  GEN00
GCLK_DAC:  GEN00
GEN00:  DFLL48M IDC
GEN01:  XOSC32K
GEN02:  XOSC32K/32
GEN03:  OSC8M
```


### void printZeroRegI2S(ZeroRegOptions &opts) -- **NOT YET IMPLEMENTED**
Prints out the configuration registers for the `I2S` peripheral.


### void printZeroRegMTB(ZeroRegOptions &opts) -- **NOT YET IMPLEMENTED**
Prints out the configuration registers for the `MTB` peripheral.


### void printZeroRegNVMCTRL(ZeroRegOptions &opts)
Prints out the configuration registers for the `NVMCTRL` peripheral.

example output:
```text
--------------------------- NVMCTRL
CTRLB:  RWS=1 SLEEPPRM=WAKEONACCESS READMODE=NO_MISS_PENALTY
PARAM:  NVMP=4096 PSZ=64 RWWEEPROM=0
STATUS:
LOCK:  1111111111111111
user row:  BOOTPROT=0 EEPROM_SIZE=0 REGION_LOCKS=1111111111111111
serial # 0x594A1C9A 0x514D3246 0x43202020 0xFF0A1919
```


### void printZeroRegPACs(ZeroRegOptions &opts)
Prints out the configuration registers for the `PAC` peripherals.


### void printZeroRegPM(ZeroRegOptions &opts)
Prints out the configuration registers for the `PM` peripheral.

example output:
```text
--------------------------- PM
SLEEP:  IDLE=CPU
EXTCTRL:
CPUSEL:  CPUDIV=1
APBASEL:  APBADIV=1
APBBSEL:  APBBDIV=1
APBCSEL:  APBCDIV=1
AHBMASK:  CLK_HPBA_AHB CLK_HPBB_AHB CLK_HPBC_AHB CLK_DSU_AHB CLK_NVMCTRL_AHB CLK_DMAC_AHB CLK_USB_AHB
APBAMASK:  CLK_PAC0_APB CLK_PM_APB CLK_SYSCTRL_APB CLK_GCLK_APB CLK_WDT_APB CLK_RTC_APB CLK_EIC_APB
APBBMASK:  CLK_PAC1_APB CLK_DSU_APB CLK_NVMCTRL_APB CLK_PORT_APB CLK_DMAC_APB CLK_USB_APB
APBCMASK:  CLK_SERCOM0_APB CLK_SERCOM1_APB CLK_SERCOM2_APB CLK_SERCOM3_APB CLK_SERCOM4_APB CLK_SERCOM5_APB CLK_TCC0_APB CLK_TCC1_APB CLK_TCC2_APB CLK_TC3_APB CLK_TC4_APB CLK_TC5_APB CLK_ADC_APB CLK_DAC_APB
RCAUSE:  SYST
```


### void printZeroRegPORT(ZeroRegOptions &opts)
Prints out the configuration registers for the `PORT` peripheral.

example output:
```text
--------------------------- PORT
PA11 D0/RX:  PMUX=SERCOM0:3
PA10 D1/TX:  PMUX=SERCOM0:2
PA14 D2:  DIR=IN INEN
PA09 D3:  DIR=IN INEN
PA08 D4:  DIR=IN INEN
PA15 D5:  DIR=IN INEN
PA20 D6:  DIR=IN INEN
PA21 D7:  DIR=IN INEN
PA06 D8:  DIR=IN INEN
PA07 D9:  PMUX=EIC:7
PA18 D10:  DIR=IN INEN
PA16 D11:  PMUX=SERCOM1:0
PA19 D12:  PMUX=SERCOM1:3
PA17 D13:  PMUX=SERCOM1:1
PA02 A0:  PMUX=AIN0/Y0/VOUT
PA22 SDA:  PMUX=SERCOM3:0
PA23 SCL:  PMUX=SERCOM3:1
PA24 USB_DM:  PMUX=USB:DM
PA25 USB_DP:  PMUX=USB:DP
PA22 EDBG_SDA:  PMUX=SERCOM3:0
PA23 EDBG_SCL:  PMUX=SERCOM3:1
PA19 EDBG_MISO:  PMUX=SERCOM1:3
PA16 EDBG_MOSI:  PMUX=SERCOM1:0
PA18 EDBG_SS:  DIR=IN INEN
PA17 EDBG_SCK:  PMUX=SERCOM1:1
PA21 EDBG_GPIO1:  DIR=IN INEN
PA06 EDBG_GPIO2:  DIR=IN INEN
PA07 EDBG_GPIO3:  PMUX=EIC:7
```


### void printZeroRegRTC(ZeroRegOptions &opts)
Prints out the configuration registers for the `RTC` peripheral.

example output:
```text
--------------------------- RTC MODE2
CTRL:  PRESCALER=1024
READREQ:
EVCTRL:
FREQCORR:  0x0
ALARM:  00-00-00 00:00:00
MASK:  SS
```


### void printZeroRegSBMATRIX(ZeroRegOptions &opts)
Prints out the configuration registers for the `SBMATRIX` peripheral.


### void printZeroRegSERCOM(ZeroRegOptions &opts, Sercom* sercom, uint8_t idx)
Prints out the configuration registers for a `SERCOM` peripheral.

example output:
```text
--------------------------- SERCOM1 SPI master
CTRLA:  MISO=PAD3 MOSI=PAD0 SCK=PAD1 FORM=SPI CPHA=LEADING CPOL=LOW DORD=MSB
CTRLB:  CHSIZE=8bit AMODE=MASK RXEN
BAUD:  0x5
```


### void printZeroRegSYSCTRL(ZeroRegOptions &opts)
Prints out the configuration registers for the `SYSCTRL` peripheral.

example output:
```text
--------------------------- SYSCTRL
PCLKSR:  XOSC32KRDY OSC8MRDY DFLLRDY DFLLLCKF DFLLLCKC BOD33RDY B33SRDY
XOSC32K:  XTALEN EN32K RUNSTDBY ONDEMAND STARTUP=2000092us
OSCULP32K:  CALIB=0x11
OSC8M:  PRESC=1 CALIB=0x80D FRANGE=8-11MHz
DFLL:  MODE QLDIS WAITLOCK
BOD33:  ACTION=RESET PSEL=1 LEVEL=0x7
VREG:
VREF:  CALIB=0x60
```


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


