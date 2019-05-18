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


#define PRINTFLAG(x,y) do { if (x.bit.y) { opts.ser.print(" " #y); } } while(0)
#define PRINTHEX(x) do { opts.ser.print("0x"); opts.ser.print(x, HEX); } while(0)
#define PRINTSCALE(x) (opts.ser.print(1 << (x)))
#define PRINTPAD2(x) do { if (x < 10) { opts.ser.print("0"); } opts.ser.print(x, DEC); } while(0)
#define WRITE8(x,y) *((uint8_t*)&(x)) = uint8_t(y)
#define READFUSE(x,y) ((*((uint32_t *) x##_FUSES_##y##_ADDR) & x##_FUSES_##y##_Msk) >> x##_FUSES_##y##_Pos)
#define READ2FUSES(x,y,o) ( ((*((uint32_t *) x##_FUSES_##y##_0_ADDR) & x##_FUSES_##y##_0_Msk) >> x##_FUSES_##y##_0_Pos) | (((*((uint32_t *) x##_FUSES_##y##_1_ADDR) & x##_FUSES_##y##_1_Msk) >> x##_FUSES_##y##_1_Pos) << o))
#define READADDR32(x) (*((uint32_t*)(x)))


void printZeroRegAC(ZeroRegOptions &opts) {
    // FUTURE
}


void printZeroRegADC(ZeroRegOptions &opts) {
    // FUTURE
}


void printZeroRegDAC(ZeroRegOptions &opts) {
    // FUTURE
}


void printZeroRegDMAC(ZeroRegOptions &opts) {
    // FUTURE
}


void printZeroRegDSU(ZeroRegOptions &opts) {
    // FUTURE
}


void printZeroRegEIC_SENSE(ZeroRegOptions &opts, uint8_t sense) {
    switch (sense) {
        case 0x0: opts.ser.print("none"); break;
        case 0x1: opts.ser.print("RISE"); break;
        case 0x2: opts.ser.print("FALL"); break;
        case 0x3: opts.ser.print("BOTH"); break;
        case 0x4: opts.ser.print("HIGH"); break;
        case 0x5: opts.ser.print("LOW"); break;
    }
}
void printZeroRegEIC(ZeroRegOptions &opts) {
    while (EIC->STATUS.bit.SYNCBUSY) {}
    if (! EIC->CTRL.bit.ENABLE) {
        if (opts.showDisabled) {
            opts.ser.println("--------------------------- EIC\n--disabled--");
        }
        return;
    }
    opts.ser.println("--------------------------- EIC");

    if (EIC->NMICTRL.bit.NMISENSE) {
        opts.ser.print("NMI:  ");
        printZeroRegEIC_SENSE(opts, EIC->NMICTRL.bit.NMISENSE);
        if (EIC->NMICTRL.bit.NMIFILTEN) {
            opts.ser.print(" FILTEN");
        }
        opts.ser.println("");
    } else {
        if (opts.showDisabled) {
            opts.ser.println("NMI:  none");
        }
    }

    for (uint8_t extint = 0; extint < 18; extint++) {
        uint8_t cfg = extint / 8;
        uint8_t pos = (extint % 8) * 4;
        uint32_t entry = 0xF & (EIC->CONFIG[cfg].reg >> pos);
        if (!opts.showDisabled && (entry & 0x7) == 0) {
            continue;
        }
        opts.ser.print("EXTINT");
        PRINTPAD2(extint);
        opts.ser.print(":  ");
        printZeroRegEIC_SENSE(opts, 0x7 & entry);
        if (0x8 & entry) {
            opts.ser.print(" FILTEN");
        }
        if (EIC->EVCTRL.vec.EXTINTEO & (1 << extint)) {
            opts.ser.print(" EXTINTEO");
        }
        if (EIC->WAKEUP.vec.WAKEUPEN & (1 << extint)) {
            opts.ser.print(" WAKEUP");
        }
        opts.ser.println("");
    }

    // NMIFLAG
    // INTENCLR
    // INTENSET
    // INTFLAG
}


void printZeroRegEVSYS(ZeroRegOptions &opts) {
    opts.ser.println("--------------------------- EVSYS");
    while (EVSYS->CTRL.bit.SWRST) {}

    opts.ser.print("CTRL: ");
    PRINTFLAG(EVSYS->CTRL, GCLKREQ);
    opts.ser.println("");

    for (uint8_t chid = 0; chid < 12; chid++) {
        // [23.8.2] To read from this register, first do an 8-bit write to the
        // CHANNEL.CHANNEL bit group specifying the channel configuration to be
        // read, and then read the Channel register (CHANNEL).
        WRITE8(EVSYS->CHANNEL.reg, chid);
        // FUTURE: better way to wait until write has synchronized
        delay(1);
        if (EVSYS->CHANNEL.bit.EVGEN || opts.showDisabled) {
            opts.ser.print("CHANNEL");
            PRINTPAD2(chid);
            opts.ser.print(":  ");
        }
        if (! EVSYS->CHANNEL.bit.EVGEN) {
            if (opts.showDisabled) {
                opts.ser.println("--disabled--");
            }
            continue;
        }

        switch (EVSYS->CHANNEL.bit.PATH) {
            case 0x0: opts.ser.print("SYNC"); break;
            case 0x1: opts.ser.print("RESYNC"); break;
            case 0x2: opts.ser.print("ASYNC"); break;
        }
        opts.ser.print(" ");
        switch (EVSYS->CHANNEL.bit.EDGSEL) {
            case 0x0: opts.ser.print("NONE"); break;
            case 0x1: opts.ser.print("RISE"); break;
            case 0x2: opts.ser.print("FALL"); break;
            case 0x3: opts.ser.print("BOTH"); break;
        }
        opts.ser.print(" ");
        switch (EVSYS->CHANNEL.bit.EVGEN) {
            case 0x00: opts.ser.print("NONE"); break;
            case 0x01: opts.ser.print("RTC:CMP0"); break;
            case 0x02: opts.ser.print("RTC:CMP1"); break;
            case 0x03: opts.ser.print("RTC:OVF"); break;
            case 0x04: opts.ser.print("RTC:PER0"); break;
            case 0x05: opts.ser.print("RTC:PER1"); break;
            case 0x06: opts.ser.print("RTC:PER2"); break;
            case 0x07: opts.ser.print("RTC:PER3"); break;
            case 0x08: opts.ser.print("RTC:PER4"); break;
            case 0x09: opts.ser.print("RTC:PER5"); break;
            case 0x0A: opts.ser.print("RTC:PER6"); break;
            case 0x0B: opts.ser.print("RTC:PER7"); break;
            case 0x0C: opts.ser.print("EIC:0"); break;
            case 0x0D: opts.ser.print("EIC:1"); break;
            case 0x0E: opts.ser.print("EIC:2"); break;
            case 0x0F: opts.ser.print("EIC:3"); break;
            case 0x10: opts.ser.print("EIC:4"); break;
            case 0x11: opts.ser.print("EIC:5"); break;
            case 0x12: opts.ser.print("EIC:6"); break;
            case 0x13: opts.ser.print("EIC:7"); break;
            case 0x14: opts.ser.print("EIC:8"); break;
            case 0x15: opts.ser.print("EIC:9"); break;
            case 0x16: opts.ser.print("EIC:10"); break;
            case 0x17: opts.ser.print("EIC:11"); break;
            case 0x18: opts.ser.print("EIC:12"); break;
            case 0x19: opts.ser.print("EIC:13"); break;
            case 0x1A: opts.ser.print("EIC:14"); break;
            case 0x1B: opts.ser.print("EIC:15"); break;
            case 0x1C: /* reserved */ break;
            case 0x1D: /* reserved */ break;
            case 0x1E: opts.ser.print("DMAC:0"); break;
            case 0x1F: opts.ser.print("DMAC:1"); break;
            case 0x20: opts.ser.print("DMAC:2"); break;
            case 0x21: opts.ser.print("DMAC:3"); break;
            case 0x22: opts.ser.print("TCC0:OVF"); break;
            case 0x23: opts.ser.print("TCC0:TRG"); break;
            case 0x24: opts.ser.print("TCC0:CNT"); break;
            case 0x25: opts.ser.print("TCC0:MC0"); break;
            case 0x26: opts.ser.print("TCC0:MC1"); break;
            case 0x27: opts.ser.print("TCC0:MC2"); break;
            case 0x28: opts.ser.print("TCC0:MC3"); break;
            case 0x29: opts.ser.print("TCC1:OVF"); break;
            case 0x2A: opts.ser.print("TCC1:TRG"); break;
            case 0x2B: opts.ser.print("TCC1:CNT"); break;
            case 0x2C: opts.ser.print("TCC1:MC0"); break;
            case 0x2D: opts.ser.print("TCC1:MC1"); break;
            case 0x2E: opts.ser.print("TCC2:OVF"); break;
            case 0x2F: opts.ser.print("TCC2:TRG"); break;
            case 0x30: opts.ser.print("TCC2:CNT"); break;
            case 0x31: opts.ser.print("TCC2:MC0"); break;
            case 0x32: opts.ser.print("TCC2:MC1"); break;
            case 0x33: opts.ser.print("TC3:OVF"); break;
            case 0x34: opts.ser.print("TC3:MC0"); break;
            case 0x35: opts.ser.print("TC3:MC1"); break;
            case 0x36: opts.ser.print("TC4:OVF"); break;
            case 0x37: opts.ser.print("TC4:MC0"); break;
            case 0x38: opts.ser.print("TC4:MC1"); break;
            case 0x39: opts.ser.print("TC5:OVF"); break;
            case 0x3A: opts.ser.print("TC5:MC0"); break;
            case 0x3B: opts.ser.print("TC5:MC1"); break;
            case 0x3C: opts.ser.print("TC6:OVF"); break;
            case 0x3D: opts.ser.print("TC6:MC0"); break;
            case 0x3E: opts.ser.print("TC6:MC1"); break;
            case 0x3F: opts.ser.print("TC7:OVF"); break;
            case 0x40: opts.ser.print("TC7:MC0"); break;
            case 0x41: opts.ser.print("TC7:MC1"); break;
            case 0x42: opts.ser.print("ADC:RESRDY"); break;
            case 0x43: opts.ser.print("ADC:WINMON"); break;
            case 0x44: opts.ser.print("AC:COMP0"); break;
            case 0x45: opts.ser.print("AC:COMP1"); break;
            case 0x46: opts.ser.print("AC:WIN0"); break;
            case 0x47: opts.ser.print("DAC:EMPTY"); break;
            case 0x48: opts.ser.print("PTC:EOC"); break;
            case 0x49: opts.ser.print("PTC:WCOMP"); break;
            case 0x4A: opts.ser.print("AC1:COMP0"); break;
            case 0x4B: opts.ser.print("AC1:COMP1"); break;
            case 0x4C: opts.ser.print("AC1:WIN0"); break;
            /* 0x4D-0x7F reserved */
        }
        opts.ser.println("");
    }

    for (uint8_t uid = 0; uid < 0x1F; uid++) {
        // [23.8.3] To read from this register, first do an 8-bit write to the
        // USER.USER bit group specifying the event user configuration to be
        // read, and then read USER.
        WRITE8(EVSYS->USER.reg, uid);
        // FUTURE: better way to wait until write has synchronized
        delay(1);
        if (EVSYS->USER.bit.CHANNEL || opts.showDisabled) {
            opts.ser.print("USER ");
            switch (uid) {
                case 0x00: opts.ser.print("DMAC:0"); break;
                case 0x01: opts.ser.print("DMAC:1"); break;
                case 0x02: opts.ser.print("DMAC:2"); break;
                case 0x03: opts.ser.print("DMAC:3"); break;
                case 0x04: opts.ser.print("TCC0:EV0"); break;
                case 0x05: opts.ser.print("TCC0:EV1"); break;
                case 0x06: opts.ser.print("TCC0:MC0"); break;
                case 0x07: opts.ser.print("TCC0:MC1"); break;
                case 0x08: opts.ser.print("TCC0:MC2"); break;
                case 0x09: opts.ser.print("TCC0:MC3"); break;
                case 0x0A: opts.ser.print("TCC1:EV0"); break;
                case 0x0B: opts.ser.print("TCC1:EV1"); break;
                case 0x0C: opts.ser.print("TCC1:MC0"); break;
                case 0x0D: opts.ser.print("TCC1:MC1"); break;
                case 0x0E: opts.ser.print("TCC2:EV0"); break;
                case 0x0F: opts.ser.print("TCC2:EV1"); break;
                case 0x10: opts.ser.print("TCC2:MC0"); break;
                case 0x11: opts.ser.print("TCC2:MC1"); break;
                case 0x12: opts.ser.print("TC3"); break;
                case 0x13: opts.ser.print("TC4"); break;
                case 0x14: opts.ser.print("TC5"); break;
                case 0x15: opts.ser.print("TC6"); break;
                case 0x16: opts.ser.print("TC7"); break;
                case 0x17: opts.ser.print("ADC:START"); break;
                case 0x18: opts.ser.print("ADC:SYNC"); break;
                case 0x19: opts.ser.print("AC:COMP0"); break;
                case 0x1A: opts.ser.print("AC:COMP1"); break;
                case 0x1B: opts.ser.print("DAC:START"); break;
                case 0x1C: opts.ser.print("PTC:STCONV"); break;
                case 0x1D: opts.ser.print("AC1:COMP0"); break;
                case 0x1E: opts.ser.print("AC1:COMP1"); break;
                /* 0x1F reserved */
            }
        }
        if (EVSYS->USER.bit.CHANNEL == 0) {
            if (opts.showDisabled) {
                opts.ser.println(":  --disabled--");
            }
        } else {
            opts.ser.print(":  CHANNEL");
            PRINTPAD2(EVSYS->USER.bit.CHANNEL - 1);
            opts.ser.println("");
        }
    }

    // CHSTATUS
    // INTENCLR
    // INTENSET
    // INTFLAG
}


static const char gclk_name_00[] = "DFLL48M_REF";
static const char gclk_name_01[] = "DPLL";
static const char gclk_name_02[] = "DPLL_32K";
static const char gclk_name_03[] = "WDT";
static const char gclk_name_04[] = "RTC";
static const char gclk_name_05[] = "EIC";
static const char gclk_name_06[] = "USB";
static const char gclk_name_07[] = "EVSYS_CHANNEL_0";
static const char gclk_name_08[] = "EVSYS_CHANNEL_1";
static const char gclk_name_09[] = "EVSYS_CHANNEL_2";
static const char gclk_name_10[] = "EVSYS_CHANNEL_3";
static const char gclk_name_11[] = "EVSYS_CHANNEL_4";
static const char gclk_name_12[] = "EVSYS_CHANNEL_5";
static const char gclk_name_13[] = "EVSYS_CHANNEL_6";
static const char gclk_name_14[] = "EVSYS_CHANNEL_7";
static const char gclk_name_15[] = "EVSYS_CHANNEL_8";
static const char gclk_name_16[] = "EVSYS_CHANNEL_9";
static const char gclk_name_17[] = "EVSYS_CHANNEL_10";
static const char gclk_name_18[] = "EVSYS_CHANNEL_11";
static const char gclk_name_19[] = "SERCOMx_SLOW";
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
void printZeroRegGCLK(ZeroRegOptions &opts) {
    opts.ser.println("--------------------------- GCLK");
    while (GCLK->CTRL.bit.SWRST || GCLK->STATUS.bit.SYNCBUSY) {}

    opts.ser.println("GCLK_MAIN:  GEN00 (always)");
    for (uint8_t gclkid = 0; gclkid < 37; gclkid++) {
        // [14.8.3] To read the CLKCTRL register, first do an 8-bit write to the
        // CLKCTRL.ID bit group with the ID of the generic clock whose configuration
        // is to be read, and then read the CLKCTRL register.
        WRITE8(GCLK->CLKCTRL.reg, gclkid);
        while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {}

        if (GCLK->CLKCTRL.bit.CLKEN || opts.showDisabled) {
            opts.ser.print("GCLK_");
            opts.ser.print(gclk_names[gclkid]);
            opts.ser.print(": ");
        }
        if (GCLK->CLKCTRL.bit.CLKEN) {
            opts.ser.print(" GEN");
            PRINTPAD2(GCLK->CLKCTRL.bit.GEN);
            PRINTFLAG(GCLK->CLKCTRL, WRTLOCK);
            opts.ser.println("");
        } else {
            if (opts.showDisabled) {
                opts.ser.println(" --disabled--");
            }
        }
    }

    for (uint8_t genid = 0; genid < 0x9; genid++) {
        // [14.8.4] To read the GENCTRL register, first do an 8-bit write to the
        // GENCTRL.ID bit group with the ID of the generic clock generator whose
        // configuration is to be read, and then read the GENCTRL register.
        WRITE8(GCLK->GENCTRL.reg, genid);
        while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {}

        if (GCLK->GENCTRL.bit.GENEN || opts.showDisabled) {
            opts.ser.print("GEN");
            PRINTPAD2(genid);
            opts.ser.print(":  ");
        }
        if (GCLK->GENCTRL.bit.GENEN) {
            switch (GCLK->GENCTRL.bit.SRC) {
                case 0x0: opts.ser.print("XOSC"); break;
                case 0x1: opts.ser.print("GCLKIN"); break;
                case 0x2: opts.ser.print("GCLKGEN1"); break;
                case 0x3: opts.ser.print("OSCULP32K"); break;
                case 0x4: opts.ser.print("OSC32K"); break;
                case 0x5: opts.ser.print("XOSC32K"); break;
                case 0x6: opts.ser.print("OSC8M"); break;
                case 0x7: opts.ser.print("DFLL48M"); break;
                case 0x8: opts.ser.print("FDFLL96M"); break;
            }
            // [14.8.5] To read the GENDIV register, first do an 8-bit write to the
            // GENDIV.ID bit group with the ID of the generic clock generator whose
            // configuration is to be read, and then read the GENDIV register.
            WRITE8(GCLK->GENDIV.reg, genid);
            while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {}
            if (GCLK->GENCTRL.bit.DIVSEL) {
                opts.ser.print("/");
                PRINTSCALE(GCLK->GENDIV.bit.DIV + 1);
            } else {
                if (GCLK->GENDIV.bit.DIV > 1) {
                    opts.ser.print("/");
                    opts.ser.print(GCLK->GENDIV.bit.DIV);
                }
            }
            PRINTFLAG(GCLK->GENCTRL, IDC);
            PRINTFLAG(GCLK->GENCTRL, OOV);
            PRINTFLAG(GCLK->GENCTRL, OE);
            PRINTFLAG(GCLK->GENCTRL, RUNSTDBY);
            opts.ser.println("");
        } else {
            if (opts.showDisabled) {
                opts.ser.println("--disabled--");
            }
        }
    }
}


void printZeroRegI2S(ZeroRegOptions &opts) {
    // FUTURE
}


void printZeroRegMTB(ZeroRegOptions &opts) {
    // FUTURE
}


void printZeroRegNVMCTRL(ZeroRegOptions &opts) {
    opts.ser.println("--------------------------- NVMCTRL");

    // CTRLA

    opts.ser.print("CTRLB: ");
    PRINTFLAG(NVMCTRL->CTRLB, MANW);
    PRINTFLAG(NVMCTRL->CTRLB, CACHEDIS);
    opts.ser.print(" RWS=");
    opts.ser.print(NVMCTRL->CTRLB.bit.RWS);
    opts.ser.print(" SLEEPPRM=");
    switch (NVMCTRL->CTRLB.bit.SLEEPPRM) {
        case 0x0: opts.ser.print("WAKEONACCESS"); break;
        case 0x1: opts.ser.print("WAKEUPINSTANT"); break;
        case 0x2: break;
        case 0x3: opts.ser.print("DISABLED"); break;
    }
    opts.ser.print(" READMODE=");
    switch (NVMCTRL->CTRLB.bit.READMODE) {
        case 0x0: opts.ser.print("NO_MISS_PENALTY"); break;
        case 0x1: opts.ser.print("LOW_POWER"); break;
        case 0x2: opts.ser.print("DETERMINISTIC"); break;
    }
    opts.ser.println("");

    opts.ser.print("PARAM:  NVMP=");
    opts.ser.print(NVMCTRL->PARAM.bit.NVMP);
    opts.ser.print(" PSZ=");
    opts.ser.print(1 << (3 + NVMCTRL->PARAM.bit.PSZ));
    opts.ser.println("");

    // INTENCLR
    // INTENSET
    // INTFLAG

    opts.ser.print("STATUS: ");
    PRINTFLAG(NVMCTRL->STATUS, PRM);
    PRINTFLAG(NVMCTRL->STATUS, LOAD);
    PRINTFLAG(NVMCTRL->STATUS, PROGE);
    PRINTFLAG(NVMCTRL->STATUS, LOCKE);
    PRINTFLAG(NVMCTRL->STATUS, NVME);
    PRINTFLAG(NVMCTRL->STATUS, SB);
    opts.ser.println("");

    // ADDR

    opts.ser.print("LOCK:  ");
    opts.ser.println(NVMCTRL->LOCK.reg, BIN);

    // [9.3.1] User Row
    opts.ser.print("user row: ");
    opts.ser.print(" BOOTPROT=");
    switch (READFUSE(NVMCTRL, BOOTPROT)) {
        case 0x7: opts.ser.print("0"); break;
        case 0x6: opts.ser.print("512"); break;
        case 0x5: opts.ser.print("1k"); break;
        case 0x4: opts.ser.print("2k"); break;
        case 0x3: opts.ser.print("4k"); break;
        case 0x2: opts.ser.print("8k"); break;
        case 0x1: opts.ser.print("16k"); break;
        case 0x0: opts.ser.print("32k"); break;
    }
    opts.ser.print(" EEPROM_SIZE=");
    switch (READFUSE(NVMCTRL, EEPROM_SIZE)) {
        case 0x7: opts.ser.print("0"); break;
        case 0x6: opts.ser.print("256"); break;
        case 0x5: opts.ser.print("512"); break;
        case 0x4: opts.ser.print("1k"); break;
        case 0x3: opts.ser.print("2k"); break;
        case 0x2: opts.ser.print("4k"); break;
        case 0x1: opts.ser.print("8k"); break;
        case 0x0: opts.ser.print("16k"); break;
    }
    opts.ser.print(" REGION_LOCKS=");
    opts.ser.print(READFUSE(NVMCTRL, REGION_LOCKS), BIN);
    opts.ser.println("");

    // [9.3.2] Software Calibration Area
    // FUTURE

    // [9.3.3] Serial Number
    // 0x0080A00C 0x0080A040 0x0080A044 0x0080A048
    opts.ser.print("serial # ");
    PRINTHEX(READADDR32(0x0080A00C));
    opts.ser.print(" ");
    PRINTHEX(READADDR32(0x0080A040));
    opts.ser.print(" ");
    PRINTHEX(READADDR32(0x0080A044));
    opts.ser.print(" ");
    PRINTHEX(READADDR32(0x0080A048));
    opts.ser.println("");
}


void printZeroRegPAC(ZeroRegOptions &opts) {
    opts.ser.println("--------------------------- PAC");

    if (PAC0->WPSET.reg || opts.showDisabled) {
        opts.ser.print("PAC0: ");
        if (bitRead(PAC0->WPSET.reg, 1)) opts.ser.print(" PM");
        if (bitRead(PAC0->WPSET.reg, 2)) opts.ser.print(" SYSCTRL");
        if (bitRead(PAC0->WPSET.reg, 3)) opts.ser.print(" GCLK");
        if (bitRead(PAC0->WPSET.reg, 4)) opts.ser.print(" WDT");
        if (bitRead(PAC0->WPSET.reg, 5)) opts.ser.print(" RTC");
        if (bitRead(PAC0->WPSET.reg, 6)) opts.ser.print(" EIC");
        opts.ser.println("");
    }

    if (PAC1->WPSET.reg || opts.showDisabled) {
        opts.ser.print("PAC1: ");
        if (bitRead(PAC1->WPSET.reg, 1)) opts.ser.print(" DSU");
        if (bitRead(PAC1->WPSET.reg, 2)) opts.ser.print(" NVMCTRL");
        if (bitRead(PAC1->WPSET.reg, 3)) opts.ser.print(" PORT");
        if (bitRead(PAC1->WPSET.reg, 4)) opts.ser.print(" DMAC");
        if (bitRead(PAC1->WPSET.reg, 5)) opts.ser.print(" USB");
        if (bitRead(PAC1->WPSET.reg, 6)) opts.ser.print(" MTB");
        opts.ser.println("");
    }

    if (PAC2->WPSET.reg || opts.showDisabled) {
        opts.ser.print("PAC2: ");
        if (bitRead(PAC2->WPSET.reg,  1)) opts.ser.print(" EVSYS");
        if (bitRead(PAC2->WPSET.reg,  2)) opts.ser.print(" SERCOM0");
        if (bitRead(PAC2->WPSET.reg,  3)) opts.ser.print(" SERCOM1");
        if (bitRead(PAC2->WPSET.reg,  4)) opts.ser.print(" SERCOM2");
        if (bitRead(PAC2->WPSET.reg,  5)) opts.ser.print(" SERCOM3");
        if (bitRead(PAC2->WPSET.reg,  6)) opts.ser.print(" SERCOM4");
        if (bitRead(PAC2->WPSET.reg,  7)) opts.ser.print(" SERCOM5");
        if (bitRead(PAC2->WPSET.reg,  8)) opts.ser.print(" TCC0");
        if (bitRead(PAC2->WPSET.reg,  9)) opts.ser.print(" TCC1");
        if (bitRead(PAC2->WPSET.reg, 10)) opts.ser.print(" TCC2");
        if (bitRead(PAC2->WPSET.reg, 11)) opts.ser.print(" TC3");
        if (bitRead(PAC2->WPSET.reg, 12)) opts.ser.print(" TC4");
        if (bitRead(PAC2->WPSET.reg, 13)) opts.ser.print(" TC5");
        if (bitRead(PAC2->WPSET.reg, 14)) opts.ser.print(" TC6");
        if (bitRead(PAC2->WPSET.reg, 15)) opts.ser.print(" TC7");
        if (bitRead(PAC2->WPSET.reg, 16)) opts.ser.print(" ADC");
        if (bitRead(PAC2->WPSET.reg, 17)) opts.ser.print(" AC");
        if (bitRead(PAC2->WPSET.reg, 18)) opts.ser.print(" DAC");
        if (bitRead(PAC2->WPSET.reg, 19)) opts.ser.print(" PTC");
        if (bitRead(PAC2->WPSET.reg, 20)) opts.ser.print(" I2S");
        if (bitRead(PAC2->WPSET.reg, 21)) opts.ser.print(" AC1");
        if (bitRead(PAC2->WPSET.reg, 22)) opts.ser.print(" 22?");
        if (bitRead(PAC2->WPSET.reg, 23)) opts.ser.print(" 23?");
        if (bitRead(PAC2->WPSET.reg, 24)) opts.ser.print(" 24?");
        if (bitRead(PAC2->WPSET.reg, 25)) opts.ser.print(" 25?");
        if (bitRead(PAC2->WPSET.reg, 26)) opts.ser.print(" 26?");
        if (bitRead(PAC2->WPSET.reg, 27)) opts.ser.print(" 27?");
        if (bitRead(PAC2->WPSET.reg, 28)) opts.ser.print(" 28?");
        if (bitRead(PAC2->WPSET.reg, 29)) opts.ser.print(" 29?");
        if (bitRead(PAC2->WPSET.reg, 30)) opts.ser.print(" 30?");
        if (bitRead(PAC2->WPSET.reg, 31)) opts.ser.print(" 31?");
        opts.ser.println("");
    }
}


void printZeroRegPM(ZeroRegOptions &opts) {
    opts.ser.println("--------------------------- PM");

    // CTRL

    opts.ser.print("SLEEP:  IDLE=");
    switch(PM->SLEEP.bit.IDLE) {
        case 0x0: opts.ser.print("CPU"); break;
        case 0x1: opts.ser.print("CPU+AHB"); break;
        case 0x2: opts.ser.print("CPU+AHB+APB"); break;
    }
    opts.ser.println("");

    opts.ser.print("CPUSEL:  CPUDIV=");
    PRINTSCALE(PM->CPUSEL.bit.CPUDIV);
    opts.ser.println("");

    opts.ser.print("APBASEL:  APBADIV=");
    PRINTSCALE(PM->APBASEL.bit.APBADIV);
    opts.ser.println("");

    opts.ser.print("APBBSEL:  APBBDIV=");
    PRINTSCALE(PM->APBBSEL.bit.APBBDIV);
    opts.ser.println("");

    opts.ser.print("APBCSEL:  APBCDIV=");
    PRINTSCALE(PM->APBCSEL.bit.APBCDIV);
    opts.ser.println("");

    opts.ser.print("AHBMASK: ");
    if (PM->AHBMASK.bit.HPB0_)      { opts.ser.print(" CLK_HPBA_AHB"); }
    if (PM->AHBMASK.bit.HPB1_)      { opts.ser.print(" CLK_HPBB_AHB"); }
    if (PM->AHBMASK.bit.HPB2_)      { opts.ser.print(" CLK_HPBC_AHB"); }
    if (PM->AHBMASK.bit.DSU_)       { opts.ser.print(" CLK_DSU_AHB"); }
    if (PM->AHBMASK.bit.NVMCTRL_)   { opts.ser.print(" CLK_NVMCTRL_AHB"); }
    if (PM->AHBMASK.bit.DMAC_)      { opts.ser.print(" CLK_DMAC_AHB"); }
    if (PM->AHBMASK.bit.USB_)       { opts.ser.print(" CLK_USB_AHB"); }
    opts.ser.println("");

    opts.ser.print("APBAMASK: ");
    if (PM->APBAMASK.bit.PAC0_)     { opts.ser.print(" CLK_PAC0_APB"); }
    if (PM->APBAMASK.bit.PM_)       { opts.ser.print(" CLK_PM_APB"); }
    if (PM->APBAMASK.bit.SYSCTRL_)  { opts.ser.print(" CLK_SYSCTRL_APB"); }
    if (PM->APBAMASK.bit.GCLK_)     { opts.ser.print(" CLK_GCLK_APB"); }
    if (PM->APBAMASK.bit.WDT_)      { opts.ser.print(" CLK_WDT_APB"); }
    if (PM->APBAMASK.bit.RTC_)      { opts.ser.print(" CLK_RTC_APB"); }
    if (PM->APBAMASK.bit.EIC_)      { opts.ser.print(" CLK_EIC_APB"); }
    opts.ser.println("");

    opts.ser.print("APBBMASK: ");
    if (PM->APBBMASK.bit.PAC1_)     { opts.ser.print(" CLK_PAC1_APB"); }
    if (PM->APBBMASK.bit.DSU_)      { opts.ser.print(" CLK_DSU_APB"); }
    if (PM->APBBMASK.bit.NVMCTRL_)  { opts.ser.print(" CLK_NVMCTRL_APB"); }
    if (PM->APBBMASK.bit.PORT_)     { opts.ser.print(" CLK_PORT_APB"); }
    if (PM->APBBMASK.bit.DMAC_)     { opts.ser.print(" CLK_DMAC_APB"); }
    if (PM->APBBMASK.bit.USB_)      { opts.ser.print(" CLK_USB_APB"); }
    opts.ser.println("");

    opts.ser.print("APBCMASK: ");
    if (PM->APBCMASK.bit.PAC2_)     { opts.ser.print(" CLK_PAC2_APB"); }
    if (PM->APBCMASK.bit.EVSYS_)    { opts.ser.print(" CLK_EVSYS_APB"); }
    if (PM->APBCMASK.bit.SERCOM0_)  { opts.ser.print(" CLK_SERCOM0_APB"); }
    if (PM->APBCMASK.bit.SERCOM1_)  { opts.ser.print(" CLK_SERCOM1_APB"); }
    if (PM->APBCMASK.bit.SERCOM2_)  { opts.ser.print(" CLK_SERCOM2_APB"); }
    if (PM->APBCMASK.bit.SERCOM3_)  { opts.ser.print(" CLK_SERCOM3_APB"); }
    if (PM->APBCMASK.bit.SERCOM4_)  { opts.ser.print(" CLK_SERCOM4_APB"); }
    if (PM->APBCMASK.bit.SERCOM5_)  { opts.ser.print(" CLK_SERCOM5_APB"); }
    if (PM->APBCMASK.bit.TCC0_)     { opts.ser.print(" CLK_TCC0_APB"); }
    if (PM->APBCMASK.bit.TCC1_)     { opts.ser.print(" CLK_TCC1_APB"); }
    if (PM->APBCMASK.bit.TCC2_)     { opts.ser.print(" CLK_TCC2_APB"); }
    if (PM->APBCMASK.bit.TC3_)      { opts.ser.print(" CLK_TC3_APB"); }
    if (PM->APBCMASK.bit.TC4_)      { opts.ser.print(" CLK_TC4_APB"); }
    if (PM->APBCMASK.bit.TC5_)      { opts.ser.print(" CLK_TC5_APB"); }
    if (PM->APBCMASK.bit.TC6_)      { opts.ser.print(" CLK_TC6_APB"); }
    if (PM->APBCMASK.bit.TC7_)      { opts.ser.print(" CLK_TC7_APB"); }
    if (PM->APBCMASK.bit.ADC_)      { opts.ser.print(" CLK_ADC_APB"); }
    if (PM->APBCMASK.bit.AC_)       { opts.ser.print(" CLK_AC_APB"); }
    if (PM->APBCMASK.bit.DAC_)      { opts.ser.print(" CLK_DAC_APB"); }
    if (PM->APBCMASK.bit.PTC_)      { opts.ser.print(" CLK_PTC_APB"); }
    if (PM->APBCMASK.bit.I2S_)      { opts.ser.print(" CLK_I2S_APB"); }
    opts.ser.println("");

    // INTENCLR
    // INTENSET
    // INTFLAG

    opts.ser.print("RCAUSE: ");
    PRINTFLAG(PM->RCAUSE, POR);
    PRINTFLAG(PM->RCAUSE, BOD12);
    PRINTFLAG(PM->RCAUSE, BOD33);
    PRINTFLAG(PM->RCAUSE, EXT);
    if (PM->RCAUSE.reg & PM_RCAUSE_WDT) opts.ser.print(" WDT");
    PRINTFLAG(PM->RCAUSE, SYST);
    opts.ser.println("");
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
    { "USB_DM",         0, 24, { "EIC:12",  NULL,               "SERCOM3:2",    "SERCOM5:2",    "TC5:0",    "TCC1:2",   "USB:DM",   NULL } },
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
void printZeroRegPORT(ZeroRegOptions &opts) {
    opts.ser.println("--------------------------- PORT");
    for (uint8_t p = 0; p < port_pin_count; p++) {
        const Port_Pin &pin = port_pins[p];
        bool dir    = bitRead(PORT->Group[pin.grp].DIR.reg, pin.idx);
        bool inen   = PORT->Group[pin.grp].PINCFG[pin.idx].bit.INEN;
        bool pullen = PORT->Group[pin.grp].PINCFG[pin.idx].bit.PULLEN;
        bool pmuxen = PORT->Group[pin.grp].PINCFG[pin.idx].bit.PMUXEN;
        bool disabled = !dir && !inen && !pullen && !pmuxen;        // [22.6.3.4] Digital Functionality Disabled
        if (disabled && !opts.showDisabled) {
            continue;
        }
        opts.ser.print(pin.grp ? "PB" : "PA");      // FUTURE: fix this if/when there's a port C
        PRINTPAD2(pin.idx);
        opts.ser.print(" ");
        opts.ser.print(pin.name);
        opts.ser.print(": ");
        if (disabled) {
            opts.ser.println(" --disabled--");
            continue;
        }
        if (pmuxen) {
            uint8_t pmux = 0xF & PORT->Group[pin.grp].PMUX[pin.idx / 2].reg >> (4 * (pin.idx % 2));
            opts.ser.print(" PMUX=");
            if (pin.pmux[pmux]) {
                opts.ser.print(pin.pmux[pmux]);
            } else {
                // shouldn't get here so we'll print some debugging
                PRINTHEX(pmux);
            }
        } else {
            opts.ser.print(" DIR=");
            opts.ser.print(dir ? "OUT" : "IN");
            PRINTFLAG(PORT->Group[pin.grp].PINCFG[pin.idx], INEN);
            if (!dir && pullen) {
                opts.ser.print(bitRead(PORT->Group[pin.grp].OUT.reg, pin.idx) ? " PULLUP" : " PULLDOWN");
            }
            PRINTFLAG(PORT->Group[pin.grp].PINCFG[pin.idx], DRVSTR);
        }
        opts.ser.println("");
    }
}


void printZeroRegRTC_MODE0(ZeroRegOptions &opts, RtcMode0 &mode) {
    if (RTC->MODE0.CTRL.bit.ENABLE || opts.showDisabled) {
        opts.ser.println("--------------------------- RTC MODE0");
    }
    if (! RTC->MODE0.CTRL.bit.ENABLE) {
        if (opts.showDisabled) {
            opts.ser.println("--disabled--");
        }
        return;
    }
    opts.ser.print("CTRL:  PRESCALER=");
    PRINTSCALE(mode.CTRL.bit.PRESCALER);
    PRINTFLAG(mode.CTRL, MATCHCLR);
    opts.ser.println("");

    opts.ser.print("READREQ: ");
    PRINTFLAG(mode.READREQ, RCONT);
    opts.ser.println("");

    opts.ser.print("EVCTRL: ");
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
    opts.ser.println("");

    // INTENCLR
    // INTENSET
    // INTFLAG
    // DBGCTRL

    opts.ser.print("FREQCORR:  ");
    PRINTHEX(mode.FREQCORR.reg);
    opts.ser.println("");

    opts.ser.print("COMP0:  ");
    PRINTHEX(mode.COMP[0].reg);
    opts.ser.println("");
}


void printZeroRegRTC_MODE1(ZeroRegOptions &opts, RtcMode1 &mode) {
    if (RTC->MODE1.CTRL.bit.ENABLE || opts.showDisabled) {
        opts.ser.println("--------------------------- RTC MODE1");
    }
    if (! RTC->MODE1.CTRL.bit.ENABLE) {
        if (opts.showDisabled) {
            opts.ser.println("--disabled--");
        }
        return;
    }
    opts.ser.print("CTRL:  PRESCALER=");
    PRINTSCALE(mode.CTRL.bit.PRESCALER);
    opts.ser.println("");

    opts.ser.print("READREQ: ");
    PRINTFLAG(mode.READREQ, RCONT);
    opts.ser.println("");

    opts.ser.print("EVCTRL: ");
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
    opts.ser.println("");

    // INTENCLR
    // INTENSET
    // INTFLAG
    // DBGCTRL

    opts.ser.print("FREQCORR:  ");
    PRINTHEX(mode.FREQCORR.reg);
    opts.ser.println("");

    opts.ser.print("PER:  PER=");
    PRINTHEX(mode.PER.bit.PER);
    opts.ser.println("");

    opts.ser.print("COMP0:  ");
    PRINTHEX(mode.COMP[0].reg);
    opts.ser.println("");

    opts.ser.print("COMP1:  ");
    PRINTHEX(mode.COMP[1].reg);
    opts.ser.println("");
}


void printZeroRegRTC_MODE2(ZeroRegOptions &opts, RtcMode2 &mode) {
    if (RTC->MODE2.CTRL.bit.ENABLE || opts.showDisabled) {
        opts.ser.println("--------------------------- RTC MODE2");
    }
    if (! RTC->MODE2.CTRL.bit.ENABLE) {
        if (opts.showDisabled) {
            opts.ser.println("--disabled--");
        }
        return;
    }
    opts.ser.print("CTRL:  PRESCALER=");
    PRINTSCALE(mode.CTRL.bit.PRESCALER);
    PRINTFLAG(mode.CTRL, CLKREP);
    PRINTFLAG(mode.CTRL, MATCHCLR);
    opts.ser.println("");

    opts.ser.print("READREQ: ");
    PRINTFLAG(mode.READREQ, RCONT);
    opts.ser.println("");

    opts.ser.print("EVCTRL: ");
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
    opts.ser.println("");

    // INTENCLR
    // INTENSET
    // INTFLAG
    // DBGCTRL

    opts.ser.print("FREQCORR:  ");
    PRINTHEX(mode.FREQCORR.reg);
    opts.ser.println("");

    opts.ser.print("ALARM:  ");
    PRINTPAD2(mode.Mode2Alarm[0].ALARM.bit.YEAR);
    opts.ser.print("-");
    PRINTPAD2(mode.Mode2Alarm[0].ALARM.bit.MONTH);
    opts.ser.print("-");
    PRINTPAD2(mode.Mode2Alarm[0].ALARM.bit.DAY);
    opts.ser.print(" ");
    PRINTPAD2(mode.Mode2Alarm[0].ALARM.bit.HOUR);
    opts.ser.print(":");
    PRINTPAD2(mode.Mode2Alarm[0].ALARM.bit.MINUTE);
    opts.ser.print(":");
    PRINTPAD2(mode.Mode2Alarm[0].ALARM.bit.SECOND);
    opts.ser.println("");

    opts.ser.print("MASK:  ");
    switch (mode.Mode2Alarm[0].MASK.bit.SEL) {
        case 0x0: opts.ser.print("OFF"); break;
        case 0x1: opts.ser.print("SS"); break;
        case 0x2: opts.ser.print("MM:SS"); break;
        case 0x3: opts.ser.print("HH:MM:SS"); break;
        case 0x4: opts.ser.print("DD HH:MM:SS"); break;
        case 0x5: opts.ser.print("MM-DD HH:MM:SS"); break;
        case 0x6: opts.ser.print("YY-MM-DD HH:MM:SS"); break;
        /* 0x7 reserved */
    }
    opts.ser.println("");
}


void printZeroRegRTC(ZeroRegOptions &opts) {
    while (RTC->MODE0.CTRL.bit.SWRST || RTC->MODE0.STATUS.bit.SYNCBUSY) {}
    switch (RTC->MODE0.CTRL.bit.MODE) {
        case 0x0: printZeroRegRTC_MODE0(opts, RTC->MODE0); break;
        case 0x1: printZeroRegRTC_MODE1(opts, RTC->MODE1); break;
        case 0x2: printZeroRegRTC_MODE2(opts, RTC->MODE2); break;
    }
}


void printZeroRegSBMATRIX(ZeroRegOptions &opts) {
    opts.ser.println("--------------------------- SBMATRIX");
    for (uint8_t i = 0; i < 16; i++) {
        opts.ser.print("PRS ");
        PRINTHEX(i);
        opts.ser.print(":  ");
        opts.ser.print(SBMATRIX->Prs[i].PRAS.reg, BIN);
        opts.ser.print(" ");
        opts.ser.print(SBMATRIX->Prs[i].PRBS.reg, BIN);
        opts.ser.println("");
    }
    for (uint8_t i = 0; i < 16; i++) {
        opts.ser.print("SFR ");
        PRINTHEX(i);
        opts.ser.print(":  ");
        opts.ser.print(SBMATRIX->SFR[i].reg, BIN);
        opts.ser.print(" ");
        opts.ser.print(SBMATRIX->SFR[i].reg, BIN);
        opts.ser.println("");
    }
}


void printZeroRegSERCOM_I2CM(ZeroRegOptions &opts, SercomI2cm &i2cm) {
    opts.ser.print("CTRLA: ");
    PRINTFLAG(i2cm.CTRLA, RUNSTDBY);
    PRINTFLAG(i2cm.CTRLA, PINOUT);
    opts.ser.print(" SDAHOLD=");
    switch (i2cm.CTRLA.bit.SDAHOLD) {
        case 0x0: opts.ser.print("DIS"); break;
        case 0x1: opts.ser.print("75NS"); break;
        case 0x2: opts.ser.print("450NS"); break;
        case 0x3: opts.ser.print("600NS"); break;
    }
    PRINTFLAG(i2cm.CTRLA, MEXTTOEN);
    PRINTFLAG(i2cm.CTRLA, SEXTTOEN);
    opts.ser.print(" SPEED=");
    switch (i2cm.CTRLA.bit.SPEED) {
        case 0x0: opts.ser.print("SM<100kHz,FM<400kHz"); break;
        case 0x1: opts.ser.print("FM+<1MHz"); break;
        case 0x2: opts.ser.print("HS<3.4MHz"); break;
    }
    PRINTFLAG(i2cm.CTRLA, SCLSM);
    opts.ser.print(" INACTOUT=");
    switch (i2cm.CTRLA.bit.INACTOUT) {
        case 0x0: opts.ser.print("DIS"); break;
        case 0x1: opts.ser.print("55NS"); break;
        case 0x2: opts.ser.print("105NS"); break;
        case 0x3: opts.ser.print("205NS"); break;
    }
    PRINTFLAG(i2cm.CTRLA, LOWTOUTEN);
    opts.ser.println("");

    opts.ser.print("CTRLB: ");
    PRINTFLAG(i2cm.CTRLB, QCEN);
    if (i2cm.CTRLB.bit.SMEN) {
        opts.ser.print(" SMEN");
        opts.ser.print(" ACKACT=");
        opts.ser.print(i2cm.CTRLB.bit.ACKACT ? "ACK" : "NACK");
    }
    opts.ser.println("");

    opts.ser.print("BAUD:  ");
    PRINTHEX(i2cm.BAUD.reg);  // FUTURE
    opts.ser.println("");

    // INTENCLR
    // INTENSET
    // INTFLAG
    // STATUS
    // ADDR
    // DATA
    // DBGCTRL
}
void printZeroRegSERCOM_I2CS(ZeroRegOptions &opts, SercomI2cs &i2cs) {
    opts.ser.print("CTRLA: ");
    PRINTFLAG(i2cs.CTRLA, RUNSTDBY);
    PRINTFLAG(i2cs.CTRLA, PINOUT);
    opts.ser.print(" SDAHOLD=");
    switch (i2cs.CTRLA.bit.SDAHOLD) {
        case 0x0: opts.ser.print("DIS"); break;
        case 0x1: opts.ser.print("75NS"); break;
        case 0x2: opts.ser.print("450NS"); break;
        case 0x3: opts.ser.print("600NS"); break;
    }
    PRINTFLAG(i2cs.CTRLA, SEXTTOEN);
    opts.ser.print(" SPEED=");
    switch (i2cs.CTRLA.bit.SPEED) {
        case 0x0: opts.ser.print("SM<100kHz,FM<400kHz"); break;
        case 0x1: opts.ser.print("FM+<1MHz"); break;
        case 0x2: opts.ser.print("HS<3.4MHz"); break;
    }
    PRINTFLAG(i2cs.CTRLA, SCLSM);
    PRINTFLAG(i2cs.CTRLA, LOWTOUTEN);
    opts.ser.println("");

    opts.ser.print("CTRLB: ");
    if (i2cs.CTRLB.bit.SMEN) {
        opts.ser.print(" SMEN");
        opts.ser.print(" ACKACT=");
        opts.ser.print(i2cs.CTRLB.bit.ACKACT ? "ACK" : "NACK");
    }
    PRINTFLAG(i2cs.CTRLB, GCMD);
    PRINTFLAG(i2cs.CTRLB, AACKEN);
    opts.ser.print(" AMODE=");
    switch (i2cs.CTRLB.bit.AMODE) {
        case 0x0: opts.ser.print("MASK"); break;
        case 0x1: opts.ser.print("2ADDRS"); break;
        case 0x2: opts.ser.print("RANGE"); break;
    }
    opts.ser.println("");

    // INTENCLR
    // INTENSET
    // INTFLAG
    // STATUS

    opts.ser.print("ADDR: ");
    PRINTFLAG(i2cs.ADDR, GENCEN);
    opts.ser.print(" ADDR=");
    PRINTHEX(i2cs.ADDR.bit.ADDR);
    PRINTFLAG(i2cs.ADDR, TENBITEN);
    opts.ser.print(" ADDRMASK=");
    PRINTHEX(i2cs.ADDR.bit.ADDRMASK);
    opts.ser.println("");

    // DATA
}
void printZeroRegSERCOM_SPI(ZeroRegOptions &opts, SercomSpi &spi, bool master) {
    opts.ser.print("CTRLA: ");
    PRINTFLAG(spi.CTRLA, RUNSTDBY);
    PRINTFLAG(spi.CTRLA, IBON);
    opts.ser.print(master ? " MISO=" : " MOSI=");
    switch (spi.CTRLA.bit.DIPO) {
        case 0x0: opts.ser.print("PAD0"); break;
        case 0x1: opts.ser.print("PAD1"); break;
        case 0x2: opts.ser.print("PAD2"); break;
        case 0x3: opts.ser.print("PAD3"); break;
    }
    opts.ser.print(master ? " MOSI=" : " MISO=");
    switch (spi.CTRLA.bit.DOPO) {
        case 0x0:
            opts.ser.print("PAD0 SCK=PAD1");
            if (!master) opts.ser.print(" SS=PAD2");
            break;
        case 0x1:
            opts.ser.print("PAD2 SCK=PAD3");
            if (!master) opts.ser.print(" SS=PAD1");
            break;
        case 0x2:
            opts.ser.print("PAD3 SCK=PAD1");
            if (!master) opts.ser.print(" SS=PAD2");
            break;
        case 0x3:
            opts.ser.print("PAD0 SCK=PAD3");
            if (!master) opts.ser.print(" SS=PAD1");
            break;
    }
    opts.ser.print(" FORM=");
    switch (spi.CTRLA.bit.FORM) {
        case 0x0: opts.ser.print("SPI"); break;
        case 0x1: /* reserved */ break;
        case 0x2: opts.ser.print("SPI_ADDR"); break;
        /* 0x3-0xF reserved */
    }
    opts.ser.print(" CPHA="); opts.ser.print(spi.CTRLA.bit.CPHA ? "TRAILING" : "LEADING");
    opts.ser.print(" CPOL="); opts.ser.print(spi.CTRLA.bit.CPOL ? "HIGH" : "LOW");
    opts.ser.print(" DORD="); opts.ser.print(spi.CTRLA.bit.DORD ? "LSB" : "MSB");
    opts.ser.println("");

    opts.ser.print("CTRLB: ");
    opts.ser.print(" CHSIZE=");
    switch (spi.CTRLB.bit.CHSIZE) {
        case 0x0: opts.ser.print("8bit"); break;
        case 0x1: opts.ser.print("9bit"); break;
        /* 0x2-0xF reserved */
    }
    PRINTFLAG(spi.CTRLB, PLOADEN);
    PRINTFLAG(spi.CTRLB, SSDE);
    PRINTFLAG(spi.CTRLB, MSSEN);
    opts.ser.print(" AMODE=");
    switch (spi.CTRLB.bit.AMODE) {
        case 0x0: opts.ser.print("MASK"); break;
        case 0x1: opts.ser.print("2ADDRS"); break;
        case 0x2: opts.ser.print("RANGE"); break;
    }
    PRINTFLAG(spi.CTRLB, RXEN);
    opts.ser.println("");

    opts.ser.print("BAUD:  ");
    PRINTHEX(spi.BAUD.reg);   // FUTURE
    opts.ser.println("");

    // INTENCLR
    // INTENSET
    // INTFLAG
    // STATUS
    // SYNCBUSY

    if (spi.CTRLA.bit.FORM == 0x2) {
        opts.ser.print("ADDR: ");
        opts.ser.print(" ADDR=");
        PRINTHEX(spi.ADDR.bit.ADDR);
        opts.ser.print(" ADDRMASK=");
        PRINTHEX(spi.ADDR.bit.ADDRMASK);
        opts.ser.println("");
    }

    // DATA
}
void printZeroRegSERCOM_USART(ZeroRegOptions &opts, SercomUsart &usart) {
    opts.ser.print("CTRLA: ");
    PRINTFLAG(usart.CTRLA, RUNSTDBY);
    PRINTFLAG(usart.CTRLA, IBON);
    opts.ser.print(" CMODE="); opts.ser.print(usart.CTRLA.bit.CMODE ? "SYNC" : "ASYNC");
    opts.ser.print(" CPOL="); opts.ser.print(usart.CTRLA.bit.CPOL ? "FALLING" : "RISING");
    opts.ser.print(" DORD="); opts.ser.print(usart.CTRLA.bit.CPOL ? "LSB" : "MSB");
    opts.ser.print(" SAMPR=");
    PRINTHEX(usart.CTRLA.bit.SAMPR);  // FUTURE
    opts.ser.print(" SAMPA=");
    PRINTHEX(usart.CTRLA.bit.SAMPA);  // FUTURE
    opts.ser.print(" FORM=");
    switch(usart.CTRLA.bit.FORM) {
        case 0x0: opts.ser.print("USART"); break;
        case 0x1: opts.ser.print("USART,PARITY"); break;
        case 0x2: /* reserved */ break;
        case 0x3: /* reserved */ break;
        case 0x4: opts.ser.print("AUTOBAUD"); break;
        case 0x5: opts.ser.print("AUTOBAUD,PARITY"); break;
        /* 0x6-0xF reserved */
    }
    switch (usart.CTRLA.bit.RXPO) {
        case 0x0: opts.ser.print(" RX=PAD0"); break;
        case 0x1: opts.ser.print(" RX=PAD1"); break;
        case 0x2: opts.ser.print(" RX=PAD2"); break;
        case 0x3: opts.ser.print(" RX=PAD3"); break;
    }
    switch (usart.CTRLA.bit.TXPO) {
        case 0x0:
            opts.ser.print(" TX=PAD0");
            if (usart.CTRLA.bit.MODE == 0) opts.ser.print(" XCK=PAD1");
            break;
        case 0x1:
            opts.ser.print(" TX=PAD2");
            if (usart.CTRLA.bit.MODE == 0) opts.ser.print(" XCK=PAD3");
            break;
        case 0x2:
            opts.ser.print(" TX=PAD0");
            opts.ser.print(" RTS=PAD2");
            opts.ser.print(" CTS=PAD3");
            break;
    }
    opts.ser.println("");

    opts.ser.print("CTRLB:  CHSIZE=");
    switch (usart.CTRLB.bit.CHSIZE) {
        case 0x0: opts.ser.print("8bit"); break;
        case 0x1: opts.ser.print("9bit"); break;
        case 0x2: /* reserved */ break;
        case 0x3: /* reserved */ break;
        case 0x4: /* reserved */ break;
        case 0x5: opts.ser.print("5bit"); break;
        case 0x6: opts.ser.print("6bit"); break;
        case 0x7: opts.ser.print("7bit"); break;
    }
    PRINTFLAG(usart.CTRLB, SBMODE);
    PRINTFLAG(usart.CTRLB, COLDEN);
    PRINTFLAG(usart.CTRLB, SFDE);
    PRINTFLAG(usart.CTRLB, ENC);
    PRINTFLAG(usart.CTRLB, PMODE);
    PRINTFLAG(usart.CTRLB, TXEN);
    PRINTFLAG(usart.CTRLB, RXEN);
    opts.ser.println("");

    opts.ser.print("BAUD:  ");
    PRINTHEX(usart.BAUD.reg); // FUTURE
    opts.ser.println("");

    if (usart.CTRLB.bit.ENC) {
        opts.ser.print("RXPL:  ");
        PRINTHEX(usart.RXPL.reg);
        opts.ser.println("");
    }

    // INTENCLR
    // INTENSET
    // INTFLAG
    // STATUS
    // DATA
    // DBGCTRL
}
void printZeroRegSERCOM(ZeroRegOptions &opts, Sercom* sercom, uint8_t idx) {
    while (sercom->I2CM.CTRLA.bit.SWRST || sercom->I2CM.SYNCBUSY.reg) {}
    if (! sercom->I2CM.CTRLA.bit.ENABLE) {
        if (opts.showDisabled) {
            opts.ser.print("--------------------------- SERCOM");
            opts.ser.print(idx);
            opts.ser.println("\n--disabled--");
        }
        return;
    }
    opts.ser.print("--------------------------- SERCOM");
    opts.ser.print(idx);
    switch (sercom->I2CM.CTRLA.bit.MODE) {
        case 0x0: opts.ser.println(" USART");       printZeroRegSERCOM_USART(opts, sercom->USART); break;
        case 0x1: opts.ser.println(" USART");       printZeroRegSERCOM_USART(opts, sercom->USART); break;
        case 0x2: opts.ser.println(" SPI slave");   printZeroRegSERCOM_SPI(opts, sercom->SPI, false); break;
        case 0x3: opts.ser.println(" SPI master");  printZeroRegSERCOM_SPI(opts, sercom->SPI, true); break;
        case 0x4: opts.ser.println(" I2C slave");   printZeroRegSERCOM_I2CS(opts, sercom->I2CS); break;
        case 0x5: opts.ser.println(" I2C master");  printZeroRegSERCOM_I2CM(opts, sercom->I2CM); break;
    }
}


void printZeroRegSYSCTRL(ZeroRegOptions &opts) {
    opts.ser.println("--------------------------- SYSCTRL");

    // INTENCLR
    // INTENSET
    // INTFLAG

    opts.ser.print("PCLKSR: ");
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
    opts.ser.println("");

    if (SYSCTRL->XOSC.bit.ENABLE) {
        opts.ser.print("XOSC: ");
        PRINTFLAG(SYSCTRL->XOSC, XTALEN);
        PRINTFLAG(SYSCTRL->XOSC, RUNSTDBY);
        PRINTFLAG(SYSCTRL->XOSC, ONDEMAND);
        PRINTFLAG(SYSCTRL->XOSC, AMPGC);
        opts.ser.print(" GAIN=");
        switch (SYSCTRL->XOSC.bit.GAIN) {
            case 0x0: opts.ser.print("2MHz"); break;
            case 0x1: opts.ser.print("4MHz"); break;
            case 0x2: opts.ser.print("8MHz"); break;
            case 0x3: opts.ser.print("16MHz"); break;
            case 0x4: opts.ser.print("30MHz"); break;
        }
        opts.ser.print(" STARTUP=");
        switch (SYSCTRL->XOSC.bit.STARTUP) {
            case 0x0: opts.ser.print("31us"); break;
            case 0x1: opts.ser.print("61us"); break;
            case 0x2: opts.ser.print("122us"); break;
            case 0x3: opts.ser.print("244us"); break;
            case 0x4: opts.ser.print("488us"); break;
            case 0x5: opts.ser.print("977us"); break;
            case 0x6: opts.ser.print("1953us"); break;
            case 0x7: opts.ser.print("3906us"); break;
            case 0x8: opts.ser.print("7813us"); break;
            case 0x9: opts.ser.print("15625us"); break;
            case 0xA: opts.ser.print("31250us"); break;
            case 0xB: opts.ser.print("62500us"); break;
            case 0xC: opts.ser.print("125ms"); break;
            case 0xD: opts.ser.print("250ms"); break;
            case 0xE: opts.ser.print("500ms"); break;
            case 0xF: opts.ser.print("1s"); break;
        }
        opts.ser.println("");
    } else {
        if (opts.showDisabled) {
            opts.ser.println("XOSC:  --disabled--");
        }
    }

    if (SYSCTRL->XOSC32K.bit.ENABLE) {
        opts.ser.print("XOSC32K: ");
        PRINTFLAG(SYSCTRL->XOSC32K, XTALEN);
        PRINTFLAG(SYSCTRL->XOSC32K, EN32K);
        PRINTFLAG(SYSCTRL->XOSC32K, EN1K);
        PRINTFLAG(SYSCTRL->XOSC32K, AAMPEN);
        PRINTFLAG(SYSCTRL->XOSC32K, RUNSTDBY);
        PRINTFLAG(SYSCTRL->XOSC32K, ONDEMAND);
        PRINTFLAG(SYSCTRL->XOSC32K, WRTLOCK);
        opts.ser.print(" STARTUP=");
        switch (SYSCTRL->XOSC32K.bit.STARTUP) {
            case 0x0: opts.ser.print("122us"); break;
            case 0x1: opts.ser.print("1068us"); break;
            case 0x2: opts.ser.print("62592us"); break;
            case 0x3: opts.ser.print("125092us"); break;
            case 0x4: opts.ser.print("500092us"); break;
            case 0x5: opts.ser.print("1000092us"); break;
            case 0x6: opts.ser.print("2000092us"); break;
            case 0x7: opts.ser.print("4000092us"); break;
        }
        opts.ser.println("");
    } else {
        if (opts.showDisabled) {
            opts.ser.println("XOSC32K:  --disabled--");
        }
    }

    if (SYSCTRL->OSC32K.bit.ENABLE) {
        opts.ser.print("OSC32K: ");
        PRINTFLAG(SYSCTRL->OSC32K, EN32K);
        PRINTFLAG(SYSCTRL->OSC32K, EN1K);
        PRINTFLAG(SYSCTRL->OSC32K, RUNSTDBY);
        PRINTFLAG(SYSCTRL->OSC32K, ONDEMAND);
        PRINTFLAG(SYSCTRL->OSC32K, WRTLOCK);
        opts.ser.print(" STARTUP=");
        switch (SYSCTRL->OSC32K.bit.STARTUP) {
            case 0x0: opts.ser.print("92us"); break;
            case 0x1: opts.ser.print("122us"); break;
            case 0x2: opts.ser.print("183us"); break;
            case 0x3: opts.ser.print("305us"); break;
            case 0x4: opts.ser.print("549us"); break;
            case 0x5: opts.ser.print("1038us"); break;
            case 0x6: opts.ser.print("2014us"); break;
            case 0x7: opts.ser.print("3967us"); break;
        }
        opts.ser.print(" CALIB=");
        PRINTHEX(SYSCTRL->OSC32K.bit.CALIB);
        opts.ser.println("");
    } else {
        if (opts.showDisabled) {
            opts.ser.println("OSC32K:  --disabled--");
        }
    }

    opts.ser.print("OSCULP32K: ");
    PRINTFLAG(SYSCTRL->OSCULP32K, WRTLOCK);
    opts.ser.print(" CALIB=");
    PRINTHEX(SYSCTRL->OSCULP32K.bit.CALIB);
    opts.ser.println("");

    if (SYSCTRL->OSC8M.bit.ENABLE) {
        opts.ser.print("OSC8M: ");
        PRINTFLAG(SYSCTRL->OSC8M, RUNSTDBY);
        PRINTFLAG(SYSCTRL->OSC8M, ONDEMAND);
        opts.ser.print(" PRESC=");
        PRINTSCALE(SYSCTRL->OSC8M.bit.PRESC);
        opts.ser.print(" CALIB=");
        PRINTHEX(SYSCTRL->OSC8M.bit.CALIB);
        opts.ser.print(" FRANGE=");
        switch (SYSCTRL->OSC8M.bit.FRANGE) {
            case 0x0: opts.ser.print("4-6MHz"); break;
            case 0x1: opts.ser.print("6-8MHz"); break;
            case 0x2: opts.ser.print("8-11MHz"); break;
            case 0x3: opts.ser.print("11-15MHz"); break;
        }
        opts.ser.println("");
    } else {
        if (opts.showDisabled) {
            opts.ser.println("OSC8M:  --disabled--");
        }
    }

    if (SYSCTRL->DFLLCTRL.bit.ENABLE) {
        opts.ser.print("DFLL: ");
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
        opts.ser.println("");
    } else {
        if (opts.showDisabled) {
            opts.ser.println("DFLL:  --disabled--");
        }
    }

    if (SYSCTRL->BOD33.bit.ENABLE) {
        opts.ser.print("BOD33: ");
        PRINTFLAG(SYSCTRL->BOD33, HYST);
        PRINTFLAG(SYSCTRL->BOD33, RUNSTDBY);
        PRINTFLAG(SYSCTRL->BOD33, MODE);
        PRINTFLAG(SYSCTRL->BOD33, CEN);
        opts.ser.print(" ACTION=");
        switch (SYSCTRL->BOD33.bit.ACTION) {
            case 0x0: opts.ser.print("NONE"); break;
            case 0x1: opts.ser.print("RESET"); break;
            case 0x2: opts.ser.print("INT"); break;
        }
        opts.ser.print(" PSEL=");
        PRINTSCALE(SYSCTRL->BOD33.bit.PSEL);
        opts.ser.print(" LEVEL=");
        PRINTHEX(SYSCTRL->BOD33.bit.LEVEL);
        opts.ser.println("");
    } else {
        if (opts.showDisabled) {
            opts.ser.println("BOD33:  --disabled--");
        }
    }

    opts.ser.print("VREG: ");
    PRINTFLAG(SYSCTRL->VREG, RUNSTDBY);
    PRINTFLAG(SYSCTRL->VREG, FORCELDO);
    opts.ser.println("");

    opts.ser.print("VREF: ");
    PRINTFLAG(SYSCTRL->VREF, TSEN);
    PRINTFLAG(SYSCTRL->VREF, BGOUTEN);
    opts.ser.print(" CALIB=");
    PRINTHEX(SYSCTRL->VREF.bit.CALIB);
    opts.ser.println("");

    if (SYSCTRL->DPLLCTRLA.bit.ENABLE) {
        opts.ser.print("DPLL: ");
        PRINTFLAG(SYSCTRL->DPLLCTRLA, RUNSTDBY);
        PRINTFLAG(SYSCTRL->DPLLCTRLA, ONDEMAND);
        // DPLLRATIO
        // DPLLCTRLB
        // DPLLSTATUS
        opts.ser.println("");
    } else {
        if (opts.showDisabled) {
            opts.ser.println("DPLL:  --disabled--");
        }
    }
}


void printZeroRegTC(ZeroRegOptions &opts, Tc* tc, uint8_t idx) {
    // FUTURE
}


void printZeroRegTCC(ZeroRegOptions &opts, Tcc* tcc, uint8_t idx) {
    // FUTURE
}


void printZeroRegUSB(ZeroRegOptions &opts) {
    // FUTURE
}


void printZeroRegWDT(ZeroRegOptions &opts) {
    while (WDT->STATUS.bit.SYNCBUSY) {}
    if (! WDT->CTRL.bit.ENABLE) {
        if (opts.showDisabled) {
            opts.ser.println("--------------------------- WDT\n--disabled--");
        }
        return;
    }
    opts.ser.println("--------------------------- WDT");

    opts.ser.print("CTRL: ");
    PRINTFLAG(WDT->CTRL, WEN);
    PRINTFLAG(WDT->CTRL, ALWAYSON);
    opts.ser.println("");

    opts.ser.print("CONFIG:  PER=");
    PRINTSCALE(3 + WDT->CONFIG.bit.PER);
    opts.ser.print(" WINDOW=");
    PRINTSCALE(3 + WDT->CONFIG.bit.WINDOW);
    opts.ser.println("");

    opts.ser.print("EWCTRL:  EWOFFSET=");
    PRINTSCALE(3 + WDT->EWCTRL.bit.EWOFFSET);
    opts.ser.println("");

    // INTENCLR
    // INTENSET
    // INTFLAG

    opts.ser.print("NVM user row: ");
    opts.ser.print(" ENABLE=");
    opts.ser.print(READFUSE(WDT, ENABLE));
    opts.ser.print(" ALWAYSON=");
    opts.ser.print(READFUSE(WDT, ALWAYSON));
    opts.ser.print(" PER=");
    PRINTSCALE(3 + READFUSE(WDT, PER));
    opts.ser.print(" WINDOW=");
    PRINTSCALE(3 + READ2FUSES(WDT, WINDOW, 1));
    opts.ser.print(" EWOFFSET=");
    PRINTSCALE(3 + READFUSE(WDT, EWOFFSET));
    opts.ser.print(" WEN=");
    opts.ser.print(READFUSE(WDT, WEN));
    opts.ser.println("");
}


void printZeroRegs(ZeroRegOptions &opts) {
    printZeroRegAC(opts);
    printZeroRegADC(opts);
    printZeroRegDAC(opts);
    printZeroRegDMAC(opts);
    printZeroRegDSU(opts);
    printZeroRegEIC(opts);
    printZeroRegEVSYS(opts);
    printZeroRegGCLK(opts);
    printZeroRegI2S(opts);
    printZeroRegMTB(opts);
    printZeroRegNVMCTRL(opts);
    printZeroRegPAC(opts);
    printZeroRegPM(opts);
    printZeroRegPORT(opts);
    printZeroRegRTC(opts);
    //printZeroRegSBMATRIX(opts);    // questionable value
    printZeroRegSERCOM(opts, SERCOM0, 0);
    printZeroRegSERCOM(opts, SERCOM1, 1);
    printZeroRegSERCOM(opts, SERCOM2, 2);
    printZeroRegSERCOM(opts, SERCOM3, 3);
    printZeroRegSERCOM(opts, SERCOM4, 4);
    printZeroRegSERCOM(opts, SERCOM5, 5);
    printZeroRegSYSCTRL(opts);
    printZeroRegTCC(opts, TCC0, 0);
    printZeroRegTCC(opts, TCC1, 1);
    printZeroRegTCC(opts, TCC2, 2);
    printZeroRegTC(opts, TC3, 3);
    printZeroRegTC(opts, TC4, 4);
    printZeroRegTC(opts, TC5, 5);
    printZeroRegUSB(opts);
    printZeroRegWDT(opts);
}


