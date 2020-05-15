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
Example: `REGISTER:  ...fields...`.

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
| `bool` | `showDisabled` | whether to show items which are disabled |


### void printZeroRegs(ZeroRegOptions &opts)
Prints out configuration registers, as many as this library knows how.


### void printZeroRegAC(ZeroRegOptions &opts)
Prints out the configuration registers for the `AC` peripheral.


### void printZeroRegADC(ZeroRegOptions &opts)
Prints out the configuration registers for the `ADC` peripheral.


### void printZeroRegDAC(ZeroRegOptions &opts)
Prints out the configuration registers for the `DAC` peripheral.


### void printZeroRegDMAC(ZeroRegOptions &opts)
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
GEN00:  GENEN DFLL48M IDC
GEN01:  GENEN XOSC32K
GEN02:  GENEN XOSC32K/32
GEN03:  GENEN OSC8M
GCLK_MAIN:  GEN00 (always)
GCLK_DFLL48M_REF:  CLKEN GEN01
GCLK_RTC:  CLKEN GEN02
GCLK_EIC:  CLKEN GEN00
GCLK_USB:  CLKEN GEN00
GCLK_SERCOM0_CORE:  CLKEN GEN00
GCLK_SERCOM3_CORE:  CLKEN GEN00
GCLK_SERCOM4_CORE:  CLKEN GEN00
GCLK_ADC:  CLKEN GEN00
GCLK_DAC:  CLKEN GEN00
```


### void printZeroRegI2S(ZeroRegOptions &opts)
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
--------------------------- PORT A
PA10:  pmux=SERCOM0:2(usart:tx)
PA11:  pmux=SERCOM0:3(usart:rx)
PA12:  pmux=SERCOM4:0(spi:miso)
PA17:  output INEN
PA19:  pmux=EIC:3 input INEN pull=UP
PA22:  pmux=SERCOM3:0(i2c:sda)
PA23:  pmux=SERCOM3:1(i2c:scl)
PA24:  pmux=USB:DN
PA25:  pmux=USB:DP
PA27:  output INEN
PA30:  pmux=SWCLK
--------------------------- PORT B
PB03:  output INEN
PB09:  pmux=ADC:3,Y15
PB10:  pmux=SERCOM4:2(spi:mosi)
PB11:  pmux=SERCOM4:3(spi:sck)
PB30:  output DRVSTR
```

### void printZeroRegPORT_Arduino(ZeroRegOptions &opts)
Prints out the `PORT` configuration for the Arduino pins.

example output:
```text
--------------------------- ARDUINO PINS
D0:  pmux=SERCOM0:3(usart:rx)
D1:  pmux=SERCOM0:2(usart:tx)
D12:  pmux=EIC:3 input INEN pull=UP
D13:  output INEN
A2:  pmux=ADC:3,Y15
D20:  pmux=SERCOM3:0(i2c:sda)
D21:  pmux=SERCOM3:1(i2c:scl)
D22:  pmux=SERCOM4:0(spi:miso)
D23:  pmux=SERCOM4:2(spi:mosi)
D24:  pmux=SERCOM4:3(spi:sck)
D25:  output INEN
```


### void printZeroRegRTC(ZeroRegOptions &opts)
Prints out the configuration registers for the `RTC` peripheral.

example output:
```text
--------------------------- RTC MODE2
CTRL:  ENABLE PRESCALER=0xA(GCLK_RTC/1024)
EVCTRL:
FREQCORR:  +0
ALARM0:  00-00-00 00:00:00
MASK0:  OFF
```


### void printZeroRegSCS(ZeroRegOptions &opts)
Prints out the ARM Cortex-M0+ system control space.

example output:
```text
--------------------------- SCS
CPUID:  REV=0x1 PARTNO=0xC60 ARCH=0xC VAR=0x0 IMPL=0x41
SysTick:  ENABLE TICKINT CLKSOURCE=CPU RELOAD=47999 TENMS=79999 SKEW
irq pri0:  PM SYSCTRL WDT RTC EIC NVMCTRL DMAC USB EVSYS SERCOM1 SERCOM2 SERCOM5 TCC0 TCC1 TCC2 TC3 TC4 TC5 TC6 TC7 ADC AC DAC PTC I2S
irq pri1:
irq pri2:
irq pri3:  SERCOM0 SERCOM3 SERCOM4
```


### void printZeroRegSERCOM(ZeroRegOptions &opts, Sercom* sercom, uint8_t idx)
Prints out the configuration registers for a `SERCOM` peripheral.

example output:
```text
--------------------------- SERCOM0 USART (internal clock)
CTRLA:  ENABLE SAMPR=0x1 SAMPA=0x0 cmode=ASYNC cpol=RISING dord=MSB form=USART rx=PAD3 tx=PAD2
CTRLB:  chsize=8bit TXEN RXEN
BAUD:  0x8138
--------------------------- SERCOM3 I2C master
CTRLA:  ENABLE sdahold=DIS speed=SM<100kHz,FM<400kHz inactout=DIS
CTRLB:  ackact=ACK
BAUD:  BAUD=0xE8 BAUDLOW=0x0 HSBAUD=0x0 HSBAUDLOW=0x0
ADDR:  ADDR=0x0 LEN=0x0
--------------------------- SERCOM4 SPI master
CTRLA:  ENABLE miso=PAD0 mosi=PAD2 sck=PAD3 form=SPI cpha=LEADING cpol=LOW dord=MSB
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


### void printZeroRegTC(ZeroRegOptions &opts, Tc* tc, uint8_t idx)
Prints out the configuration registers for a `TC` peripheral.

example output:
```text
--------------------------- TC3 COUNT16
CTRLA:  ENABLE wavegen=NPWM prescaler=DIV1 prescsync=GCLK
CTRLB:  dir=UP
CTRLC:
EVCTRL:  evact=OFF
CC0:  2304
CC1:  0
```


### void printZeroRegTCC(ZeroRegOptions &opts, Tcc* tcc, uint8_t idx)
Prints out the configuration registers for a `TCC` peripheral.

example output:
```text
--------------------------- TCC2
CTRLA:  ENABLE resolution=NONE prescaler=DIV1 prescsync=GCLK
CTRLB:  dir=UP
FCTRLA:  SRC=0x0 BLANK=0x0 HALT=0x0 chsel=CC0 CAPTURE=0x0 BLANKVAL=0x0 FILTERVAL=0x0
FCTRLB:  SRC=0x0 BLANK=0x0 HALT=0x0 chsel=CC0 CAPTURE=0x0 BLANKVAL=0x0 FILTERVAL=0x0
WEXCTRL:  OTMX=0x0 DTLS=0 DTHS=0
DRVCTRL:  FILTERVAL0=0x0 FILTERVAL1=0x0
EVCTRL:  EVACT0=0x0 EVACT1=0x0 CNTSEL=0x0
PATT:  ........
WAVE:  WAVEGEN=0x2 RAMP=0x0 POL0=0 POL1=0 POL2=0 POL3=0
PER:  65535
CC0:  2304
CC1:  0
CC2:  0
CC3:  0
```


### void printZeroRegUSB(ZeroRegOptions &opts)
Prints out the configuration registers for the `USB` peripheral.

example output:
```text
--------------------------- USB
CTRLA:  ENABLE RUNSTDBY mode=DEVICE
QOSCTRL:  cqos=LOW dqos=LOW
CTRLB:  spdconf=FS LPMHDSK=0x0
DADD:  0x3D ADDEN
PADCAL:  TRANSP=0x1D TRANSN=0x5 TRIM=0x3
ENDPOINT0:
    BANK0:  eptype=CTRL-out ADDR=0x20000D2C size=64byte EXTREG=0x0
    BANK1:  eptype=CTRL-in ADDR=0x20000B6C size=64byte
ENDPOINT1:
    BANK0:  eptype=--disabled--
    BANK1:  eptype=INT-in ADDR=0x20000BAC size=64byte
ENDPOINT2:
    BANK0:  eptype=BULK-out ADDR=0x20001160 size=64byte EXTREG=0x0
    BANK1:  eptype=--disabled--
ENDPOINT3:
    BANK0:  eptype=--disabled--
    BANK1:  eptype=BULK-in ADDR=0x20000C2C size=64byte
ENDPOINT4:
    BANK0:  eptype=--disabled--
    BANK1:  eptype=--disabled--
ENDPOINT5:
    BANK0:  eptype=--disabled--
    BANK1:  eptype=--disabled--
ENDPOINT6:
    BANK0:  eptype=--disabled--
    BANK1:  eptype=--disabled--
ENDPOINT7:
    BANK0:  eptype=--disabled--
    BANK1:  eptype=--disabled--
```


### void printZeroRegWDT(ZeroRegOptions &opts)
Prints out the configuration registers for the `WDT` peripheral.


License
-------
This code is licensed under the MIT license.
See the `LICENSE` file for details.


