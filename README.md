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


Output Conventions
------------------
The following conventions are followed as best as possible:

Each peripheral prints its own section, with a header.
Example: `--------------------------- PERIPHERAL`

Each register is printed on it's own line.
Example: `REGISTRY:  ...fields...`.

Register fields which are boolean flags are either shown (if set) or not shown (if not set).

Register fields which aren't boolean flags are shown as `NAME=VALUE`.
If the value is printed directly (either decimal or hexadecimal) then the field name will be uppercase.
Hexadecimal values are always prefixed with `0x`.
If the value is interpretted in any way (such as referenced from a table) then the field name will be lowercase.

Any editorial comments made by this library will be in lower case surrounded by parentheses.
Example: `(slow)`.


Function Reference
------------------


### struct ZeroRegOptions

| type | name | description |
| ---- | ---- | ---- |
| `Print&` | `out` | object used for printing output |
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


### void printZeroRegEIC(ZeroRegOptions &opts)
Prints out the configuration registers for the `EIC` peripheral.

example output:
```text
--------------------------- EIC
EXTINT03:  sense=FALL WAKEUP
```


### void printZeroRegEVSYS(ZeroRegOptions &opts)
Prints out the configuration registers for the `EVSYS` peripheral.


### void printZeroRegGCLK(ZeroRegOptions &opts)
Prints out the configuration registers for the `GCLK` peripheral.

example output:
```text
--------------------------- GCLK
GEN00:  GENEN DFLL48M IDC OOV=0
GEN01:  GENEN XOSC32K OOV=0
GEN02:  GENEN XOSC32K/32 OOV=0
GEN03:  GENEN OSC8M OOV=0
GCLK_MAIN:  GEN00 (always)
GCLK_DFLL48M_REF:  CLKEN GEN01
GCLK_RTC:  CLKEN GEN02
GCLK_EIC:  CLKEN GEN00
GCLK_USB:  CLKEN GEN00
GCLK_SERCOM3_CORE:  CLKEN GEN00
GCLK_ADC:  CLKEN GEN00
GCLK_DAC:  CLKEN GEN00
```


### void printZeroRegI2S(ZeroRegOptions &opts) -- **NOT YET IMPLEMENTED**
Prints out the configuration registers for the `I2S` peripheral.


### void printZeroRegNVMCTRL(ZeroRegOptions &opts)
Prints out the configuration registers for the `NVMCTRL` peripheral.

example output:
```text
--------------------------- NVMCTRL
CTRLB:  RWS=1 MANW sleepprm=WAKEONACCESS readmode=NO_MISS_PENALTY
PARAM:  NVMP=4096 psz=64bytes
LOCK:  1111111111111111
user row:  bootprot=0 eeprom_size=0 region_locks=1111111111111111
software calibration:  ADC_LINEARITY=0x10 ADC_BIAS=0x7 OSC32K_CAL=0x7E USB_TRANSN=0xA USB_TRANSP=0x1A USB_TRIM=0x7 DFLL48M_COARSE_CAL=0x2C
serial # 0x59FC4595 0x514D464F 0x4F202020 0xFF140C28
```


### void printZeroRegPAC(ZeroRegOptions &opts)
Prints out the configuration registers for the `PAC` peripherals.


### void printZeroRegPM(ZeroRegOptions &opts)
Prints out the configuration registers for the `PM` peripheral.

example output:
```text
--------------------------- PM
SLEEP:  idle=CPU
CPUSEL:  /1
APBASEL:  /1
APBBSEL:  /1
APBCSEL:  /1
AHBMASK:  CLK_HPBA_AHB CLK_HPBB_AHB CLK_HPBC_AHB CLK_DSU_AHB CLK_NVMCTRL_AHB CLK_DMAC_AHB CLK_USB_AHB
APBAMASK:  CLK_PAC0_APB CLK_PM_APB CLK_SYSCTRL_APB CLK_GCLK_APB CLK_WDT_APB CLK_RTC_APB CLK_EIC_APB
APBBMASK:  CLK_PAC1_APB CLK_DSU_APB CLK_NVMCTRL_APB CLK_PORT_APB CLK_DMAC_APB CLK_USB_APB
APBCMASK:  CLK_SERCOM0_APB CLK_SERCOM1_APB CLK_SERCOM2_APB CLK_SERCOM3_APB CLK_SERCOM4_APB CLK_SERCOM5_APB CLK_TCC0_APB CLK_TCC1_APB CLK_TCC2_APB CLK_TC3_APB CLK_TC4_APB CLK_TC5_APB CLK_TC6_APB CLK_TC7_APB CLK_ADC_APB CLK_DAC_APB
```


### void printZeroRegPORT(ZeroRegOptions &opts)
Prints out the configuration registers for the `PORT` peripheral.

example output:
```text
--------------------------- PORT
PA11 D0/RX:  pmux=SERCOM0:3
PA10 D1/TX:  pmux=SERCOM0:2
PA14 D2:  dir=IN INEN
PA09 D3:  dir=IN INEN
PA08 D4:  dir=IN INEN
PA15 D5:  dir=IN INEN
PA20 D6:  dir=IN INEN
PA21 D7:  dir=IN INEN
PA06 D8:  dir=IN INEN
PA07 D9:  pmux=EIC:7
PA18 D10:  dir=IN INEN
PA16 D11:  pmux=SERCOM1:0
PA19 D12:  pmux=SERCOM1:3
PA17 D13:  pmux=SERCOM1:1
PA02 A0:  pmux=AIN0/Y0/VOUT
PA22 SDA:  pmux=SERCOM3:0
PA23 SCL:  pmux=SERCOM3:1
PA24 USB_DM:  pmux=USB:DM
PA25 USB_DP:  pmux=USB:DP
PA22 EDBG_SDA:  pmux=SERCOM3:0
PA23 EDBG_SCL:  pmux=SERCOM3:1
PA19 EDBG_MISO:  pmux=SERCOM1:3
PA16 EDBG_MOSI:  pmux=SERCOM1:0
PA18 EDBG_SS:  dir=IN INEN
PA17 EDBG_SCK:  pmux=SERCOM1:1
PA21 EDBG_GPIO1:  dir=IN INEN
PA06 EDBG_GPIO2:  dir=IN INEN
PA07 EDBG_GPIO3:  pmux=EIC:7
```

### void printZeroRegPORT_Arduino(ZeroRegOptions &opts)
Prints out the `PORT` configuration for the Arduino pins.

example output:
```text
--------------------------- ARDUINO PINS
D0:  --disabled--
D1:  --disabled--
D2:  --disabled--
D3:  --disabled--
D4:  --disabled--
D5:  --disabled--
D6:  --disabled--
D7:  --disabled--
D8:  --disabled--
D9:  --disabled--
D10:  --disabled--
D11:  --disabled--
D12:  pmux=EIC:3 input INEN pull=UP
D13:  output INEN
A0:  --disabled--
A1:  --disabled--
A2:  --disabled--
A3:  --disabled--
A4:  --disabled--
A5:  --disabled--
D20:  pmux=SERCOM3:0
D21:  pmux=SERCOM3:1
D22:  --disabled--
D23:  --disabled--
D24:  --disabled--
D25:  output INEN
```


### void printZeroRegRTC(ZeroRegOptions &opts)
Prints out the configuration registers for the `RTC` peripheral.

example output:
```text
--------------------------- RTC MODE2
CTRL:  PRESCALER=0xA(GCLK_RTC/1024)
READREQ:
EVCTRL:
FREQCORR:  0x0
ALARM0:  00-00-00 00:00:00
MASK0:  SS
```


### void printZeroRegSERCOM(ZeroRegOptions &opts, Sercom* sercom, uint8_t idx)
Prints out the configuration registers for a `SERCOM` peripheral.

example output:
```text
--------------------------- SERCOM1 SPI master
CTRLA:  miso=PAD3 mosi=PAD0 sck=PAD1 form=SPI cpha=LEADING cpol=LOW dord=MSB
CTRLB:  chsize=8bit amode=MASK RXEN
BAUD:  0x5
```


### void printZeroRegSYSCTRL(ZeroRegOptions &opts)
Prints out the configuration registers for the `SYSCTRL` peripheral.

example output:
```text
--------------------------- SYSCTRL
OSCULP32K:  CALIB=0x10
OSC8M:  ENABLE presc=1 CALIB=0x80E frange=8-11MHz
XOSC32K:  ENABLE XTALEN EN32K RUNSTDBY ONDEMAND STARTUP=0x6
DFLL:  ENABLE mode=closed-loop QLDIS WAITLOCK MUL=1465
BOD33:  ENABLE action=RESET psel=2 LEVEL=0x7
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


