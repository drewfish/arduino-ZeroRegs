// vim: ts=4 sts=4 sw=4 et si ai
/*
ZeroRegs.cpp - Library for printing the low-level SAM D21 configuration registers
Created by Drew Folta, November 27, 2015.

The MIT License (MIT)

Copyright (c) 2015 Drew Folta

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#include "ZeroRegs.h"
#include <samd.h>


#define PRINTFLAG(x,y) do { if (x.bit.y) { ser.print(" " #y); } } while(0)
#define PRINTHEX(x) do { ser.print("0x"); ser.print(x, HEX); } while(0)
#define PRINTSCALE(x) (ser.print(1 << (x)))
#define PRINTPAD2(x) do { if (x < 10) { ser.print("0"); } ser.print(x, DEC); } while(0)
#define WRITE8(x,y) *((uint8_t*)&(x)) = uint8_t(y)
#define READFUSE(x,y) ((*((uint32_t *) x##_FUSES_##y##_ADDR) & x##_FUSES_##y##_Msk) >> x##_FUSES_##y##_Pos)
#define READ2FUSES(x,y,o) ( ((*((uint32_t *) x##_FUSES_##y##_0_ADDR) & x##_FUSES_##y##_0_Msk) >> x##_FUSES_##y##_0_Pos) | (((*((uint32_t *) x##_FUSES_##y##_1_ADDR) & x##_FUSES_##y##_1_Msk) >> x##_FUSES_##y##_1_Pos) << o))
#define READADDR32(x) (*((uint32_t*)(x)))


void printZeroRegAC(Stream &ser) {
    // FUTURE
}


void printZeroRegADC(Stream &ser) {
    // FUTURE
}


void printZeroRegDAC(Stream &ser) {
    // FUTURE
}


void printZeroRegDMAC(Stream &ser) {
    // FUTURE
}


void printZeroRegDSU(Stream &ser) {
    // FUTURE
}


void printZeroRegEIC_SENSE(Stream &ser, uint8_t sense) {
    switch (sense) {
        case 0x0: ser.print("none"); break;
        case 0x1: ser.print("RISE"); break;
        case 0x2: ser.print("FALL"); break;
        case 0x3: ser.print("BOTH"); break;
        case 0x4: ser.print("HIGH"); break;
        case 0x5: ser.print("LOW"); break;
    }
}
void printZeroRegEIC(Stream &ser) {
    ser.println("--------------------------- EIC");
    while (EIC->STATUS.bit.SYNCBUSY) {}
    if (! EIC->CTRL.bit.ENABLE) {
        ser.println("--disabled--");
        return;
    }

    ser.print("NMI: ");
    printZeroRegEIC_SENSE(ser, EIC->NMICTRL.bit.NMISENSE);
    if (EIC->NMICTRL.bit.NMIFILTEN) {
        ser.print(" FILTEN");
    }
    ser.println("");

    for (uint8_t extint = 0; extint < 18; extint++) {
        ser.print(extint);
        ser.print(": ");
        uint8_t cfg = extint / 8;
        uint8_t pos = (extint % 8) * 4;
        uint32_t entry = 0xF & (EIC->CONFIG[cfg].reg >> pos);
        printZeroRegEIC_SENSE(ser, 0x7 & entry);
        if (0x8 & entry) {
            ser.print(" FILTEN");
        }
        if (EIC->EVCTRL.vec.EXTINTEO & (1 << extint)) {
            ser.print(" EXTINTEO");
        }
        if (EIC->WAKEUP.vec.WAKEUPEN & (1 << extint)) {
            ser.print(" WAKEUP");
        }
        ser.println("");
    }

    // NMIFLAG
    // INTENCLR
    // INTENSET
    // INTFLAG
}


void printZeroRegEVSYS(Stream &ser) {
    ser.println("--------------------------- EVSYS");
    while (EVSYS->CTRL.bit.SWRST) {}

    ser.print("CTRL: ");
    PRINTFLAG(EVSYS->CTRL, GCLKREQ);
    ser.println("");

    for (uint8_t chid = 0; chid < 12; chid++) {
        // [23.8.2] To read from this register, first do an 8-bit write to the
        // CHANNEL.CHANNEL bit group specifying the channel configuration to be
        // read, and then read the Channel register (CHANNEL).
        WRITE8(EVSYS->CHANNEL.reg, chid);
        // FUTURE: better way to wait until write has synchronized
        delay(1);
        ser.print("CHANNEL ");
        PRINTHEX(chid);
        ser.print(":  ");
        if (EVSYS->CHANNEL.bit.EVGEN == 0) {
            ser.println("--disabled--");
            continue;
        }
        switch (EVSYS->CHANNEL.bit.PATH) {
            case 0x0: ser.print("SYNC"); break;
            case 0x1: ser.print("RESYNC"); break;
            case 0x2: ser.print("ASYNC"); break;
        }
        ser.print(" ");
        switch (EVSYS->CHANNEL.bit.EDGSEL) {
            case 0x0: ser.print("NONE"); break;
            case 0x1: ser.print("RISE"); break;
            case 0x2: ser.print("FALL"); break;
            case 0x3: ser.print("BOTH"); break;
        }
        ser.print(" ");
        switch (EVSYS->CHANNEL.bit.EVGEN) {
            case 0x00: ser.print("NONE"); break;
            case 0x01: ser.print("RTC:CMP0"); break;
            case 0x02: ser.print("RTC:CMP1"); break;
            case 0x03: ser.print("RTC:OVF"); break;
            case 0x04: ser.print("RTC:PER0"); break;
            case 0x05: ser.print("RTC:PER1"); break;
            case 0x06: ser.print("RTC:PER2"); break;
            case 0x07: ser.print("RTC:PER3"); break;
            case 0x08: ser.print("RTC:PER4"); break;
            case 0x09: ser.print("RTC:PER5"); break;
            case 0x0A: ser.print("RTC:PER6"); break;
            case 0x0B: ser.print("RTC:PER7"); break;
            case 0x0C: ser.print("EIC:0"); break;
            case 0x0D: ser.print("EIC:1"); break;
            case 0x0E: ser.print("EIC:2"); break;
            case 0x0F: ser.print("EIC:3"); break;
            case 0x10: ser.print("EIC:4"); break;
            case 0x11: ser.print("EIC:5"); break;
            case 0x12: ser.print("EIC:6"); break;
            case 0x13: ser.print("EIC:7"); break;
            case 0x14: ser.print("EIC:8"); break;
            case 0x15: ser.print("EIC:9"); break;
            case 0x16: ser.print("EIC:10"); break;
            case 0x17: ser.print("EIC:11"); break;
            case 0x18: ser.print("EIC:12"); break;
            case 0x19: ser.print("EIC:13"); break;
            case 0x1A: ser.print("EIC:14"); break;
            case 0x1B: ser.print("EIC:15"); break;
            case 0x1C: /* reserved */ break;
            case 0x1D: /* reserved */ break;
            case 0x1E: ser.print("DMAC:0"); break;
            case 0x1F: ser.print("DMAC:1"); break;
            case 0x20: ser.print("DMAC:2"); break;
            case 0x21: ser.print("DMAC:3"); break;
            case 0x22: ser.print("TCC0:OVF"); break;
            case 0x23: ser.print("TCC0:TRG"); break;
            case 0x24: ser.print("TCC0:CNT"); break;
            case 0x25: ser.print("TCC0:MC0"); break;
            case 0x26: ser.print("TCC0:MC1"); break;
            case 0x27: ser.print("TCC0:MC2"); break;
            case 0x28: ser.print("TCC0:MC3"); break;
            case 0x29: ser.print("TCC1:OVF"); break;
            case 0x2A: ser.print("TCC1:TRG"); break;
            case 0x2B: ser.print("TCC1:CNT"); break;
            case 0x2C: ser.print("TCC1:MC0"); break;
            case 0x2D: ser.print("TCC1:MC1"); break;
            case 0x2E: ser.print("TCC2:OVF"); break;
            case 0x2F: ser.print("TCC2:TRG"); break;
            case 0x30: ser.print("TCC2:CNT"); break;
            case 0x31: ser.print("TCC2:MC0"); break;
            case 0x32: ser.print("TCC2:MC1"); break;
            case 0x33: ser.print("TC3:OVF"); break;
            case 0x34: ser.print("TC3:MC0"); break;
            case 0x35: ser.print("TC3:MC1"); break;
            case 0x36: ser.print("TC4:OVF"); break;
            case 0x37: ser.print("TC4:MC0"); break;
            case 0x38: ser.print("TC4:MC1"); break;
            case 0x39: ser.print("TC5:OVF"); break;
            case 0x3A: ser.print("TC5:MC0"); break;
            case 0x3B: ser.print("TC5:MC1"); break;
            case 0x3C: ser.print("TC6:OVF"); break;
            case 0x3D: ser.print("TC6:MC0"); break;
            case 0x3E: ser.print("TC6:MC1"); break;
            case 0x3F: ser.print("TC7:OVF"); break;
            case 0x40: ser.print("TC7:MC0"); break;
            case 0x41: ser.print("TC7:MC1"); break;
            case 0x42: ser.print("ADC:RESRDY"); break;
            case 0x43: ser.print("ADC:WINMON"); break;
            case 0x44: ser.print("AC:COMP0"); break;
            case 0x45: ser.print("AC:COMP1"); break;
            case 0x46: ser.print("AC:WIN0"); break;
            case 0x47: ser.print("DAC:EMPTY"); break;
            case 0x48: ser.print("PTC:EOC"); break;
            case 0x49: ser.print("PTC:WCOMP"); break;
            case 0x4A: ser.print("AC1:COMP0"); break;
            case 0x4B: ser.print("AC1:COMP1"); break;
            case 0x4C: ser.print("AC1:WIN0"); break;
            /* 0x4D-0x7F reserved */
        }
        ser.println("");
    }

    for (uint8_t uid = 0; uid < 0x1F; uid++) {
        // [23.8.3] To read from this register, first do an 8-bit write to the
        // USER.USER bit group specifying the event user configuration to be
        // read, and then read USER.
        WRITE8(EVSYS->USER.reg, uid);
        // FUTURE: better way to wait until write has synchronized
        delay(1);
        ser.print("USER ");
        switch (uid) {
            case 0x00: ser.print("DMAC:0"); break;
            case 0x01: ser.print("DMAC:1"); break;
            case 0x02: ser.print("DMAC:2"); break;
            case 0x03: ser.print("DMAC:3"); break;
            case 0x04: ser.print("TCC0:EV0"); break;
            case 0x05: ser.print("TCC0:EV1"); break;
            case 0x06: ser.print("TCC0:MC0"); break;
            case 0x07: ser.print("TCC0:MC1"); break;
            case 0x08: ser.print("TCC0:MC2"); break;
            case 0x09: ser.print("TCC0:MC3"); break;
            case 0x0A: ser.print("TCC1:EV0"); break;
            case 0x0B: ser.print("TCC1:EV1"); break;
            case 0x0C: ser.print("TCC1:MC0"); break;
            case 0x0D: ser.print("TCC1:MC1"); break;
            case 0x0E: ser.print("TCC2:EV0"); break;
            case 0x0F: ser.print("TCC2:EV1"); break;
            case 0x10: ser.print("TCC2:MC0"); break;
            case 0x11: ser.print("TCC2:MC1"); break;
            case 0x12: ser.print("TC3"); break;
            case 0x13: ser.print("TC4"); break;
            case 0x14: ser.print("TC5"); break;
            case 0x15: ser.print("TC6"); break;
            case 0x16: ser.print("TC7"); break;
            case 0x17: ser.print("ADC:START"); break;
            case 0x18: ser.print("ADC:SYNC"); break;
            case 0x19: ser.print("AC:COMP0"); break;
            case 0x1A: ser.print("AC:COMP1"); break;
            case 0x1B: ser.print("DAC:START"); break;
            case 0x1C: ser.print("PTC:STCONV"); break;
            case 0x1D: ser.print("AC1:COMP0"); break;
            case 0x1E: ser.print("AC1:COMP1"); break;
            /* 0x1F reserved */
        }
        if (EVSYS->USER.bit.CHANNEL == 0) {
            ser.print(" --disabled--");
        } else {
            ser.print(" CHANNEL=");
            PRINTHEX(EVSYS->USER.bit.CHANNEL - 1);
        }
        ser.println("");
    }

    // CHSTATUS
    // INTENCLR
    // INTENSET
    // INTFLAG
}


static const char gclk_name_00[] = "DFLL48M";
static const char gclk_name_01[] = "FDPLL96M";
static const char gclk_name_02[] = "FDPLL96M_32K";
static const char gclk_name_03[] = "WDT";
static const char gclk_name_04[] = "RTC";
static const char gclk_name_05[] = "EIC";
static const char gclk_name_06[] = "USB";
static const char gclk_name_07[] = "EVSYS_0";
static const char gclk_name_08[] = "EVSYS_1";
static const char gclk_name_09[] = "EVSYS_2";
static const char gclk_name_10[] = "EVSYS_3";
static const char gclk_name_11[] = "EVSYS_4";
static const char gclk_name_12[] = "EVSYS_5";
static const char gclk_name_13[] = "EVSYS_6";
static const char gclk_name_14[] = "EVSYS_7";
static const char gclk_name_15[] = "EVSYS_8";
static const char gclk_name_16[] = "EVSYS_9";
static const char gclk_name_17[] = "EVSYS_10";
static const char gclk_name_18[] = "EVSYS_11";
static const char gclk_name_19[] = "SERCOMX_SLOW";
static const char gclk_name_20[] = "SERCOM0_CORE";
static const char gclk_name_21[] = "SERCOM1_CORE";
static const char gclk_name_22[] = "SERCOM2_CORE";
static const char gclk_name_23[] = "SERCOM3_CORE";
static const char gclk_name_24[] = "SERCOM4_CORE";
static const char gclk_name_25[] = "SERCOM5_CORE";
static const char gclk_name_26[] = "TCC0_TCC1";
static const char gclk_name_27[] = "TCC2_TC3";
static const char gclk_name_28[] = "TC4_TC5";
static const char gclk_name_29[] = "TC6_TC7";
static const char gclk_name_30[] = "ADC";
static const char gclk_name_31[] = "AC_DIG";
static const char gclk_name_32[] = "AC_ANA";
static const char gclk_name_33[] = "DAC";
static const char gclk_name_34[] = "PTC";
static const char gclk_name_35[] = "I2S_0";
static const char gclk_name_36[] = "I2S_1";
static const char* const gclk_names[] = {
    gclk_name_00, gclk_name_01, gclk_name_02, gclk_name_03, gclk_name_04, gclk_name_05, gclk_name_06, gclk_name_07, gclk_name_08, gclk_name_09,
    gclk_name_10, gclk_name_11, gclk_name_12, gclk_name_13, gclk_name_14, gclk_name_15, gclk_name_16, gclk_name_17, gclk_name_18, gclk_name_19,
    gclk_name_20, gclk_name_21, gclk_name_22, gclk_name_23, gclk_name_24, gclk_name_25, gclk_name_26, gclk_name_27, gclk_name_28, gclk_name_29,
    gclk_name_30, gclk_name_31, gclk_name_32, gclk_name_33, gclk_name_34, gclk_name_35, gclk_name_36,
};
void printZeroRegGCLK(Stream &ser) {
    ser.println("--------------------------- GCLK");
    while (GCLK->CTRL.bit.SWRST || GCLK->STATUS.bit.SYNCBUSY) {}

    for (uint8_t gclkid = 0; gclkid < 37; gclkid++) {
        ser.print("GCLK ");
        ser.print(gclk_names[gclkid]);
        ser.print(":");
        // [14.8.3] To read the CLKCTRL register, first do an 8-bit write to the
        // CLKCTRL.ID bit group with the ID of the generic clock whose configuration
        // is to be read, and then read the CLKCTRL register.
        WRITE8(GCLK->CLKCTRL.reg, gclkid);
        while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {}
        if (GCLK->CLKCTRL.bit.CLKEN) {
            ser.print(" GEN=");
            PRINTHEX(GCLK->CLKCTRL.bit.GEN);
            PRINTFLAG(GCLK->CLKCTRL, WRTLOCK);
        } else {
            ser.print(" --disabled-- ");
        }
        ser.println("");
    }

    for (uint8_t genid = 0; genid < 0x9; genid++) {
        ser.print("GEN ");
        PRINTHEX(genid);
        ser.print(":  ");
        // [14.8.4] To read the GENCTRL register, first do an 8-bit write to the
        // GENCTRL.ID bit group with the ID of the generic clock generator whose
        // configuration is to be read, and then read the GENCTRL register.
        WRITE8(GCLK->GENCTRL.reg, genid);
        while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {}
        if (GCLK->GENCTRL.bit.GENEN) {
            switch (GCLK->GENCTRL.bit.SRC) {
                case 0x0: ser.print("XOSC"); break;
                case 0x1: ser.print("GCLKIN"); break;
                case 0x2: ser.print("GCLKGEN1"); break;
                case 0x3: ser.print("OSCULP32K"); break;
                case 0x4: ser.print("OSC32K"); break;
                case 0x5: ser.print("XOSC32K"); break;
                case 0x6: ser.print("OSC8M"); break;
                case 0x7: ser.print("DFLL48M"); break;
                case 0x8: ser.print("FDFLL96M"); break;
            }
            // [14.8.5] To read the GENDIV register, first do an 8-bit write to the
            // GENDIV.ID bit group with the ID of the generic clock generator whose
            // configuration is to be read, and then read the GENDIV register.
            WRITE8(GCLK->GENDIV.reg, genid);
            while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {}
            ser.print("/");
            PRINTSCALE(GCLK->GENDIV.bit.DIV);
            PRINTFLAG(GCLK->GENCTRL, IDC);
            PRINTFLAG(GCLK->GENCTRL, OOV);
            PRINTFLAG(GCLK->GENCTRL, OE);
            PRINTFLAG(GCLK->GENCTRL, DIVSEL);
            PRINTFLAG(GCLK->GENCTRL, RUNSTDBY);
        } else {
            ser.print(" --disabled--");
        }
        ser.println("");
    }
}


void printZeroRegI2S(Stream &ser) {
    // FUTURE
}


void printZeroRegMTB(Stream &ser) {
    // FUTURE
}


void printZeroRegNVMCTRL(Stream &ser) {
    ser.println("--------------------------- NVMCTRL");

    // CTRLA

    ser.print("CTRLB: ");
    PRINTFLAG(NVMCTRL->CTRLB, MANW);
    PRINTFLAG(NVMCTRL->CTRLB, CACHEDIS);
    ser.print(" RWS=");
    ser.print(NVMCTRL->CTRLB.bit.RWS);
    ser.print(" SLEEPPRM=");
    switch (NVMCTRL->CTRLB.bit.SLEEPPRM) {
        case 0x0: ser.print("WAKEONACCESS"); break;
        case 0x1: ser.print("WAKEUPINSTANT"); break;
        case 0x2: break;
        case 0x3: ser.print("DISABLED"); break;
    }
    ser.print(" READMODE=");
    switch (NVMCTRL->CTRLB.bit.READMODE) {
        case 0x0: ser.print("NO_MISS_PENALTY"); break;
        case 0x1: ser.print("LOW_POWER"); break;
        case 0x2: ser.print("DETERMINISTIC"); break;
    }
    ser.println("");

    ser.print("PARAM:  NVMP=");
    ser.print(NVMCTRL->PARAM.bit.NVMP);
    ser.print(" PSZ=");
    ser.print(1 << (3 + NVMCTRL->PARAM.bit.PSZ));
    ser.print(" RWWEEPROM=");
    ser.print(NVMCTRL->PARAM.bit.RWWEEP);
    ser.println("");

    // INTENCLR
    // INTENSET
    // INTFLAG

    ser.print("STATUS: ");
    PRINTFLAG(NVMCTRL->STATUS, PRM);
    PRINTFLAG(NVMCTRL->STATUS, LOAD);
    PRINTFLAG(NVMCTRL->STATUS, PROGE);
    PRINTFLAG(NVMCTRL->STATUS, LOCKE);
    PRINTFLAG(NVMCTRL->STATUS, NVME);
    PRINTFLAG(NVMCTRL->STATUS, SB);
    ser.println("");

    // ADDR

    ser.print("LOCK:  ");
    ser.println(NVMCTRL->LOCK.reg, BIN);

    // [9.3.1] User Row
    ser.print("user row: ");
    ser.print(" BOOTPROT=");
    switch (READFUSE(NVMCTRL, BOOTPROT)) {
        case 0x7: ser.print("0"); break;
        case 0x6: ser.print("512"); break;
        case 0x5: ser.print("1k"); break;
        case 0x4: ser.print("2k"); break;
        case 0x3: ser.print("4k"); break;
        case 0x2: ser.print("8k"); break;
        case 0x1: ser.print("16k"); break;
        case 0x0: ser.print("32k"); break;
    }
    ser.print(" EEPROM_SIZE=");
    switch (READFUSE(NVMCTRL, EEPROM_SIZE)) {
        case 0x7: ser.print("0"); break;
        case 0x6: ser.print("256"); break;
        case 0x5: ser.print("512"); break;
        case 0x4: ser.print("1k"); break;
        case 0x3: ser.print("2k"); break;
        case 0x2: ser.print("4k"); break;
        case 0x1: ser.print("8k"); break;
        case 0x0: ser.print("16k"); break;
    }
    ser.print(" REGION_LOCKS=");
    ser.print(READFUSE(NVMCTRL, REGION_LOCKS), BIN);
    ser.println("");

    // [9.3.2] Software Calibration Area
    // FUTURE

    // [9.3.3] Serial Number
    // 0x0080A00C 0x0080A040 0x0080A044 0x0080A048
    ser.print("serial # ");
    PRINTHEX(READADDR32(0x0080A00C));
    ser.print(" ");
    PRINTHEX(READADDR32(0x0080A040));
    ser.print(" ");
    PRINTHEX(READADDR32(0x0080A044));
    ser.print(" ");
    PRINTHEX(READADDR32(0x0080A048));
    ser.println("");
}


void printZeroRegPAC(Stream &ser) {
    ser.println("--------------------------- PAC");

    ser.print("PAC0: ");
    if (bitRead(PAC0->WPSET.reg, 1)) ser.print(" PM");
    if (bitRead(PAC0->WPSET.reg, 2)) ser.print(" SYSCTRL");
    if (bitRead(PAC0->WPSET.reg, 3)) ser.print(" GCLK");
    if (bitRead(PAC0->WPSET.reg, 4)) ser.print(" WDT");
    if (bitRead(PAC0->WPSET.reg, 5)) ser.print(" RTC");
    if (bitRead(PAC0->WPSET.reg, 6)) ser.print(" EIC");
    ser.println("");

    ser.print("PAC1: ");
    if (bitRead(PAC1->WPSET.reg, 1)) ser.print(" DSU");
    if (bitRead(PAC1->WPSET.reg, 2)) ser.print(" NVMCTRL");
    if (bitRead(PAC1->WPSET.reg, 3)) ser.print(" PORT");
    if (bitRead(PAC1->WPSET.reg, 4)) ser.print(" DMAC");
    if (bitRead(PAC1->WPSET.reg, 5)) ser.print(" USB");
    if (bitRead(PAC1->WPSET.reg, 6)) ser.print(" MTB");
    ser.println("");

    ser.print("PAC2: ");
    if (bitRead(PAC2->WPSET.reg,  1)) ser.print(" EVSYS");
    if (bitRead(PAC2->WPSET.reg,  2)) ser.print(" SERCOM0");
    if (bitRead(PAC2->WPSET.reg,  3)) ser.print(" SERCOM1");
    if (bitRead(PAC2->WPSET.reg,  4)) ser.print(" SERCOM2");
    if (bitRead(PAC2->WPSET.reg,  5)) ser.print(" SERCOM3");
    if (bitRead(PAC2->WPSET.reg,  6)) ser.print(" SERCOM4");
    if (bitRead(PAC2->WPSET.reg,  7)) ser.print(" SERCOM5");
    if (bitRead(PAC2->WPSET.reg,  8)) ser.print(" TCC0");
    if (bitRead(PAC2->WPSET.reg,  9)) ser.print(" TCC1");
    if (bitRead(PAC2->WPSET.reg, 10)) ser.print(" TCC2");
    if (bitRead(PAC2->WPSET.reg, 11)) ser.print(" TC3");
    if (bitRead(PAC2->WPSET.reg, 12)) ser.print(" TC4");
    if (bitRead(PAC2->WPSET.reg, 13)) ser.print(" TC5");
    if (bitRead(PAC2->WPSET.reg, 14)) ser.print(" TC6");
    if (bitRead(PAC2->WPSET.reg, 15)) ser.print(" TC7");
    if (bitRead(PAC2->WPSET.reg, 16)) ser.print(" ADC");
    if (bitRead(PAC2->WPSET.reg, 17)) ser.print(" AC");
    if (bitRead(PAC2->WPSET.reg, 18)) ser.print(" DAC");
    if (bitRead(PAC2->WPSET.reg, 19)) ser.print(" PTC");
    if (bitRead(PAC2->WPSET.reg, 20)) ser.print(" I2S");
    if (bitRead(PAC2->WPSET.reg, 21)) ser.print(" AC1");
    ser.println("");
}


void printZeroRegPM(Stream &ser) {
    ser.println("--------------------------- PM");

    ser.print("SLEEP:  IDLE=");
    switch(PM->SLEEP.bit.IDLE) {
        case 0x0: ser.print("CPU"); break;
        case 0x1: ser.print("CPU+AHB"); break;
        case 0x2: ser.print("CPU+AHB+APB"); break;
    }
    ser.println("");

    ser.print("EXTCTRL: ");
    PRINTFLAG(PM->EXTCTRL, SETDIS);
    ser.println("");

    ser.print("CPUSEL:  CPUDIV=");
    PRINTSCALE(PM->CPUSEL.bit.CPUDIV);
    ser.println("");

    ser.print("APBASEL:  APBADIV=");
    PRINTSCALE(PM->APBASEL.bit.APBADIV);
    ser.println("");

    ser.print("APBBSEL:  APBBDIV=");
    PRINTSCALE(PM->APBBSEL.bit.APBBDIV);
    ser.println("");

    ser.print("APBCSEL:  APBCDIV=");
    PRINTSCALE(PM->APBCSEL.bit.APBCDIV);
    ser.println("");

    ser.print("AHBMASK: ");
    PRINTFLAG(PM->AHBMASK, HPB0_);
    PRINTFLAG(PM->AHBMASK, HPB1_);
    PRINTFLAG(PM->AHBMASK, HPB2_);
    PRINTFLAG(PM->AHBMASK, DSU_);
    PRINTFLAG(PM->AHBMASK, NVMCTRL_);
    PRINTFLAG(PM->AHBMASK, DMAC_);
    PRINTFLAG(PM->AHBMASK, USB_);
    ser.println("");

    ser.print("APBAMASK: ");
    PRINTFLAG(PM->APBAMASK, PAC0_);
    PRINTFLAG(PM->APBAMASK, PM_);
    PRINTFLAG(PM->APBAMASK, SYSCTRL_);
    PRINTFLAG(PM->APBAMASK, GCLK_);
    PRINTFLAG(PM->APBAMASK, WDT_);
    PRINTFLAG(PM->APBAMASK, RTC_);
    PRINTFLAG(PM->APBAMASK, EIC_);
    ser.println("");

    ser.print("APBBMASK: ");
    PRINTFLAG(PM->APBBMASK, PAC1_);
    PRINTFLAG(PM->APBBMASK, DSU_);
    PRINTFLAG(PM->APBBMASK, NVMCTRL_);
    PRINTFLAG(PM->APBBMASK, PORT_);
    PRINTFLAG(PM->APBBMASK, DMAC_);
    PRINTFLAG(PM->APBBMASK, USB_);
    ser.println("");

    ser.print("APBCMASK: ");
    PRINTFLAG(PM->APBCMASK, PAC2_);
    PRINTFLAG(PM->APBCMASK, EVSYS_);
    PRINTFLAG(PM->APBCMASK, SERCOM0_);
    PRINTFLAG(PM->APBCMASK, SERCOM1_);
    PRINTFLAG(PM->APBCMASK, SERCOM2_);
    PRINTFLAG(PM->APBCMASK, SERCOM3_);
    PRINTFLAG(PM->APBCMASK, SERCOM4_);
    PRINTFLAG(PM->APBCMASK, SERCOM5_);
    PRINTFLAG(PM->APBCMASK, TCC0_);
    PRINTFLAG(PM->APBCMASK, TCC1_);
    PRINTFLAG(PM->APBCMASK, TCC2_);
    PRINTFLAG(PM->APBCMASK, TC3_);
    PRINTFLAG(PM->APBCMASK, TC4_);
    PRINTFLAG(PM->APBCMASK, TC5_);
    PRINTFLAG(PM->APBCMASK, TC6_);
    PRINTFLAG(PM->APBCMASK, TC7_);
    PRINTFLAG(PM->APBCMASK, ADC_);
    PRINTFLAG(PM->APBCMASK, AC_);
    PRINTFLAG(PM->APBCMASK, DAC_);
    PRINTFLAG(PM->APBCMASK, PTC_);
    PRINTFLAG(PM->APBCMASK, I2S_);
    PRINTFLAG(PM->APBCMASK, AC1_);
    PRINTFLAG(PM->APBCMASK, LINCTRL_);
    ser.println("");

    // INTENCLR
    // INTENSET
    // INTFLAG

    ser.print("RCAUSE: ");
    PRINTFLAG(PM->RCAUSE, POR);
    PRINTFLAG(PM->RCAUSE, BOD12);
    PRINTFLAG(PM->RCAUSE, BOD33);
    PRINTFLAG(PM->RCAUSE, EXT);
    if (PM->RCAUSE.reg & PM_RCAUSE_WDT) ser.print(" WDT");
    PRINTFLAG(PM->RCAUSE, SYST);
    ser.println("");
}


struct Port_Pin {
    const char *name;
    uint8_t     grp;
    uint8_t     idx;
    const char *pmux[8];
};
const Port_Pin port_pins[] = {
    // This is a combination of the arduino pin description found at the top of
    // packages/arduino/hardware/samd/1.6.2/variants/arduino_zero/variant.cpp
    // and table 6-1 ("PORT Function Multiplexing") in the SAM-D21 datasheet.
    // The peripheral name approach is:
    //  - {peripheral}:{signal}
    //  - peripheral is skipped if obvious
    //      - FUTURE: add distinction between ADC and AC
    //  - multiple options separated by slash ("/")
    // zero name    group  index A          B                   C               D               E           F           G           H
    { "D0/RX",          0, 11, { "EIC:11",  "AIN19/X3",         "SERCOM0:3",    "SERCOM2:3",    "TCC1:1",   "TCC0:3",   "I2S:FS0",  "GCLK:5" } },
    { "D1/TX",          0, 10, { "EIC:10",  "AIN18/X2",         "SERCOM0:2",    "SERCOM2:2",    "TCC1:0",   "TCC0:2",   "I2S:SCK0", "GCLK:4" } },
    { "D2",             0, 14, { "EIC:14",  NULL,               "SERCOM2:2",    "SERCOM4:2",    "TC3:0",    "TCC0:4",   NULL,       "GCLK:0" } },
    { "D3",             0,  9, { "EIC:9",   "AIN17/X1",         "SERCOM0:1",    "SERCOM2:1",    "TCC0:1",   "TCC1:3",   "I2S:MCK0", NULL } },
    { "D4",             0,  8, { "EIC:NMI", "AIN16/X0",         "SERCOM0:0",    "SERCOM2:0",    "TCC0:0",   "TCC1:2",   "I2S:SD1",  NULL } },
    { "D5",             0, 15, { "EIC:15",  NULL,               "SERCOM2:3",    "SERCOM4:3",    "TC3:1",    "TCC0:5",   NULL,       "GCLK:1" } },
    { "D6",             0, 20, { "EIC:4",   "X8",               "SERCOM5:2",    "SERCOM3:2",    "TC7:0",    "TCC0:6",   "I2S:SCK0", "GCLK:4" } },
    { "D7",             0, 21, { "EIC:5",   "X9",               "SERCOM5:3",    "SERCOM3:3",    "TC7:1",    "TCC0:7",   "I2S:FS0",  "GCLK:5" } },
    { "D8",             0,  6, { "EIC:6",   "AIN6/AIN2/Y4",     NULL,           "SERCOM0:2",    "TCC1:0",   NULL,       NULL,       NULL } },
    { "D9",             0,  7, { "EIC:7",   "AIN7/AIN3/Y5",     NULL,           "SERCOM0:3",    "TCC1:1",   NULL,       "I2S:SD0",  NULL } },
    { "D10",            0, 18, { "EIC:2",   "X6",               "SERCOM1:2",    "SERCOM3:2",    "TC3:0",    "TCC0:2",   NULL,       "AC:0" } },
    { "D11",            0, 16, { "EIC:0",   "X4",               "SERCOM1:0",    "SERCOM3:0",    "TCC2:0",   "TCC0:6",   NULL,       "GCLK:2" } },
    { "D12",            0, 19, { "EIC:3",   "X7",               "SERCOM1:3",    "SERCOM3:3",    "TC3:1",    "TCC0:3",   "I2S:SD0",  "AC:1" } },
    { "D13",            0, 17, { "EIC:1",   "X5",               "SERCOM1:1",    "SERCOM3:1",    "TCC1:1",   "TCC0:7",   NULL,       "GCLK:3" } },
    { "A0",             0,  2, { "EIC:2",   "AIN0/Y0/VOUT",     NULL,           NULL,           NULL,       NULL,       NULL,       NULL } },
    { "A1",             1,  8, { "EIC:8",   "AIN2/Y14",         NULL,           "SERCOM4:0",    "TC4:0",    NULL,       NULL,       NULL } },
    { "A2",             1,  9, { "EIC:9",   "AIN3/Y15",         NULL,           "SERCOM4:1",    "TC4:1",    NULL,       NULL,       NULL } },
    { "A3",             0,  4, { "EIC:4",   "ADC:VREFB/AIN4/AIN0/Y2", NULL,     "SERCOM0:0",    "TCC0:0",   NULL,       NULL,       NULL } },
    { "A4",             0,  5, { "EIC:5",   "AIN5/AIN1/Y3",     NULL,           "SERCOM0:1",    "TCC0:1",   NULL,       NULL,       NULL } },
    { "A5",             1,  2, { "EIC:2",   "AIN10/Y8",         NULL,           "SERCOM5:0",    "TC6:0",    NULL,       NULL,       NULL } },
    { "SDA",            0, 22, { "EIC:6",   "X10",              "SERCOM3:0",    "SERCOM5:0",    "TC4:0",    "TCC0:4",   NULL,       "GCLK:6" } },
    { "SCL",            0, 23, { "EID:7",   "X11",              "SERCOM3:1",    "SERCOM5:1",    "TC4:1",    "TCC0:5",   "USB:SOF",  "GCLK:7" } },
    { "SPI_MISO",       0, 12, { "EIC:12",  NULL,               "SERCOM2:0",    "SERCOM4:0",    "TCC2:0",   "TCC0:6",   NULL,       "AC:0" } },
    { "SPI_MOSI",       1, 10, { "EIC:10",  NULL,               NULL,           "SERCOM4:2",    "TC5:0",    "TCC0:4",   "I2S:MCK1", "GCLK:4" } },
    { "SPI_SCK",        1, 11, { "EIC:11",  NULL,               NULL,           "SERCOM4:3",    "TC5:1",    "TCC0:5",   "I2S:SCK1", "GCLK:5" } },
    { "LED_RX",         1,  3, { "EIC:3",   "AIN11/Y9",         NULL,           "SERCOM5:1",    "TC6:1",    NULL,       NULL,       NULL } },
    { "LED_TX",         0, 27, { "EIC:15",  NULL,               NULL,           NULL,           NULL,       NULL,       NULL,       "GCLK:0" } },
    { "USB_HOST_EN",    0, 28, { "EIC:8",   NULL,               NULL,           NULL,           NULL,       NULL,       NULL,       "GCLK:0" } },
    { "USB_DN",         0, 24, { "EIC:12",  NULL,               "SERCOM3:2",    "SERCOM5:2",    "TC5:0",    "TCC1:2",   "USB:DM",   NULL } },
    { "USB_DP",         0, 25, { "EIC:13",  NULL,               "SERCOM3:3",    "SERCOM5:3",    "TC5:1",    "TCC1:3",   "USB:DP",   NULL } },
    { "EDBG_TX",        1, 22, { "EIC:6",   NULL,               NULL,           "SERCOM5:2",    "TC7:0",    NULL,       NULL,       "GCLK:0" } },
    { "EDBG_RX",        1, 23, { "EIC:7",   NULL,               NULL,           "SERCOM5:3",    "TC7:1",    NULL,       NULL,       "GCLK:1" } },
    { "EDBG_SDA",       0, 22, { "EIC:6",   "X10",              "SERCOM3:0",    "SERCOM5:0",    "TC4:0",    "TCC0:4",   NULL,       "GCLK:6" } },
    { "EDBG_SCL",       0, 23, { "EIC:7",   "X11",              "SERCOM3:1",    "SERCOM5:1",    "TC4:1",    "TCC0:5",   "USB:SOF",  "GCLK:7" } },
    { "EDBG_MISO",      0, 19, { "EIC:3",   "X7",               "SERCOM1:3",    "SERCOM3:3",    "TC3:1",    "TCC0:3",   "I2S:SD0",  "AC:1" } },
    { "EDBG_MOSI",      0, 16, { "EIC:0",   "X4",               "SERCOM1:0",    "SERCOM3:0",    "TCC2:0",   "TCC0:6",   NULL,       "GCLK:2" } },
    { "EDBG_SS",        0, 18, { "EIC:2",   "X6",               "SERCOM1:2",    "SERCOM3:2",    "TC3:0",    "TCC0:2",   NULL,       "AC:0" } },
    { "EDBG_SCK",       0, 17, { "EIC:1",   "X5",               "SERCOM1:1",    "SERCOM3:1",    "TCC1:1",   "TCC0:7",   NULL,       "GCLK:3" } },
    { "EDBG_GPIO0",     0, 13, { "EIC:13",  NULL,               "SERCOM2:1",    "SERCOM4:1",    "TCC2:1",   "TCC0:7",   NULL,       "AC:7" } },
    { "EDBG_GPIO1",     0, 21, { "EIC:5",   "X9",               "SERCOM5:3",    "SERCOM3:3",    "TC7:1",    "TCC0:7",   "I2S:FS0",  "GCLK:5" } },
    { "EDBG_GPIO2",     0,  6, { "EIC:6",   "AIN6/AIN2/Y4",     NULL,           "SERCOM0:2",    "TCC1:0",   NULL,       NULL,       NULL } },
    { "EDBG_GPIO3",     0,  7, { "EIC:7",   "AIN7/AIN3/Y5",     NULL,           "SERCOM0:3",    "TCC1:1",   NULL,       "I2S:SD0",  NULL } },
    { "AREF",           0,  3, { "EIC:3",   "ADC:VREFA/DAC:VREFA/AIN1/Y1", NULL, NULL,          NULL,       NULL,       NULL,       NULL } },
};
const uint8_t port_pin_count = 43;
void printZeroRegPORT(Stream &ser) {
    ser.println("--------------------------- PORT");
    for (uint8_t p = 0; p < port_pin_count; p++) {
        const Port_Pin &pin = port_pins[p];
        ser.print(pin.grp ? "PB" : "PA");     // FUTURE: fix this if/when there's a port C
        PRINTPAD2(pin.idx);
        ser.print(" ");
        ser.print(pin.name);
        ser.print(":  ");
        bool out = bitRead(PORT->Group[pin.grp].DIR.reg, pin.idx);
        ser.print(out ? "OUT" : "IN");
        if (PORT->Group[pin.grp].PINCFG[pin.idx].bit.PMUXEN) {
            uint8_t pmux = 0xF & PORT->Group[pin.grp].PMUX[pin.idx / 2].reg >> (4 * (pin.idx % 2));
            ser.print(" PMUX=");
            if (pin.pmux[pmux]) {
                ser.print(pin.pmux[pmux]);
            } else {
                // shouldn't get here so we'll print some debugging
                PRINTHEX(pmux);
            }
        } else {
            if (out) {
                PRINTFLAG(PORT->Group[pin.grp].PINCFG[pin.idx], DRVSTR);
            } else {
                PRINTFLAG(PORT->Group[pin.grp].PINCFG[pin.idx], INEN);
                if (PORT->Group[pin.grp].PINCFG[pin.idx].bit.PULLEN) {
                    ser.print(out ? " PULLUP" : " PULLDOWN");
                }
            }
        }
        ser.println("");
    }
}


void printZeroRegRTC_MODE0(Stream &ser, RtcMode0 &mode) {
    ser.print("CTRL:  PRESCALER=");
    PRINTSCALE(mode.CTRL.bit.PRESCALER);
    PRINTFLAG(mode.CTRL, MATCHCLR);
    ser.println("");

    ser.print("READREQ: ");
    PRINTFLAG(mode.READREQ, RCONT);
    ser.println("");

    ser.print("EVCTRL: ");
    PRINTFLAG(mode.EVCTRL, PEREO0);
    PRINTFLAG(mode.EVCTRL, PEREO1);
    PRINTFLAG(mode.EVCTRL, PEREO2);
    PRINTFLAG(mode.EVCTRL, PEREO3);
    PRINTFLAG(mode.EVCTRL, PEREO4);
    PRINTFLAG(mode.EVCTRL, PEREO5);
    PRINTFLAG(mode.EVCTRL, PEREO6);
    PRINTFLAG(mode.EVCTRL, PEREO7);
    PRINTFLAG(mode.EVCTRL, CMPEO0);
    PRINTFLAG(mode.EVCTRL, OVFEO);
    ser.println("");

    // INTENCLR
    // INTENSET
    // INTFLAG
    // DBGCTRL

    ser.print("FREQCORR:  ");
    PRINTHEX(mode.FREQCORR.reg);
    ser.println("");

    ser.print("COMP0:  ");
    PRINTHEX(mode.COMP[0].reg);
    ser.println("");
}


void printZeroRegRTC_MODE1(Stream &ser, RtcMode1 &mode) {
    ser.print("CTRL:  PRESCALER=");
    PRINTSCALE(mode.CTRL.bit.PRESCALER);
    ser.println("");

    ser.print("READREQ: ");
    PRINTFLAG(mode.READREQ, RCONT);
    ser.println("");

    ser.print("EVCTRL: ");
    PRINTFLAG(mode.EVCTRL, PEREO0);
    PRINTFLAG(mode.EVCTRL, PEREO1);
    PRINTFLAG(mode.EVCTRL, PEREO2);
    PRINTFLAG(mode.EVCTRL, PEREO3);
    PRINTFLAG(mode.EVCTRL, PEREO4);
    PRINTFLAG(mode.EVCTRL, PEREO5);
    PRINTFLAG(mode.EVCTRL, PEREO6);
    PRINTFLAG(mode.EVCTRL, PEREO7);
    PRINTFLAG(mode.EVCTRL, CMPEO0);
    PRINTFLAG(mode.EVCTRL, CMPEO1);
    PRINTFLAG(mode.EVCTRL, OVFEO);
    ser.println("");

    // INTENCLR
    // INTENSET
    // INTFLAG
    // DBGCTRL

    ser.print("FREQCORR:  ");
    PRINTHEX(mode.FREQCORR.reg);
    ser.println("");

    ser.print("PER:  PER=");
    PRINTHEX(mode.PER.bit.PER);
    ser.println("");

    ser.print("COMP0:  ");
    PRINTHEX(mode.COMP[0].reg);
    ser.println("");

    ser.print("COMP1:  ");
    PRINTHEX(mode.COMP[1].reg);
    ser.println("");
}


void printZeroRegRTC_MODE2(Stream &ser, RtcMode2 &mode) {
    ser.print("CTRL:  PRESCALER=");
    PRINTSCALE(mode.CTRL.bit.PRESCALER);
    PRINTFLAG(mode.CTRL, CLKREP);
    PRINTFLAG(mode.CTRL, MATCHCLR);
    ser.println("");

    ser.print("READREQ: ");
    PRINTFLAG(mode.READREQ, RCONT);
    ser.println("");

    ser.print("EVCTRL: ");
    PRINTFLAG(mode.EVCTRL, PEREO0);
    PRINTFLAG(mode.EVCTRL, PEREO1);
    PRINTFLAG(mode.EVCTRL, PEREO2);
    PRINTFLAG(mode.EVCTRL, PEREO3);
    PRINTFLAG(mode.EVCTRL, PEREO4);
    PRINTFLAG(mode.EVCTRL, PEREO5);
    PRINTFLAG(mode.EVCTRL, PEREO6);
    PRINTFLAG(mode.EVCTRL, PEREO7);
    PRINTFLAG(mode.EVCTRL, ALARMEO0);
    PRINTFLAG(mode.EVCTRL, OVFEO);
    ser.println("");

    // INTENCLR
    // INTENSET
    // INTFLAG
    // DBGCTRL

    ser.print("FREQCORR:  ");
    PRINTHEX(mode.FREQCORR.reg);
    ser.println("");

    ser.print("ALARM:  ");
    PRINTPAD2(mode.Mode2Alarm[0].ALARM.bit.YEAR);
    ser.print("-");
    PRINTPAD2(mode.Mode2Alarm[0].ALARM.bit.MONTH);
    ser.print("-");
    PRINTPAD2(mode.Mode2Alarm[0].ALARM.bit.DAY);
    ser.print(" ");
    PRINTPAD2(mode.Mode2Alarm[0].ALARM.bit.HOUR);
    ser.print(":");
    PRINTPAD2(mode.Mode2Alarm[0].ALARM.bit.MINUTE);
    ser.print(":");
    PRINTPAD2(mode.Mode2Alarm[0].ALARM.bit.SECOND);
    ser.println("");

    ser.print("MASK:  ");
    switch (mode.Mode2Alarm[0].MASK.bit.SEL) {
        case 0x0: ser.print("OFF"); break;
        case 0x1: ser.print("SS"); break;
        case 0x2: ser.print("MM:SS"); break;
        case 0x3: ser.print("HH:MM:SS"); break;
        case 0x4: ser.print("DD HH:MM:SS"); break;
        case 0x5: ser.print("MM-DD HH:MM:SS"); break;
        case 0x6: ser.print("YY-MM-DD HH:MM:SS"); break;
        /* 0x7 reserved */
    }
    ser.println("");
}


void printZeroRegRTC(Stream &ser) {
    ser.println("--------------------------- RTC");
    while (RTC->MODE0.CTRL.bit.SWRST || RTC->MODE0.STATUS.bit.SYNCBUSY) {}
    if (! RTC->MODE0.CTRL.bit.ENABLE) {
        ser.println("--disabled--");
        return;
    }
    ser.print("MODE:  ");
    ser.println(RTC->MODE0.CTRL.bit.MODE);
    switch (RTC->MODE0.CTRL.bit.MODE) {
        case 0x0: printZeroRegRTC_MODE0(ser, RTC->MODE0); break;
        case 0x1: printZeroRegRTC_MODE1(ser, RTC->MODE1); break;
        case 0x2: printZeroRegRTC_MODE2(ser, RTC->MODE2); break;
    }
}


void printZeroRegSBMATRIX(Stream &ser) {
    ser.println("--------------------------- SBMATRIX");
    for (uint8_t i = 0; i < 16; i++) {
        ser.print("PRS ");
        PRINTHEX(i);
        ser.print(":  ");
        ser.print(SBMATRIX->Prs[i].PRAS.reg, BIN);
        ser.print(" ");
        ser.print(SBMATRIX->Prs[i].PRBS.reg, BIN);
        ser.println("");
    }
    for (uint8_t i = 0; i < 16; i++) {
        ser.print("SFR ");
        PRINTHEX(i);
        ser.print(":  ");
        ser.print(SBMATRIX->SFR[i].reg, BIN);
        ser.print(" ");
        ser.print(SBMATRIX->SFR[i].reg, BIN);
        ser.println("");
    }
}


void printZeroRegSERCOM_I2CM(Stream &ser, SercomI2cm &i2cm) {
    ser.print("CTRLA: ");
    PRINTFLAG(i2cm.CTRLA, RUNSTDBY);
    PRINTFLAG(i2cm.CTRLA, PINOUT);
    ser.print(" SDAHOLD=");
    switch (i2cm.CTRLA.bit.SDAHOLD) {
        case 0x0: ser.print("DIS"); break;
        case 0x1: ser.print("75NS"); break;
        case 0x2: ser.print("450NS"); break;
        case 0x3: ser.print("600NS"); break;
    }
    PRINTFLAG(i2cm.CTRLA, MEXTTOEN);
    PRINTFLAG(i2cm.CTRLA, SEXTTOEN);
    ser.print(" SPEED=");
    switch (i2cm.CTRLA.bit.SPEED) {
        case 0x0: ser.print("SM<100kHz,FM<400kHz"); break;
        case 0x1: ser.print("FM+<1MHz"); break;
        case 0x2: ser.print("HS<3.4MHz"); break;
    }
    PRINTFLAG(i2cm.CTRLA, SCLSM);
    ser.print(" INACTOUT=");
    switch (i2cm.CTRLA.bit.INACTOUT) {
        case 0x0: ser.print("DIS"); break;
        case 0x1: ser.print("55NS"); break;
        case 0x2: ser.print("105NS"); break;
        case 0x3: ser.print("205NS"); break;
    }
    PRINTFLAG(i2cm.CTRLA, LOWTOUTEN);
    ser.println("");

    ser.print("CTRLB: ");
    PRINTFLAG(i2cm.CTRLB, QCEN);
    if (i2cm.CTRLB.bit.SMEN) {
        ser.print(" SMEN");
        ser.print(" ACKACT=");
        ser.print(i2cm.CTRLB.bit.ACKACT ? "ACK" : "NACK");
    }
    ser.println("");

    ser.print("BAUD:  ");
    PRINTHEX(i2cm.BAUD.reg);  // FUTURE
    ser.println("");

    // INTENCLR
    // INTENSET
    // INTFLAG
    // STATUS
    // ADDR
    // DATA
    // DBGCTRL
}
void printZeroRegSERCOM_I2CS(Stream &ser, SercomI2cs &i2cs) {
    ser.print("CTRLA: ");
    PRINTFLAG(i2cs.CTRLA, RUNSTDBY);
    PRINTFLAG(i2cs.CTRLA, PINOUT);
    switch (i2cs.CTRLA.bit.SDAHOLD) {
        case 0x0: ser.print("DIS"); break;
        case 0x1: ser.print("75NS"); break;
        case 0x2: ser.print("450NS"); break;
        case 0x3: ser.print("600NS"); break;
    }
    PRINTFLAG(i2cs.CTRLA, SEXTTOEN);
    ser.print(" SPEED=");
    switch (i2cs.CTRLA.bit.SPEED) {
        case 0x0: ser.print("SM<100kHz,FM<400kHz"); break;
        case 0x1: ser.print("FM+<1MHz"); break;
        case 0x2: ser.print("HS<3.4MHz"); break;
    }
    PRINTFLAG(i2cs.CTRLA, SCLSM);
    PRINTFLAG(i2cs.CTRLA, LOWTOUTEN);
    ser.println("");

    ser.print("CTRLB: ");
    if (i2cs.CTRLB.bit.SMEN) {
        ser.print(" SMEN");
        ser.print(" ACKACT=");
        ser.print(i2cs.CTRLB.bit.ACKACT ? "ACK" : "NACK");
    }
    PRINTFLAG(i2cs.CTRLB, GCMD);
    PRINTFLAG(i2cs.CTRLB, AACKEN);
    ser.print(" AMODE=");
    switch (i2cs.CTRLB.bit.AMODE) {
        case 0x0: ser.print("MASK"); break;
        case 0x1: ser.print("2ADDRS"); break;
        case 0x2: ser.print("RANGE"); break;
    }
    ser.println("");

    // INTENCLR
    // INTENSET
    // INTFLAG
    // STATUS

    ser.print("ADDR: ");
    PRINTFLAG(i2cs.ADDR, GENCEN);
    ser.print(" ADDR=");
    PRINTHEX(i2cs.ADDR.bit.ADDR);
    PRINTFLAG(i2cs.ADDR, TENBITEN);
    ser.print(" ADDRMASK=");
    PRINTHEX(i2cs.ADDR.bit.ADDRMASK);

    // DATA
}
void printZeroRegSERCOM_SPI(Stream &ser, SercomSpi &spi, bool master) {
    ser.print("CTRLA: ");
    PRINTFLAG(spi.CTRLA, RUNSTDBY);
    PRINTFLAG(spi.CTRLA, IBON);
    ser.print(master ? " MISO=" : " MOSI=");
    switch (spi.CTRLA.bit.DIPO) {
        case 0x0: ser.print("PAD0"); break;
        case 0x1: ser.print("PAD1"); break;
        case 0x2: ser.print("PAD2"); break;
        case 0x3: ser.print("PAD3"); break;
    }
    ser.print(master ? " MOSI=" : " MISO=");
    switch (spi.CTRLA.bit.DOPO) {
        case 0x0:
            ser.print("PAD0 SCK=PAD1");
            if (!master) ser.print(" SS=PAD2");
            break;
        case 0x1:
            ser.print("PAD2 SCK=PAD3");
            if (!master) ser.print(" SS=PAD1");
            break;
        case 0x2:
            ser.print("PAD3 SCK=PAD1");
            if (!master) ser.print(" SS=PAD2");
            break;
        case 0x3:
            ser.print("PAD0 SCK=PAD3");
            if (!master) ser.print(" SS=PAD1");
            break;
    }
    ser.print(" FORM=");
    switch (spi.CTRLA.bit.FORM) {
        case 0x0: ser.print("SPI"); break;
        case 0x1: /* reserved */ break;
        case 0x2: ser.print("SPI_ADDR"); break;
        /* 0x3-0xF reserved */
    }
    ser.print(" CPHA="); ser.print(spi.CTRLA.bit.CPHA ? "CHANGE" : "SAMPLE");
    ser.print(" CPOL="); ser.print(spi.CTRLA.bit.CPOL ? "FALLING" : "RISING");
    ser.print(" DORD="); ser.print(spi.CTRLA.bit.CPOL ? "LSB" : "MSB");
    ser.println("");

    ser.print("CTRLB: ");
    ser.print(" CHSIZE=");
    switch (spi.CTRLB.bit.CHSIZE) {
        case 0x0: ser.print("8bit"); break;
        case 0x1: ser.print("9bit"); break;
        /* 0x2-0xF reserved */
    }
    PRINTFLAG(spi.CTRLB, PLOADEN);
    PRINTFLAG(spi.CTRLB, SSDE);
    PRINTFLAG(spi.CTRLB, MSSEN);
    switch (spi.CTRLB.bit.AMODE) {
        case 0x0: ser.print("MASK"); break;
        case 0x1: ser.print("2ADDRS"); break;
        case 0x2: ser.print("RANGE"); break;
    }
    PRINTFLAG(spi.CTRLB, RXEN);
    ser.println("");

    ser.print("BAUD:  ");
    PRINTHEX(spi.BAUD.reg);   // FUTURE
    ser.println("");

    // INTENCLR
    // INTENSET
    // INTFLAG
    // STATUS
    // SYNCBUSY

    if (spi.CTRLA.bit.FORM == 0x2) {
        ser.print("ADDR: ");
        ser.print(" ADDR=");
        PRINTHEX(spi.ADDR.bit.ADDR);
        ser.print(" ADDRMASK=");
        PRINTHEX(spi.ADDR.bit.ADDRMASK);
        ser.println("");
    }

    // DATA
}
void printZeroRegSERCOM_USART(Stream &ser, SercomUsart &usart) {
    ser.print("CTRLA: ");
    PRINTFLAG(usart.CTRLA, RUNSTDBY);
    PRINTFLAG(usart.CTRLA, IBON);
    ser.print(" CMODE="); ser.print(usart.CTRLA.bit.CMODE ? "SYNC" : "ASYNC");
    ser.print(" CPOL="); ser.print(usart.CTRLA.bit.CPOL ? "FALLING" : "RISING");
    ser.print(" DORD="); ser.print(usart.CTRLA.bit.CPOL ? "LSB" : "MSB");
    ser.print(" SAMPR=");
    PRINTHEX(usart.CTRLA.bit.SAMPR);  // FUTURE
    ser.print(" SAMPA=");
    PRINTHEX(usart.CTRLA.bit.SAMPA);  // FUTURE
    ser.print(" FORM=");
    switch(usart.CTRLA.bit.FORM) {
        case 0x0: ser.print("USART"); break;
        case 0x1: ser.print("USART,PARITY"); break;
        case 0x2: /* reserved */ break;
        case 0x3: /* reserved */ break;
        case 0x4: ser.print("AUTOBAUD"); break;
        case 0x5: ser.print("AUTOBAUD,PARITY"); break;
        /* 0x6-0xF reserved */
    }
    switch (usart.CTRLA.bit.RXPO) {
        case 0x0: ser.print(" RX=PAD0"); break;
        case 0x1: ser.print(" RX=PAD1"); break;
        case 0x2: ser.print(" RX=PAD2"); break;
        case 0x3: ser.print(" RX=PAD3"); break;
    }
    switch (usart.CTRLA.bit.TXPO) {
        case 0x0:
            ser.print(" TX=PAD0");
            if (usart.CTRLA.bit.MODE == 0) ser.print(" XCK=PAD1");
            break;
        case 0x1:
            ser.print(" TX=PAD2");
            if (usart.CTRLA.bit.MODE == 0) ser.print(" XCK=PAD3");
            break;
        case 0x2:
            ser.print(" TX=PAD0");
            ser.print(" RTS=PAD2");
            ser.print(" CTS=PAD3");
            break;
    }
    ser.println("");

    ser.print("CTRLB:  CHSIZE=");
    switch (usart.CTRLB.bit.CHSIZE) {
        case 0x0: ser.print("8bit"); break;
        case 0x1: ser.print("9bit"); break;
        case 0x2: /* reserved */ break;
        case 0x3: /* reserved */ break;
        case 0x4: /* reserved */ break;
        case 0x5: ser.print("5bit"); break;
        case 0x6: ser.print("6bit"); break;
        case 0x7: ser.print("7bit"); break;
    }
    PRINTFLAG(usart.CTRLB, SBMODE);
    PRINTFLAG(usart.CTRLB, COLDEN);
    PRINTFLAG(usart.CTRLB, SFDE);
    PRINTFLAG(usart.CTRLB, ENC);
    PRINTFLAG(usart.CTRLB, PMODE);
    PRINTFLAG(usart.CTRLB, TXEN);
    PRINTFLAG(usart.CTRLB, RXEN);
    ser.println("");

    ser.print("BAUD:  ");
    PRINTHEX(usart.BAUD.reg); // FUTURE
    ser.println("");

    if (usart.CTRLB.bit.ENC) {
        ser.print("RXPL:  ");
        PRINTHEX(usart.RXPL.reg);
        ser.println("");
    }

    // INTENCLR
    // INTENSET
    // INTFLAG
    // STATUS
    // DATA
    // DBGCTRL
}
void printZeroRegSERCOM(Stream &ser, Sercom* sercom, uint8_t idx) {
    ser.print("--------------------------- SERCOM");
    ser.print(idx);
    while (sercom->I2CM.CTRLA.bit.SWRST || sercom->I2CM.SYNCBUSY.reg) {}
    if (! sercom->I2CM.CTRLA.bit.ENABLE) {
        ser.println("\n--disabled--");
        return;
    }
    switch (sercom->I2CM.CTRLA.bit.MODE) {
        case 0x0: ser.println(" USART"); printZeroRegSERCOM_USART(ser, sercom->USART); break;
        case 0x1: ser.println(" USART"); printZeroRegSERCOM_USART(ser, sercom->USART); break;
        case 0x2: ser.println(" SPIS");  printZeroRegSERCOM_SPI(ser, sercom->SPI, false); break;
        case 0x3: ser.println(" SPIM");  printZeroRegSERCOM_SPI(ser, sercom->SPI, true); break;
        case 0x4: ser.println(" I2CS");  printZeroRegSERCOM_I2CS(ser, sercom->I2CS); break;
        case 0x5: ser.println(" I2CM");  printZeroRegSERCOM_I2CM(ser, sercom->I2CM); break;
    }
}


void printZeroRegSYSCTRL(Stream &ser) {
    ser.println("--------------------------- SYSCTRL");

    // INTENCLR
    // INTENSET
    // INTFLAG

    ser.print("PCLKSR: ");
    PRINTFLAG(SYSCTRL->PCLKSR, XOSCRDY);
    PRINTFLAG(SYSCTRL->PCLKSR, XOSC32KRDY);
    PRINTFLAG(SYSCTRL->PCLKSR, OSC32KRDY);
    PRINTFLAG(SYSCTRL->PCLKSR, OSC8MRDY);
    PRINTFLAG(SYSCTRL->PCLKSR, DFLLRDY);
    PRINTFLAG(SYSCTRL->PCLKSR, DFLLOOB);
    PRINTFLAG(SYSCTRL->PCLKSR, DFLLLCKF);
    PRINTFLAG(SYSCTRL->PCLKSR, DFLLLCKC);
    PRINTFLAG(SYSCTRL->PCLKSR, DFLLRCS);
    PRINTFLAG(SYSCTRL->PCLKSR, BOD33RDY);
    PRINTFLAG(SYSCTRL->PCLKSR, BOD33DET);
    PRINTFLAG(SYSCTRL->PCLKSR, B33SRDY);
    PRINTFLAG(SYSCTRL->PCLKSR, DPLLLCKR);
    PRINTFLAG(SYSCTRL->PCLKSR, DPLLLCKF);
    PRINTFLAG(SYSCTRL->PCLKSR, DPLLLTO);
    ser.println("");

    ser.print("XOSC: ");
    if (SYSCTRL->XOSC.bit.ENABLE) {
        PRINTFLAG(SYSCTRL->XOSC, XTALEN);
        PRINTFLAG(SYSCTRL->XOSC, RUNSTDBY);
        PRINTFLAG(SYSCTRL->XOSC, ONDEMAND);
        PRINTFLAG(SYSCTRL->XOSC, AMPGC);
        ser.print(" GAIN=");
        switch (SYSCTRL->XOSC.bit.GAIN) {
            case 0x0: ser.print("2MHz"); break;
            case 0x1: ser.print("4MHz"); break;
            case 0x2: ser.print("8MHz"); break;
            case 0x3: ser.print("16MHz"); break;
            case 0x4: ser.print("30MHz"); break;
        }
        ser.print(" STARTUP=");
        switch (SYSCTRL->XOSC.bit.STARTUP) {
            case 0x0: ser.print("31us"); break;
            case 0x1: ser.print("61us"); break;
            case 0x2: ser.print("122us"); break;
            case 0x3: ser.print("244us"); break;
            case 0x4: ser.print("488us"); break;
            case 0x5: ser.print("977us"); break;
            case 0x6: ser.print("1953us"); break;
            case 0x7: ser.print("3906us"); break;
            case 0x8: ser.print("7813us"); break;
            case 0x9: ser.print("15625us"); break;
            case 0xA: ser.print("31250us"); break;
            case 0xB: ser.print("62500us"); break;
            case 0xC: ser.print("125ms"); break;
            case 0xD: ser.print("250ms"); break;
            case 0xE: ser.print("500ms"); break;
            case 0xF: ser.print("1s"); break;
        }
    } else {
        ser.print(" --disabled--");
    }
    ser.println("");

    ser.print("XOSC32K: ");
    if (SYSCTRL->XOSC32K.bit.ENABLE) {
        PRINTFLAG(SYSCTRL->XOSC32K, XTALEN);
        PRINTFLAG(SYSCTRL->XOSC32K, EN32K);
        PRINTFLAG(SYSCTRL->XOSC32K, EN1K);
        PRINTFLAG(SYSCTRL->XOSC32K, AAMPEN);
        PRINTFLAG(SYSCTRL->XOSC32K, RUNSTDBY);
        PRINTFLAG(SYSCTRL->XOSC32K, ONDEMAND);
        PRINTFLAG(SYSCTRL->XOSC32K, WRTLOCK);
        ser.print(" STARTUP=");
        switch (SYSCTRL->XOSC32K.bit.STARTUP) {
            case 0x0: ser.print("122us"); break;
            case 0x1: ser.print("1068us"); break;
            case 0x2: ser.print("62592us"); break;
            case 0x3: ser.print("125092us"); break;
            case 0x4: ser.print("500092us"); break;
            case 0x5: ser.print("1000092us"); break;
            case 0x6: ser.print("2000092us"); break;
            case 0x7: ser.print("4000092us"); break;
        }
    } else {
        ser.print(" --disabled--");
    }
    ser.println("");

    ser.print("OSC32K: ");
    if (SYSCTRL->OSC32K.bit.ENABLE) {
        PRINTFLAG(SYSCTRL->OSC32K, EN32K);
        PRINTFLAG(SYSCTRL->OSC32K, EN1K);
        PRINTFLAG(SYSCTRL->OSC32K, RUNSTDBY);
        PRINTFLAG(SYSCTRL->OSC32K, ONDEMAND);
        PRINTFLAG(SYSCTRL->OSC32K, WRTLOCK);
        ser.print(" STARTUP=");
        switch (SYSCTRL->OSC32K.bit.STARTUP) {
            case 0x0: ser.print("92us"); break;
            case 0x1: ser.print("122us"); break;
            case 0x2: ser.print("183us"); break;
            case 0x3: ser.print("305us"); break;
            case 0x4: ser.print("549us"); break;
            case 0x5: ser.print("1038us"); break;
            case 0x6: ser.print("2014us"); break;
            case 0x7: ser.print("3967us"); break;
        }
        ser.print(" CALIB=");
        PRINTHEX(SYSCTRL->OSC32K.bit.CALIB);
    } else {
        ser.print(" --disabled--");
    }
    ser.println("");

    ser.print("OSCULP32K: ");
    PRINTFLAG(SYSCTRL->OSCULP32K, WRTLOCK);
    ser.print(" CALIB=");
    PRINTHEX(SYSCTRL->OSCULP32K.bit.CALIB);
    ser.println("");

    ser.print("OSC8M: ");
    if (SYSCTRL->OSC8M.bit.ENABLE) {
        PRINTFLAG(SYSCTRL->OSC8M, RUNSTDBY);
        PRINTFLAG(SYSCTRL->OSC8M, ONDEMAND);
        ser.print(" PRESC=");
        PRINTSCALE(SYSCTRL->OSC8M.bit.PRESC);
        ser.print(" CALIB=");
        PRINTHEX(SYSCTRL->OSC8M.bit.CALIB);
        ser.print(" FRANGE=");
        switch (SYSCTRL->OSC8M.bit.FRANGE) {
            case 0x0: ser.print("4-6MHz"); break;
            case 0x1: ser.print("6-8MHz"); break;
            case 0x2: ser.print("8-11MHz"); break;
            case 0x3: ser.print("11-15MHz"); break;
        }
    } else {
        ser.print(" --disabled--");
    }
    ser.println("");

    ser.print("DFLL: ");
    if (SYSCTRL->DFLLCTRL.bit.ENABLE) {
        PRINTFLAG(SYSCTRL->DFLLCTRL, MODE);
        PRINTFLAG(SYSCTRL->DFLLCTRL, STABLE);
        PRINTFLAG(SYSCTRL->DFLLCTRL, LLAW);
        PRINTFLAG(SYSCTRL->DFLLCTRL, USBCRM);
        PRINTFLAG(SYSCTRL->DFLLCTRL, RUNSTDBY);
        PRINTFLAG(SYSCTRL->DFLLCTRL, ONDEMAND);
        PRINTFLAG(SYSCTRL->DFLLCTRL, CCDIS);
        PRINTFLAG(SYSCTRL->DFLLCTRL, QLDIS);
        PRINTFLAG(SYSCTRL->DFLLCTRL, BPLCKC);
        PRINTFLAG(SYSCTRL->DFLLCTRL, WAITLOCK);
        // DFLLVAL
        // DFLLMUL
        // DFLLSYNC
    } else {
        ser.print(" --disabled--");
    }
    ser.println("");

    ser.print("BOD33: ");
    if (SYSCTRL->BOD33.bit.ENABLE) {
        PRINTFLAG(SYSCTRL->BOD33, HYST);
        PRINTFLAG(SYSCTRL->BOD33, RUNSTDBY);
        PRINTFLAG(SYSCTRL->BOD33, MODE);
        PRINTFLAG(SYSCTRL->BOD33, CEN);
        ser.print(" ACTION=");
        switch (SYSCTRL->BOD33.bit.ACTION) {
            case 0x0: ser.print("NONE"); break;
            case 0x1: ser.print("RESET"); break;
            case 0x2: ser.print("INT"); break;
        }
        ser.print(" PSEL=");
        PRINTSCALE(SYSCTRL->BOD33.bit.PSEL);
        ser.print(" LEVEL=");
        PRINTHEX(SYSCTRL->BOD33.bit.LEVEL);
    } else {
        ser.print(" --disabled--");
    }
    ser.println("");

    ser.print("VREG: ");
    PRINTFLAG(SYSCTRL->VREG, RUNSTDBY);
    PRINTFLAG(SYSCTRL->VREG, FORCELDO);
    ser.println("");

    ser.print("VREF: ");
    PRINTFLAG(SYSCTRL->VREF, TSEN);
    PRINTFLAG(SYSCTRL->VREF, BGOUTEN);
    ser.print(" CALIB=");
    PRINTHEX(SYSCTRL->VREF.bit.CALIB);
    ser.println("");

    ser.print("DPLL: ");
    if (SYSCTRL->DPLLCTRLA.bit.ENABLE) {
        PRINTFLAG(SYSCTRL->DPLLCTRLA, RUNSTDBY);
        PRINTFLAG(SYSCTRL->DPLLCTRLA, ONDEMAND);
        // DPLLRATIO
        // DPLLCTRLB
        // DPLLSTATUS
    } else {
        ser.print(" --disabled--");
    }
    ser.println("");
}


void printZeroRegTC(Stream &ser, Tc* tc, uint8_t idx) {
    // FUTURE
}


void printZeroRegTCC(Stream &ser, Tcc* tcc, uint8_t idx) {
    // FUTURE
}


void printZeroRegUSB(Stream &ser) {
    // FUTURE
}


void printZeroRegWDT(Stream &ser) {
    ser.println("--------------------------- WDT");
    while (WDT->STATUS.bit.SYNCBUSY) {}
    if (! WDT->CTRL.bit.ENABLE) {
        ser.println("--disabled--");
        return;
    }

    ser.print("CTRL: ");
    PRINTFLAG(WDT->CTRL, WEN);
    PRINTFLAG(WDT->CTRL, ALWAYSON);
    ser.println("");

    ser.print("CONFIG:  PER=");
    PRINTSCALE(3 + WDT->CONFIG.bit.PER);
    ser.print(" WINDOW=");
    PRINTSCALE(3 + WDT->CONFIG.bit.WINDOW);
    ser.println("");

    ser.print("EWCTRL:  EWOFFSET=");
    PRINTSCALE(3 + WDT->EWCTRL.bit.EWOFFSET);
    ser.println("");

    // INTENCLR
    // INTENSET
    // INTFLAG

    ser.print("NVM user row: ");
    ser.print(" ENABLE=");
    ser.print(READFUSE(WDT, ENABLE));
    ser.print(" ALWAYSON=");
    ser.print(READFUSE(WDT, ALWAYSON));
    ser.print(" PER=");
    PRINTSCALE(3 + READFUSE(WDT, PER));
    ser.print(" WINDOW=");
    PRINTSCALE(3 + READ2FUSES(WDT, WINDOW, 1));
    ser.print(" EWOFFSET=");
    PRINTSCALE(3 + READFUSE(WDT, EWOFFSET));
    ser.print(" WEN=");
    ser.print(READFUSE(WDT, WEN));
    ser.println("");
}


void printZeroRegs(Stream &ser) {
    printZeroRegAC(ser);
    printZeroRegADC(ser);
    printZeroRegDAC(ser);
    printZeroRegDMAC(ser);
    printZeroRegDSU(ser);
    printZeroRegEIC(ser);
    printZeroRegEVSYS(ser);
    printZeroRegGCLK(ser);
    printZeroRegI2S(ser);
    printZeroRegMTB(ser);
    printZeroRegNVMCTRL(ser);
    printZeroRegPAC(ser);
    printZeroRegPM(ser);
    printZeroRegPORT(ser);
    printZeroRegRTC(ser);
    //printZeroRegSBMATRIX(ser);    // questionable value
    printZeroRegSERCOM(ser, SERCOM0, 0);
    printZeroRegSERCOM(ser, SERCOM1, 1);
    printZeroRegSERCOM(ser, SERCOM2, 2);
    printZeroRegSERCOM(ser, SERCOM3, 3);
    printZeroRegSERCOM(ser, SERCOM4, 4);
    printZeroRegSERCOM(ser, SERCOM5, 5);
    printZeroRegSYSCTRL(ser);
    printZeroRegTCC(ser, TCC0, 0);
    printZeroRegTCC(ser, TCC1, 1);
    printZeroRegTCC(ser, TCC2, 2);
    printZeroRegTC(ser, TC3, 3);
    printZeroRegTC(ser, TC4, 4);
    printZeroRegTC(ser, TC5, 5);
    printZeroRegUSB(ser);
    printZeroRegWDT(ser);
}


