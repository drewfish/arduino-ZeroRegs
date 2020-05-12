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

/*
DSrevF:  Refers to SAM D21/DA1 Family datasheet revision F (March 2020).
*/


#include "ZeroRegs.h"
#include <samd.h>


static const char* ZeroRegs__DISABLED = "--disabled--";
static const char* ZeroRegs__RESERVED = "--reserved--";
static const char* ZeroRegs__empty = "";
#define PRINTFLAG(x,y) do { if (x.bit.y) { opts.out.print(" " #y); } } while(0)
#define PRINTHEX(x) do { opts.out.print("0x"); opts.out.print(x, HEX); } while(0)
#define PRINTNL() opts.out.println(ZeroRegs__empty)
#define PRINTPAD2(x) do { if (x < 10) { opts.out.print("0"); } opts.out.print(x, DEC); } while(0)
#define PRINTSCALE(x) (opts.out.print(1 << (x)))
#define READ2FUSES(x,y,o) ( ((*((uint32_t *) x##_FUSES_##y##_0_ADDR) & x##_FUSES_##y##_0_Msk) >> x##_FUSES_##y##_0_Pos) | (((*((uint32_t *) x##_FUSES_##y##_1_ADDR) & x##_FUSES_##y##_1_Msk) >> x##_FUSES_##y##_1_Pos) << o))
#define READADDR32(x) (*((uint32_t*)(x)))
#define READFUSE(x,y) ((*((uint32_t *) x##_FUSES_##y##_ADDR) & x##_FUSES_##y##_Msk) >> x##_FUSES_##y##_Pos)
#define READSCS(val,name) ( (val & (name##_Msk)) >> (name##_Pos) )
#define WRITE8(x,y) *((uint8_t*)&(x)) = uint8_t(y)


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


void printZeroRegEIC_SENSE(ZeroRegOptions &opts, uint8_t sense) {
    switch (sense) {
        case 0x0: opts.out.print("none"); break;
        case 0x1: opts.out.print("RISE"); break;
        case 0x2: opts.out.print("FALL"); break;
        case 0x3: opts.out.print("BOTH"); break;
        case 0x4: opts.out.print("HIGH"); break;
        case 0x5: opts.out.print("LOW"); break;
    }
}

void printZeroRegEIC(ZeroRegOptions &opts) {
    while (EIC->CTRL.bit.SWRST || EIC->STATUS.bit.SYNCBUSY) {}
    if (!EIC->CTRL.bit.ENABLE && !opts.showDisabled) {
        return;
    }
    opts.out.println("--------------------------- EIC");

    if (EIC->NMICTRL.bit.NMISENSE) {
        opts.out.print("NMI:  sense=");
        printZeroRegEIC_SENSE(opts, EIC->NMICTRL.bit.NMISENSE);
        if (EIC->NMICTRL.bit.NMIFILTEN) {
            opts.out.print(" FILTEN");
        }
        PRINTNL();
    } else {
        if (opts.showDisabled) {
            opts.out.println("NMI:  none");
        }
    }

    for (uint8_t extint = 0; extint < 18; extint++) {
        uint8_t cfg = extint / 8;
        uint8_t pos = (extint % 8) * 4;
        uint32_t entry = 0xF & (EIC->CONFIG[cfg].reg >> pos);
        if (!opts.showDisabled && (entry & 0x7) == 0) {
            continue;
        }
        opts.out.print("EXTINT");
        PRINTPAD2(extint);
        opts.out.print(":  sense=");
        printZeroRegEIC_SENSE(opts, entry & 0x7);
        if (entry & 0x8) {
            opts.out.print(" FILTEN");
        }
        if (EIC->EVCTRL.vec.EXTINTEO & (1 << extint)) {
            opts.out.print(" EXTINTEO");
        }
        if (EIC->WAKEUP.vec.WAKEUPEN & (1 << extint)) {
            opts.out.print(" WAKEUP");
        }
        PRINTNL();
    }
}


static const char* ZeroRegsEVSYS_EVGEN00 = "NONE";
static const char* ZeroRegsEVSYS_EVGEN01 = "RTC:CMP0";
static const char* ZeroRegsEVSYS_EVGEN02 = "RTC:CMP1";
static const char* ZeroRegsEVSYS_EVGEN03 = "RTC:OVF";
static const char* ZeroRegsEVSYS_EVGEN04 = "RTC:PER0";
static const char* ZeroRegsEVSYS_EVGEN05 = "RTC:PER1";
static const char* ZeroRegsEVSYS_EVGEN06 = "RTC:PER2";
static const char* ZeroRegsEVSYS_EVGEN07 = "RTC:PER3";
static const char* ZeroRegsEVSYS_EVGEN08 = "RTC:PER4";
static const char* ZeroRegsEVSYS_EVGEN09 = "RTC:PER5";
static const char* ZeroRegsEVSYS_EVGEN0A = "RTC:PER6";
static const char* ZeroRegsEVSYS_EVGEN0B = "RTC:PER7";
static const char* ZeroRegsEVSYS_EVGEN0C = "EIC:0";
static const char* ZeroRegsEVSYS_EVGEN0D = "EIC:1";
static const char* ZeroRegsEVSYS_EVGEN0E = "EIC:2";
static const char* ZeroRegsEVSYS_EVGEN0F = "EIC:3";
static const char* ZeroRegsEVSYS_EVGEN10 = "EIC:4";
static const char* ZeroRegsEVSYS_EVGEN11 = "EIC:5";
static const char* ZeroRegsEVSYS_EVGEN12 = "EIC:6";
static const char* ZeroRegsEVSYS_EVGEN13 = "EIC:7";
static const char* ZeroRegsEVSYS_EVGEN14 = "EIC:8";
static const char* ZeroRegsEVSYS_EVGEN15 = "EIC:9";
static const char* ZeroRegsEVSYS_EVGEN16 = "EIC:10";
static const char* ZeroRegsEVSYS_EVGEN17 = "EIC:11";
static const char* ZeroRegsEVSYS_EVGEN18 = "EIC:12";
static const char* ZeroRegsEVSYS_EVGEN19 = "EIC:13";
static const char* ZeroRegsEVSYS_EVGEN1A = "EIC:14";
static const char* ZeroRegsEVSYS_EVGEN1B = "EIC:15";
static const char* ZeroRegsEVSYS_EVGEN1C = ZeroRegs__RESERVED;
static const char* ZeroRegsEVSYS_EVGEN1D = ZeroRegs__RESERVED;
static const char* ZeroRegsEVSYS_EVGEN1E = "DMAC:0";
static const char* ZeroRegsEVSYS_EVGEN1F = "DMAC:1";
static const char* ZeroRegsEVSYS_EVGEN20 = "DMAC:2";
static const char* ZeroRegsEVSYS_EVGEN21 = "DMAC:3";
static const char* ZeroRegsEVSYS_EVGEN22 = "TCC0:OVF";
static const char* ZeroRegsEVSYS_EVGEN23 = "TCC0:TRG";
static const char* ZeroRegsEVSYS_EVGEN24 = "TCC0:CNT";
static const char* ZeroRegsEVSYS_EVGEN25 = "TCC0:MC0";
static const char* ZeroRegsEVSYS_EVGEN26 = "TCC0:MC1";
static const char* ZeroRegsEVSYS_EVGEN27 = "TCC0:MC2";
static const char* ZeroRegsEVSYS_EVGEN28 = "TCC0:MC3";
static const char* ZeroRegsEVSYS_EVGEN29 = "TCC1:OVF";
static const char* ZeroRegsEVSYS_EVGEN2A = "TCC1:TRG";
static const char* ZeroRegsEVSYS_EVGEN2B = "TCC1:CNT";
static const char* ZeroRegsEVSYS_EVGEN2C = "TCC1:MC0";
static const char* ZeroRegsEVSYS_EVGEN2D = "TCC1:MC1";
static const char* ZeroRegsEVSYS_EVGEN2E = "TCC2:OVF";
static const char* ZeroRegsEVSYS_EVGEN2F = "TCC2:TRG";
static const char* ZeroRegsEVSYS_EVGEN30 = "TCC2:CNT";
static const char* ZeroRegsEVSYS_EVGEN31 = "TCC2:MC0";
static const char* ZeroRegsEVSYS_EVGEN32 = "TCC2:MC1";
static const char* ZeroRegsEVSYS_EVGEN33 = "TC3:OVF";
static const char* ZeroRegsEVSYS_EVGEN34 = "TC3:MC0";
static const char* ZeroRegsEVSYS_EVGEN35 = "TC3:MC1";
static const char* ZeroRegsEVSYS_EVGEN36 = "TC4:OVF";
static const char* ZeroRegsEVSYS_EVGEN37 = "TC4:MC0";
static const char* ZeroRegsEVSYS_EVGEN38 = "TC4:MC1";
static const char* ZeroRegsEVSYS_EVGEN39 = "TC5:OVF";
static const char* ZeroRegsEVSYS_EVGEN3A = "TC5:MC0";
static const char* ZeroRegsEVSYS_EVGEN3B = "TC5:MC1";
static const char* ZeroRegsEVSYS_EVGEN3C = "TC6:OVF";
static const char* ZeroRegsEVSYS_EVGEN3D = "TC6:MC0";
static const char* ZeroRegsEVSYS_EVGEN3E = "TC6:MC1";
static const char* ZeroRegsEVSYS_EVGEN3F = "TC7:OVF";
static const char* ZeroRegsEVSYS_EVGEN40 = "TC7:MC0";
static const char* ZeroRegsEVSYS_EVGEN41 = "TC7:MC1";
static const char* ZeroRegsEVSYS_EVGEN42 = "ADC:RESRDY";
static const char* ZeroRegsEVSYS_EVGEN43 = "ADC:WINMON";
static const char* ZeroRegsEVSYS_EVGEN44 = "AC:COMP0";
static const char* ZeroRegsEVSYS_EVGEN45 = "AC:COMP1";
static const char* ZeroRegsEVSYS_EVGEN46 = "AC:WIN0";
static const char* ZeroRegsEVSYS_EVGEN47 = "DAC:EMPTY";
static const char* ZeroRegsEVSYS_EVGEN48 = "PTC:EOC";
static const char* ZeroRegsEVSYS_EVGEN49 = "PTC:WCOMP";
static const char* ZeroRegsEVSYS_EVGEN4A = "AC:COMP2";
static const char* ZeroRegsEVSYS_EVGEN4B = "AC:COMP3";
static const char* ZeroRegsEVSYS_EVGEN4C = "AC:WIN1";
static const char* ZeroRegsEVSYS_EVGEN4D = "TCC3:OVF";
static const char* ZeroRegsEVSYS_EVGEN4E = "TCC3:TRG";
static const char* ZeroRegsEVSYS_EVGEN4F = "TCC3:CNT";
static const char* ZeroRegsEVSYS_EVGEN50 = "TCC3:MC0";
static const char* ZeroRegsEVSYS_EVGEN51 = "TCC3:MC1";
static const char* ZeroRegsEVSYS_EVGEN52 = "TCC3:MC2";
static const char* ZeroRegsEVSYS_EVGEN53 = "TCC3:MC3";
static const char* ZeroRegsEVSYS_EVGENs[] = {
    ZeroRegsEVSYS_EVGEN00, ZeroRegsEVSYS_EVGEN01, ZeroRegsEVSYS_EVGEN02, ZeroRegsEVSYS_EVGEN03, ZeroRegsEVSYS_EVGEN04, ZeroRegsEVSYS_EVGEN05, ZeroRegsEVSYS_EVGEN06, ZeroRegsEVSYS_EVGEN07, ZeroRegsEVSYS_EVGEN08, ZeroRegsEVSYS_EVGEN09, ZeroRegsEVSYS_EVGEN0A, ZeroRegsEVSYS_EVGEN0B, ZeroRegsEVSYS_EVGEN0C, ZeroRegsEVSYS_EVGEN0D, ZeroRegsEVSYS_EVGEN0E, ZeroRegsEVSYS_EVGEN0F ,
    ZeroRegsEVSYS_EVGEN10, ZeroRegsEVSYS_EVGEN11, ZeroRegsEVSYS_EVGEN12, ZeroRegsEVSYS_EVGEN13, ZeroRegsEVSYS_EVGEN14, ZeroRegsEVSYS_EVGEN15, ZeroRegsEVSYS_EVGEN16, ZeroRegsEVSYS_EVGEN17, ZeroRegsEVSYS_EVGEN18, ZeroRegsEVSYS_EVGEN19, ZeroRegsEVSYS_EVGEN1A, ZeroRegsEVSYS_EVGEN1B, ZeroRegsEVSYS_EVGEN1C, ZeroRegsEVSYS_EVGEN1D, ZeroRegsEVSYS_EVGEN1E, ZeroRegsEVSYS_EVGEN1F,
    ZeroRegsEVSYS_EVGEN20, ZeroRegsEVSYS_EVGEN21, ZeroRegsEVSYS_EVGEN22, ZeroRegsEVSYS_EVGEN23, ZeroRegsEVSYS_EVGEN24, ZeroRegsEVSYS_EVGEN25, ZeroRegsEVSYS_EVGEN26, ZeroRegsEVSYS_EVGEN27, ZeroRegsEVSYS_EVGEN28, ZeroRegsEVSYS_EVGEN29, ZeroRegsEVSYS_EVGEN2A, ZeroRegsEVSYS_EVGEN2B, ZeroRegsEVSYS_EVGEN2C, ZeroRegsEVSYS_EVGEN2D, ZeroRegsEVSYS_EVGEN2E, ZeroRegsEVSYS_EVGEN2F,
    ZeroRegsEVSYS_EVGEN30, ZeroRegsEVSYS_EVGEN31, ZeroRegsEVSYS_EVGEN32, ZeroRegsEVSYS_EVGEN33, ZeroRegsEVSYS_EVGEN34, ZeroRegsEVSYS_EVGEN35, ZeroRegsEVSYS_EVGEN36, ZeroRegsEVSYS_EVGEN37, ZeroRegsEVSYS_EVGEN38, ZeroRegsEVSYS_EVGEN39, ZeroRegsEVSYS_EVGEN3A, ZeroRegsEVSYS_EVGEN3B, ZeroRegsEVSYS_EVGEN3C, ZeroRegsEVSYS_EVGEN3D, ZeroRegsEVSYS_EVGEN3E, ZeroRegsEVSYS_EVGEN3F,
    ZeroRegsEVSYS_EVGEN40, ZeroRegsEVSYS_EVGEN41, ZeroRegsEVSYS_EVGEN42, ZeroRegsEVSYS_EVGEN43, ZeroRegsEVSYS_EVGEN44, ZeroRegsEVSYS_EVGEN45, ZeroRegsEVSYS_EVGEN46, ZeroRegsEVSYS_EVGEN47, ZeroRegsEVSYS_EVGEN48, ZeroRegsEVSYS_EVGEN49, ZeroRegsEVSYS_EVGEN4A, ZeroRegsEVSYS_EVGEN4B, ZeroRegsEVSYS_EVGEN4C, ZeroRegsEVSYS_EVGEN4D, ZeroRegsEVSYS_EVGEN4E, ZeroRegsEVSYS_EVGEN4F,
    ZeroRegsEVSYS_EVGEN50, ZeroRegsEVSYS_EVGEN51, ZeroRegsEVSYS_EVGEN52, ZeroRegsEVSYS_EVGEN53,
};
static const char* ZeroRegsEVSYS_USER00 = "DMAC:0";
static const char* ZeroRegsEVSYS_USER01 = "DMAC:1";
static const char* ZeroRegsEVSYS_USER02 = "DMAC:2";
static const char* ZeroRegsEVSYS_USER03 = "DMAC:3";
static const char* ZeroRegsEVSYS_USER04 = "TCC0:EV0";
static const char* ZeroRegsEVSYS_USER05 = "TCC0:EV1";
static const char* ZeroRegsEVSYS_USER06 = "TCC0:MC0";
static const char* ZeroRegsEVSYS_USER07 = "TCC0:MC1";
static const char* ZeroRegsEVSYS_USER08 = "TCC0:MC2";
static const char* ZeroRegsEVSYS_USER09 = "TCC0:MC3";
static const char* ZeroRegsEVSYS_USER0A = "TCC1:EV0";
static const char* ZeroRegsEVSYS_USER0B = "TCC1:EV1";
static const char* ZeroRegsEVSYS_USER0C = "TCC1:MC0";
static const char* ZeroRegsEVSYS_USER0D = "TCC1:MC1";
static const char* ZeroRegsEVSYS_USER0E = "TCC2:EV0";
static const char* ZeroRegsEVSYS_USER0F = "TCC2:EV1";
static const char* ZeroRegsEVSYS_USER10 = "TCC2:MC0";
static const char* ZeroRegsEVSYS_USER11 = "TCC2:MC1";
static const char* ZeroRegsEVSYS_USER12 = "TC3";
static const char* ZeroRegsEVSYS_USER13 = "TC4";
static const char* ZeroRegsEVSYS_USER14 = "TC5";
static const char* ZeroRegsEVSYS_USER15 = "TC6";
static const char* ZeroRegsEVSYS_USER16 = "TC7";
static const char* ZeroRegsEVSYS_USER17 = "ADC:START";
static const char* ZeroRegsEVSYS_USER18 = "ADC:SYNC";
static const char* ZeroRegsEVSYS_USER19 = "AC:COMP0";
static const char* ZeroRegsEVSYS_USER1A = "AC:COMP1";
static const char* ZeroRegsEVSYS_USER1B = "DAC:START";
static const char* ZeroRegsEVSYS_USER1C = "PTC:STCONV";
static const char* ZeroRegsEVSYS_USER1D = "AC:COMP2";
static const char* ZeroRegsEVSYS_USER1E = "AC:COMP3";
static const char* ZeroRegsEVSYS_USER1F = "TCC3:EV0";
static const char* ZeroRegsEVSYS_USER20 = "TCC3:EV1";
static const char* ZeroRegsEVSYS_USER21 = "TCC3:MC0";
static const char* ZeroRegsEVSYS_USER22 = "TCC3:MC1";
static const char* ZeroRegsEVSYS_USER23 = "TCC3:MC2";
static const char* ZeroRegsEVSYS_USER24 = "TCC3:MC3";
static const char* ZeroRegsEVSYS_USERs[] = {
    ZeroRegsEVSYS_USER00, ZeroRegsEVSYS_USER01, ZeroRegsEVSYS_USER02, ZeroRegsEVSYS_USER03, ZeroRegsEVSYS_USER04, ZeroRegsEVSYS_USER05, ZeroRegsEVSYS_USER06, ZeroRegsEVSYS_USER07, ZeroRegsEVSYS_USER08, ZeroRegsEVSYS_USER09, ZeroRegsEVSYS_USER0A, ZeroRegsEVSYS_USER0B, ZeroRegsEVSYS_USER0C, ZeroRegsEVSYS_USER0D, ZeroRegsEVSYS_USER0E, ZeroRegsEVSYS_USER0F ,
    ZeroRegsEVSYS_USER10, ZeroRegsEVSYS_USER11, ZeroRegsEVSYS_USER12, ZeroRegsEVSYS_USER13, ZeroRegsEVSYS_USER14, ZeroRegsEVSYS_USER15, ZeroRegsEVSYS_USER16, ZeroRegsEVSYS_USER17, ZeroRegsEVSYS_USER18, ZeroRegsEVSYS_USER19, ZeroRegsEVSYS_USER1A, ZeroRegsEVSYS_USER1B, ZeroRegsEVSYS_USER1C, ZeroRegsEVSYS_USER1D, ZeroRegsEVSYS_USER1E, ZeroRegsEVSYS_USER1F,
    ZeroRegsEVSYS_USER20, ZeroRegsEVSYS_USER21, ZeroRegsEVSYS_USER22, ZeroRegsEVSYS_USER23, ZeroRegsEVSYS_USER24,
};
void printZeroRegEVSYS(ZeroRegOptions &opts) {
    while (EVSYS->CTRL.bit.SWRST) {}
    opts.out.println("--------------------------- EVSYS");

    opts.out.print("CTRL: ");
    PRINTFLAG(EVSYS->CTRL, GCLKREQ);
    PRINTNL();

    for (uint8_t chid = 0; chid < 12; chid++) {
        // [24.6.2.4 DSrevF] It is possible to read out the configuration of a
        // channel by first selecting the channel by writing to CHANNEL.CHANNEL
        // using a, 8-bit write, and then performing a read of the CHANNEL
        // register.
        WRITE8(EVSYS->CHANNEL.reg, chid);
        //FUTURE -- better way to wait until write has synchronized
        delay(1);
        if (!EVSYS->CHANNEL.bit.EVGEN && !opts.showDisabled) {
            continue;
        }
        if (ZeroRegsEVSYS_EVGENs[EVSYS->CHANNEL.bit.EVGEN] == ZeroRegs__RESERVED) {
            continue;
        }
        opts.out.print("CHANNEL");
        PRINTPAD2(chid);
        opts.out.print(": ");
        opts.out.print(" path=");
        switch (EVSYS->CHANNEL.bit.PATH) {
            case 0x0: opts.out.print("SYNC"); break;
            case 0x1: opts.out.print("RESYNC"); break;
            case 0x2: opts.out.print("ASYNC"); break;
            default: opts.out.print(ZeroRegs__RESERVED); break;
        }
        opts.out.print(" edgsel");
        switch (EVSYS->CHANNEL.bit.EDGSEL) {
            case 0x0: opts.out.print("NONE"); break;
            case 0x1: opts.out.print("RISE"); break;
            case 0x2: opts.out.print("FALL"); break;
            case 0x3: opts.out.print("BOTH"); break;
        }
        opts.out.print(" evgen=");
        opts.out.print(ZeroRegsEVSYS_EVGENs[EVSYS->CHANNEL.bit.EVGEN]);
        PRINTNL();
    }

    for (uint8_t uid = 0; uid <= 0x24; uid++) {
        // [24.6.2.3 DSrevF] It is possible to read out the configuration of a
        // user by first selecting the user by writing to USER.USER using an
        // 8-bit write and then performing a read of the 16-bit USER register.
        WRITE8(EVSYS->USER.reg, uid);
        //FUTURE -- better way to wait until write has synchronized
        delay(1);
        if (!EVSYS->USER.bit.CHANNEL && !opts.showDisabled) {
            continue;
        }
        opts.out.print("USER");
        PRINTPAD2(uid);
        opts.out.print(":  ");
        opts.out.print(ZeroRegsEVSYS_USERs[uid]);
        if (ZeroRegsEVSYS_USERs[uid] == ZeroRegs__RESERVED) {
            PRINTNL();
            continue;
        }
        if (EVSYS->USER.bit.CHANNEL == 0) {
            opts.out.print(" ");
            opts.out.println(ZeroRegs__DISABLED);
        } else {
            opts.out.print(" CHANNEL=");
            PRINTPAD2(EVSYS->USER.bit.CHANNEL - 1);
            PRINTNL();
        }
    }
}


static const char ZeroRegsGCLK_NAME00[] = "DFLL48M_REF";
static const char ZeroRegsGCLK_NAME01[] = "DPLL";
static const char ZeroRegsGCLK_NAME02[] = "DPLL_32K";
static const char ZeroRegsGCLK_NAME03[] = "WDT";
static const char ZeroRegsGCLK_NAME04[] = "RTC";
static const char ZeroRegsGCLK_NAME05[] = "EIC";
static const char ZeroRegsGCLK_NAME06[] = "USB";
static const char ZeroRegsGCLK_NAME07[] = "EVSYS_CHANNEL_0";
static const char ZeroRegsGCLK_NAME08[] = "EVSYS_CHANNEL_1";
static const char ZeroRegsGCLK_NAME09[] = "EVSYS_CHANNEL_2";
static const char ZeroRegsGCLK_NAME0A[] = "EVSYS_CHANNEL_3";
static const char ZeroRegsGCLK_NAME0B[] = "EVSYS_CHANNEL_4";
static const char ZeroRegsGCLK_NAME0C[] = "EVSYS_CHANNEL_5";
static const char ZeroRegsGCLK_NAME0D[] = "EVSYS_CHANNEL_6";
static const char ZeroRegsGCLK_NAME0E[] = "EVSYS_CHANNEL_7";
static const char ZeroRegsGCLK_NAME0F[] = "EVSYS_CHANNEL_8";
static const char ZeroRegsGCLK_NAME10[] = "EVSYS_CHANNEL_9";
static const char ZeroRegsGCLK_NAME11[] = "EVSYS_CHANNEL_10";
static const char ZeroRegsGCLK_NAME12[] = "EVSYS_CHANNEL_11";
static const char ZeroRegsGCLK_NAME13[] = "SERCOMx_SLOW";
static const char ZeroRegsGCLK_NAME14[] = "SERCOM0_CORE";
static const char ZeroRegsGCLK_NAME15[] = "SERCOM1_CORE";
static const char ZeroRegsGCLK_NAME16[] = "SERCOM2_CORE";
static const char ZeroRegsGCLK_NAME17[] = "SERCOM3_CORE";
static const char ZeroRegsGCLK_NAME18[] = "SERCOM4_CORE";
static const char ZeroRegsGCLK_NAME19[] = "SERCOM5_CORE";
static const char ZeroRegsGCLK_NAME1A[] = "TCC0_TCC1";
static const char ZeroRegsGCLK_NAME1B[] = "TCC2_TC3";
static const char ZeroRegsGCLK_NAME1C[] = "TC4_TC5";
static const char ZeroRegsGCLK_NAME1D[] = "TC6_TC7";
static const char ZeroRegsGCLK_NAME1E[] = "ADC";
static const char ZeroRegsGCLK_NAME1F[] = "AC_DIG";
static const char ZeroRegsGCLK_NAME20[] = "AC_ANA";
static const char ZeroRegsGCLK_NAME21[] = "DAC";
static const char ZeroRegsGCLK_NAME22[] = "PTC";
static const char ZeroRegsGCLK_NAME23[] = "I2S_0";
static const char ZeroRegsGCLK_NAME24[] = "I2S_1";
static const char ZeroRegsGCLK_NAME25[] = "TCC3";
static const char* const ZeroRegsGCLK_NAMEs[] = {
    ZeroRegsGCLK_NAME00, ZeroRegsGCLK_NAME01, ZeroRegsGCLK_NAME02, ZeroRegsGCLK_NAME03, ZeroRegsGCLK_NAME04, ZeroRegsGCLK_NAME05, ZeroRegsGCLK_NAME06, ZeroRegsGCLK_NAME07,
    ZeroRegsGCLK_NAME08, ZeroRegsGCLK_NAME09, ZeroRegsGCLK_NAME0A, ZeroRegsGCLK_NAME0B, ZeroRegsGCLK_NAME0C, ZeroRegsGCLK_NAME0D, ZeroRegsGCLK_NAME0E, ZeroRegsGCLK_NAME0F,
    ZeroRegsGCLK_NAME10, ZeroRegsGCLK_NAME11, ZeroRegsGCLK_NAME12, ZeroRegsGCLK_NAME13, ZeroRegsGCLK_NAME14, ZeroRegsGCLK_NAME15, ZeroRegsGCLK_NAME16, ZeroRegsGCLK_NAME17,
    ZeroRegsGCLK_NAME18, ZeroRegsGCLK_NAME19, ZeroRegsGCLK_NAME1A, ZeroRegsGCLK_NAME1B, ZeroRegsGCLK_NAME1C, ZeroRegsGCLK_NAME1D, ZeroRegsGCLK_NAME1E, ZeroRegsGCLK_NAME1F,
    ZeroRegsGCLK_NAME20, ZeroRegsGCLK_NAME21, ZeroRegsGCLK_NAME22, ZeroRegsGCLK_NAME23, ZeroRegsGCLK_NAME24, ZeroRegsGCLK_NAME25,
};
void printZeroRegGCLK(ZeroRegOptions &opts) {
    while (GCLK->CTRL.bit.SWRST || GCLK->STATUS.bit.SYNCBUSY) {}
    opts.out.println("--------------------------- GCLK");

    for (uint8_t genid = 0; genid < 0x9; genid++) {
        // [15.6.4.1 DSrevF] Indirect Access
        WRITE8(GCLK->GENCTRL.reg, genid);
        //FUTURE -- better way to wait until write has synchronized
        delay(1);
        if (!GCLK->GENCTRL.bit.GENEN && !opts.showDisabled) {
            continue;
        }
        opts.out.print("GEN");
        PRINTPAD2(genid);
        opts.out.print(": ");
        PRINTFLAG(GCLK->GENCTRL, GENEN);
        opts.out.print(" ");
        switch (GCLK->GENCTRL.bit.SRC) {
            case 0x0: opts.out.print("XOSC"); break;
            case 0x1:
                      //FUTURE -- lookup port pin(s)
                      opts.out.print("GCLKIN");
                      break;
            case 0x2: opts.out.print("GEN01"); break;
            case 0x3: opts.out.print("OSCULP32K"); break;
            case 0x4: opts.out.print("OSC32K"); break;
            case 0x5: opts.out.print("XOSC32K"); break;
            case 0x6: opts.out.print("OSC8M"); break;
            case 0x7: opts.out.print("DFLL48M"); break;
            case 0x8: opts.out.print("FDPLL96M"); break;
            default: opts.out.print(ZeroRegs__RESERVED); break;
        }
        // [15.6.4.1 DSrevF] Indirect Access
        WRITE8(GCLK->GENDIV.reg, genid);
        //FUTURE -- better way to wait until write has synchronized
        delay(1);
        if (GCLK->GENCTRL.bit.DIVSEL) {
            opts.out.print("/");
            PRINTSCALE(GCLK->GENDIV.bit.DIV + 1);
        } else {
            if (GCLK->GENDIV.bit.DIV > 1) {
                opts.out.print("/");
                opts.out.print(GCLK->GENDIV.bit.DIV);
            }
        }
        PRINTFLAG(GCLK->GENCTRL, IDC);
        if (GCLK->GENCTRL.bit.SRC != 1) {
            PRINTFLAG(GCLK->GENCTRL, OE);   //FUTURE -- lookup port pin(s)
            if (GCLK->GENCTRL.bit.OE) {
                opts.out.print(" OOV=");
                opts.out.print(GCLK->GENCTRL.bit.OOV);
            }
        }
        PRINTFLAG(GCLK->GENCTRL, RUNSTDBY);
        PRINTNL();
    }

    opts.out.println("GCLK_MAIN:  GEN00 (always)");
    for (uint8_t gclkid = 0; gclkid < 37; gclkid++) {
        // [15.6.4.1 DSrevF] Indirect Access
        WRITE8(GCLK->CLKCTRL.reg, gclkid);
        //FUTURE -- better way to wait until write has synchronized
        delay(1);
        if (!GCLK->CLKCTRL.bit.CLKEN && !opts.showDisabled) {
            continue;
        }
        opts.out.print("GCLK_");
        opts.out.print(ZeroRegsGCLK_NAMEs[gclkid]);
        opts.out.print(": ");
        PRINTFLAG(GCLK->CLKCTRL, CLKEN);
        opts.out.print(" GEN");
        PRINTPAD2(GCLK->CLKCTRL.bit.GEN);
        PRINTFLAG(GCLK->CLKCTRL, WRTLOCK);
        PRINTNL();
    }
}


void printZeroRegI2S(ZeroRegOptions &opts) {
#ifdef I2S
    //FUTURE -- print I2S
#else
    //FUTURE -- print message about missing I2S
#endif
}


typedef union {
    struct {
        uint64_t    :26;
        uint64_t    ADC_LINEARITY:8;
        uint64_t    ADC_BIAS:3;
        uint64_t    OSC32K_CAL:7;
        uint64_t    USB_TRANSN:5;
        uint64_t    USB_TRANSP:5;
        uint64_t    USB_TRIM:3;
        uint64_t    DFLL48M_COARSE_CAL:6;
    } bit;
    uint64_t reg;
} ZeroRegsNVM_OTP4_Type;
void printZeroRegNVMCTRL(ZeroRegOptions &opts) {
    opts.out.println("--------------------------- NVMCTRL");

    opts.out.print("CTRLB: ");
    opts.out.print(" RWS=");
    opts.out.print(NVMCTRL->CTRLB.bit.RWS);
    PRINTFLAG(NVMCTRL->CTRLB, MANW);
    opts.out.print(" sleepprm=");
    switch (NVMCTRL->CTRLB.bit.SLEEPPRM) {
        case 0x0: opts.out.print("WAKEONACCESS"); break;
        case 0x1: opts.out.print("WAKEUPINSTANT"); break;
        case 0x2: opts.out.print(ZeroRegs__RESERVED); break;
        case 0x3: opts.out.print("DISABLED"); break;
    }
    opts.out.print(" readmode=");
    switch (NVMCTRL->CTRLB.bit.READMODE) {
        case 0x0: opts.out.print("NO_MISS_PENALTY"); break;
        case 0x1: opts.out.print("LOW_POWER"); break;
        case 0x2: opts.out.print("DETERMINISTIC"); break;
        default: opts.out.print(ZeroRegs__RESERVED); break;
    }
    PRINTFLAG(NVMCTRL->CTRLB, CACHEDIS);
    PRINTNL();

    opts.out.print("PARAM:  NVMP=");
    opts.out.print(NVMCTRL->PARAM.bit.NVMP);
    opts.out.print(" psz=");
    opts.out.print(1 << (3 + NVMCTRL->PARAM.bit.PSZ));
    opts.out.print("bytes");
    PRINTNL();

    opts.out.print("LOCK:  ");
    opts.out.println(NVMCTRL->LOCK.reg, BIN);

    // [10.3.1 DSrevF] NVM User Row Mapping
    // [22.6.5 DSrevF] NVM User Configuration
    opts.out.print("user row: ");
    opts.out.print(" bootprot=");
    switch (READFUSE(NVMCTRL, BOOTPROT)) {
        case 0x7: opts.out.print("0"); break;
        case 0x6: opts.out.print("512"); break;
        case 0x5: opts.out.print("1k"); break;
        case 0x4: opts.out.print("2k"); break;
        case 0x3: opts.out.print("4k"); break;
        case 0x2: opts.out.print("8k"); break;
        case 0x1: opts.out.print("16k"); break;
        case 0x0: opts.out.print("32k"); break;
    }
    opts.out.print(" eeprom_size=");
    switch (READFUSE(NVMCTRL, EEPROM_SIZE)) {
        case 0x7: opts.out.print("0"); break;
        case 0x6: opts.out.print("256"); break;
        case 0x5: opts.out.print("512"); break;
        case 0x4: opts.out.print("1k"); break;
        case 0x3: opts.out.print("2k"); break;
        case 0x2: opts.out.print("4k"); break;
        case 0x1: opts.out.print("8k"); break;
        case 0x0: opts.out.print("16k"); break;
    }
    opts.out.print(" region_locks=");
    opts.out.print(READFUSE(NVMCTRL, REGION_LOCKS), BIN);
    PRINTNL();

    // [10.3.2 DSrevF] NVM Software Calibration Area Mapping
    //  0x806020 NVMCTRL_OTP4
    ZeroRegsNVM_OTP4_Type otp4;
    otp4.reg = ((ZeroRegsNVM_OTP4_Type*)NVMCTRL_OTP4)->reg;
    opts.out.print("software calibration: ");
    opts.out.print(" ADC_LINEARITY=");
    PRINTHEX((uint8_t)otp4.bit.ADC_LINEARITY);
    opts.out.print(" ADC_BIAS=");
    PRINTHEX((uint8_t)otp4.bit.ADC_BIAS);
    opts.out.print(" OSC32K_CAL=");
    PRINTHEX((uint8_t)otp4.bit.OSC32K_CAL);
    opts.out.print(" USB_TRANSN=");
    PRINTHEX((uint8_t)otp4.bit.USB_TRANSN);
    opts.out.print(" USB_TRANSP=");
    PRINTHEX((uint8_t)otp4.bit.USB_TRANSP);
    opts.out.print(" USB_TRIM=");
    PRINTHEX((uint8_t)otp4.bit.USB_TRIM);
    opts.out.print(" DFLL48M_COARSE_CAL=");
    PRINTHEX((uint8_t)otp4.bit.DFLL48M_COARSE_CAL);
    PRINTNL();

    // [10.3.3 DSrevF] Serial Number
    // 0x0080A00C 0x0080A040 0x0080A044 0x0080A048
    opts.out.print("serial # ");
    PRINTHEX(READADDR32(0x0080A00C));
    opts.out.print(" ");
    PRINTHEX(READADDR32(0x0080A040));
    opts.out.print(" ");
    PRINTHEX(READADDR32(0x0080A044));
    opts.out.print(" ");
    PRINTHEX(READADDR32(0x0080A048));
    PRINTNL();
}


void printZeroRegPAC(ZeroRegOptions &opts) {
    if (!PAC0->WPSET.reg && !PAC1->WPSET.reg && !PAC2->WPSET.reg && !opts.showDisabled) {
        return;
    }
    opts.out.println("--------------------------- PAC");

    if (PAC0->WPSET.reg || opts.showDisabled) {
        opts.out.print("PAC0: ");
        if (bitRead(PAC0->WPSET.reg, 1)) opts.out.print(" PM");
        if (bitRead(PAC0->WPSET.reg, 2)) opts.out.print(" SYSCTRL");
        if (bitRead(PAC0->WPSET.reg, 3)) opts.out.print(" GCLK");
        if (bitRead(PAC0->WPSET.reg, 4)) opts.out.print(" WDT");
        if (bitRead(PAC0->WPSET.reg, 5)) opts.out.print(" RTC");
        if (bitRead(PAC0->WPSET.reg, 6)) opts.out.print(" EIC");
        PRINTNL();
    }

    if (PAC1->WPSET.reg || opts.showDisabled) {
        opts.out.print("PAC1: ");
        if (bitRead(PAC1->WPSET.reg, 1)) opts.out.print(" DSU");
        if (bitRead(PAC1->WPSET.reg, 2)) opts.out.print(" NVMCTRL");
        if (bitRead(PAC1->WPSET.reg, 3)) opts.out.print(" PORT");
        if (bitRead(PAC1->WPSET.reg, 4)) opts.out.print(" DMAC");
        if (bitRead(PAC1->WPSET.reg, 5)) opts.out.print(" USB");
        if (bitRead(PAC1->WPSET.reg, 6)) opts.out.print(" MTB");
        PRINTNL();
    }

    if (PAC2->WPSET.reg || opts.showDisabled) {
        opts.out.print("PAC2: ");
        if (bitRead(PAC2->WPSET.reg,  1)) opts.out.print(" EVSYS");
        if (bitRead(PAC2->WPSET.reg,  2)) opts.out.print(" SERCOM0");
        if (bitRead(PAC2->WPSET.reg,  3)) opts.out.print(" SERCOM1");
        if (bitRead(PAC2->WPSET.reg,  4)) opts.out.print(" SERCOM2");
        if (bitRead(PAC2->WPSET.reg,  5)) opts.out.print(" SERCOM3");
        if (bitRead(PAC2->WPSET.reg,  6)) opts.out.print(" SERCOM4");
        if (bitRead(PAC2->WPSET.reg,  7)) opts.out.print(" SERCOM5");
        if (bitRead(PAC2->WPSET.reg,  8)) opts.out.print(" TCC0");
        if (bitRead(PAC2->WPSET.reg,  9)) opts.out.print(" TCC1");
        if (bitRead(PAC2->WPSET.reg, 10)) opts.out.print(" TCC2");
        if (bitRead(PAC2->WPSET.reg, 11)) opts.out.print(" TC3");
        if (bitRead(PAC2->WPSET.reg, 12)) opts.out.print(" TC4");
        if (bitRead(PAC2->WPSET.reg, 13)) opts.out.print(" TC5");
        if (bitRead(PAC2->WPSET.reg, 14)) opts.out.print(" TC6");
        if (bitRead(PAC2->WPSET.reg, 15)) opts.out.print(" TC7");
        if (bitRead(PAC2->WPSET.reg, 16)) opts.out.print(" ADC");
        if (bitRead(PAC2->WPSET.reg, 17)) opts.out.print(" AC");
        if (bitRead(PAC2->WPSET.reg, 18)) opts.out.print(" DAC");
        if (bitRead(PAC2->WPSET.reg, 19)) opts.out.print(" PTC");
        if (bitRead(PAC2->WPSET.reg, 20)) opts.out.print(" I2S");
        if (bitRead(PAC2->WPSET.reg, 21)) opts.out.print(" AC1");
        // undefined                 22
        // undefined                 23
        if (bitRead(PAC2->WPSET.reg, 24)) opts.out.print(" TCC3");
        PRINTNL();
    }
}


void printZeroRegPM(ZeroRegOptions &opts) {
    opts.out.println("--------------------------- PM");

    opts.out.print("SLEEP:  idle=");
    switch(PM->SLEEP.bit.IDLE) {
        case 0x0: opts.out.print("CPU"); break;
        case 0x1: opts.out.print("CPU+AHB"); break;
        case 0x2: opts.out.print("CPU+AHB+APB"); break;
        default: opts.out.print(ZeroRegs__RESERVED); break;
    }
    PRINTNL();

    opts.out.print("CPUSEL:  /");
    PRINTSCALE(PM->CPUSEL.bit.CPUDIV);
    PRINTNL();

    opts.out.print("APBASEL:  /");
    PRINTSCALE(PM->APBASEL.bit.APBADIV);
    PRINTNL();

    opts.out.print("APBBSEL:  /");
    PRINTSCALE(PM->APBBSEL.bit.APBBDIV);
    PRINTNL();

    opts.out.print("APBCSEL:  /");
    PRINTSCALE(PM->APBCSEL.bit.APBCDIV);
    PRINTNL();

    opts.out.print("AHBMASK: ");
    if (PM->AHBMASK.bit.HPB0_)      { opts.out.print(" CLK_HPBA_AHB"); }
    if (PM->AHBMASK.bit.HPB1_)      { opts.out.print(" CLK_HPBB_AHB"); }
    if (PM->AHBMASK.bit.HPB2_)      { opts.out.print(" CLK_HPBC_AHB"); }
    if (PM->AHBMASK.bit.DSU_)       { opts.out.print(" CLK_DSU_AHB"); }
    if (PM->AHBMASK.bit.NVMCTRL_)   { opts.out.print(" CLK_NVMCTRL_AHB"); }
    if (PM->AHBMASK.bit.DMAC_)      { opts.out.print(" CLK_DMAC_AHB"); }
    if (PM->AHBMASK.bit.USB_)       { opts.out.print(" CLK_USB_AHB"); }
    PRINTNL();

    opts.out.print("APBAMASK: ");
    if (PM->APBAMASK.bit.PAC0_)     { opts.out.print(" CLK_PAC0_APB"); }
    if (PM->APBAMASK.bit.PM_)       { opts.out.print(" CLK_PM_APB"); }
    if (PM->APBAMASK.bit.SYSCTRL_)  { opts.out.print(" CLK_SYSCTRL_APB"); }
    if (PM->APBAMASK.bit.GCLK_)     { opts.out.print(" CLK_GCLK_APB"); }
    if (PM->APBAMASK.bit.WDT_)      { opts.out.print(" CLK_WDT_APB"); }
    if (PM->APBAMASK.bit.RTC_)      { opts.out.print(" CLK_RTC_APB"); }
    if (PM->APBAMASK.bit.EIC_)      { opts.out.print(" CLK_EIC_APB"); }
    PRINTNL();

    opts.out.print("APBBMASK: ");
    if (PM->APBBMASK.bit.PAC1_)     { opts.out.print(" CLK_PAC1_APB"); }
    if (PM->APBBMASK.bit.DSU_)      { opts.out.print(" CLK_DSU_APB"); }
    if (PM->APBBMASK.bit.NVMCTRL_)  { opts.out.print(" CLK_NVMCTRL_APB"); }
    if (PM->APBBMASK.bit.PORT_)     { opts.out.print(" CLK_PORT_APB"); }
    if (PM->APBBMASK.bit.DMAC_)     { opts.out.print(" CLK_DMAC_APB"); }
    if (PM->APBBMASK.bit.USB_)      { opts.out.print(" CLK_USB_APB"); }
    PRINTNL();

    opts.out.print("APBCMASK: ");
    if (PM->APBCMASK.bit.PAC2_)     { opts.out.print(" CLK_PAC2_APB"); }
    if (PM->APBCMASK.bit.EVSYS_)    { opts.out.print(" CLK_EVSYS_APB"); }
    if (PM->APBCMASK.bit.SERCOM0_)  { opts.out.print(" CLK_SERCOM0_APB"); }
    if (PM->APBCMASK.bit.SERCOM1_)  { opts.out.print(" CLK_SERCOM1_APB"); }
    if (PM->APBCMASK.bit.SERCOM2_)  { opts.out.print(" CLK_SERCOM2_APB"); }
    if (PM->APBCMASK.bit.SERCOM3_)  { opts.out.print(" CLK_SERCOM3_APB"); }
    if (PM->APBCMASK.bit.SERCOM4_)  { opts.out.print(" CLK_SERCOM4_APB"); }
    if (PM->APBCMASK.bit.SERCOM5_)  { opts.out.print(" CLK_SERCOM5_APB"); }
    if (PM->APBCMASK.bit.TCC0_)     { opts.out.print(" CLK_TCC0_APB"); }
    if (PM->APBCMASK.bit.TCC1_)     { opts.out.print(" CLK_TCC1_APB"); }
    if (PM->APBCMASK.bit.TCC2_)     { opts.out.print(" CLK_TCC2_APB"); }
    if (PM->APBCMASK.bit.TC3_)      { opts.out.print(" CLK_TC3_APB"); }
    if (PM->APBCMASK.bit.TC4_)      { opts.out.print(" CLK_TC4_APB"); }
    if (PM->APBCMASK.bit.TC5_)      { opts.out.print(" CLK_TC5_APB"); }
    if (PM->APBCMASK.bit.TC6_)      { opts.out.print(" CLK_TC6_APB"); }
    if (PM->APBCMASK.bit.TC7_)      { opts.out.print(" CLK_TC7_APB"); }
    if (PM->APBCMASK.bit.ADC_)      { opts.out.print(" CLK_ADC_APB"); }
    if (PM->APBCMASK.bit.AC_)       { opts.out.print(" CLK_AC_APB"); }
    if (PM->APBCMASK.bit.DAC_)      { opts.out.print(" CLK_DAC_APB"); }
    if (PM->APBCMASK.bit.PTC_)      { opts.out.print(" CLK_PTC_APB"); }
    if (PM->APBCMASK.bit.I2S_)      { opts.out.print(" CLK_I2S_APB"); }
    if (bitRead(PM->APBCMASK.reg, 21)) opts.out.print(" CLK_AC1_APB");
    // undefined                  22
    // undefined                  23
    if (bitRead(PM->APBCMASK.reg, 24)) opts.out.print(" CLK_TCC3_APB");
    PRINTNL();
}


// [table 7-1 DSrevF]
struct ZeroRegsPORT_PMUX {
    char        id;
    const char* name;
};
static const ZeroRegsPORT_PMUX ZeroRegsPORT_PMUXs[8] = {
    { 'A', "EIC" },
    { 'B', "REF,ADC,AC,PTC,DAC" },  // analog stuff
    { 'C', "SERCOM" },
    { 'D', "SERCOM-ALT" },
    { 'E', "TC,TCC" },
    { 'F', "TCC" },
    { 'G', "COM" },
    { 'H', "AC,GCLK" },
};
struct ZeroRegsPORT_Pin {
    const char* name;
    const char* pmux[8];    // see PMUX names above
};
static const ZeroRegsPORT_Pin ZeroRegsPORT_pins[2][32] = {
    {
        { "PA00", {   "EIC:0",                           NULL,        NULL, "SERCOM1:0", "TCC2:0",     NULL,        NULL,     NULL } },
        { "PA01", {   "EIC:1",                           NULL,        NULL, "SERCOM1:1", "TCC2:1",     NULL,        NULL,     NULL } },
        { "PA02", {   "EIC:2",            "ADC:0,Y0,DAC:VOUT",        NULL,        NULL,     NULL, "TCC3:0",        NULL,     NULL } },
        { "PA03", {   "EIC:3", "ADC:VREFA,DAC:VREFA,ADC:1,Y1",        NULL,        NULL,     NULL, "TCC3:1",        NULL,     NULL } },
        { "PA04", {   "EIC:4",      "ADC:VREFB,ADC:4,AC:0,Y2",        NULL, "SERCOM0:0", "TCC0:0", "TCC3:2",        NULL,     NULL } },
        { "PA05", {   "EIC:5",                "ADC:5,AC:1,Y3",        NULL, "SERCOM0:1", "TCC0:1", "TCC3:3",        NULL,     NULL } },
        { "PA06", {   "EIC:6",                "ADC:6,AC:2,Y4",        NULL, "SERCOM0:2", "TCC1:0", "TCC3:4",        NULL,     NULL } },
        { "PA07", {   "EIC:7",                "ADC:7,AC:3,Y5",        NULL, "SERCOM0:3", "TCC1:1", "TCC3:5",   "I2S:SD0",     NULL } },
        { "PA08", { "EIC:NMI",                    "ADC:16,X0", "SERCOM0:0", "SERCOM2:0", "TCC0:0", "TCC1:2",   "I2S:SD1",     NULL } },
        { "PA09", {   "EIC:9",                    "ADC:17,X1", "SERCOM0:1", "SERCOM2:1", "TCC0:1", "TCC1:3",  "I2S:MCK0",     NULL } },
        { "PA10", {  "EIC:10",                    "ADC:18,X2", "SERCOM0:2", "SERCOM2:2", "TCC1:0", "TCC0:2",  "I2S:SCK0", "GCLK:4" } },
        { "PA11", {  "EIC:11",                    "ADC:19,X3", "SERCOM0:3", "SERCOM2:3", "TCC1:1", "TCC0:3",   "I2S:FS0", "GCLK:5" } },
        { "PA12", {  "EIC:12",                           NULL, "SERCOM2:0", "SERCOM4:0", "TCC2:0", "TCC0:6",        NULL,   "AC:0" } },
        { "PA13", {  "EIC:13",                           NULL, "SERCOM2:1", "SERCOM4:1", "TCC2:1", "TCC0:7",        NULL,   "AC:1" } },
        { "PA14", {  "EIC:14",                           NULL, "SERCOM2:2", "SERCOM4:2",  "TC3:0", "TCC0:4",        NULL, "GCLK:0" } },
        { "PA15", {  "EIC:15",                           NULL, "SERCOM2:3", "SERCOM4:3",  "TC3:1", "TCC0:5",        NULL, "GCLK:1" } },
        { "PA16", {   "EIC:0",                           "X4", "SERCOM1:0", "SERCOM3:0", "TCC2:0", "TCC0:6",        NULL, "GCLK:2" } },
        { "PA17", {   "EIC:1",                           "X5", "SERCOM1:1", "SERCOM3:1", "TCC2:1", "TCC0:7",        NULL, "GCLK:3" } },
        { "PA18", {   "EIC:2",                           "X6", "SERCOM1:2", "SERCOM3:2",  "TC3:0", "TCC0:2",        NULL,   "AC:0" } },
        { "PA19", {   "EIC:3",                           "X7", "SERCOM1:3", "SERCOM3:3",  "TC3:1", "TCC0:3",   "I2S:SD0",   "AC:1" } },
        { "PA20", {   "EIC:4",                           "X8", "SERCOM5:2", "SERCOM3:2",  "TC7:0", "TCC0:6",  "I2S:SCK0", "GCLK:4" } },
        { "PA21", {   "EIC:5",                           "X9", "SERCOM5:3", "SERCOM3:3",  "TC7:1", "TCC0:7",   "I2S:FS0", "GCLK:5" } },
        { "PA22", {   "EIC:6",                          "X10", "SERCOM3:0", "SERCOM5:0",  "TC4:0", "TCC0:4",        NULL, "GCLK:6" } },
        { "PA23", {   "EIC:7",                          "X11", "SERCOM3:1", "SERCOM5:1",  "TC4:1", "TCC0:5", "USB:SOF1K", "GCLK:7" } },
        { "PA24", {  "EIC:12",                           NULL, "SERCOM3:2", "SERCOM5:2",  "TC5:0", "TCC1:2",    "USB:DN",     NULL } },
        { "PA25", {  "EIC:13",                           NULL, "SERCOM3:3", "SERCOM5:3",  "TC5:1", "TCC1:3",    "USB:DP",     NULL } },
        {/*PA26*/NULL,{  NULL,                           NULL,        NULL,        NULL,     NULL,     NULL,        NULL,     NULL } },
        { "PA27", {  "EIC:15",                           NULL,        NULL,        NULL,     NULL, "TCC3:6",        NULL, "GCLK:0" } },
        { "PA28", {   "EIC:8",                           NULL,        NULL,        NULL,     NULL, "TCC3:7",        NULL, "GCLK:0" } },
        {/*PA29*/NULL,{  NULL,                           NULL,        NULL,        NULL,     NULL,     NULL,        NULL,     NULL } },
        { "PA30", {  "EIC:10",                           NULL,        NULL, "SERCOM1:2", "TCC1:0", "TCC3:4",     "SWCLK", "GCLK:0" } },
        { "PA31", {  "EIC:11",                           NULL,        NULL, "SERCOM1:3", "TCC1:1", "TCC3:5",     "SWDIO",     NULL } },
    },
    {
        { "PB00", {   "EIC:0",   "ADC:8,Y6",        NULL, "SERCOM5:2",  "TC7:0",     NULL,       NULL,     NULL } },
        { "PB01", {   "EIC:1",   "ADC:9,Y7",        NULL, "SERCOM5:3",  "TC7:1",     NULL,       NULL,     NULL } },
        { "PB02", {   "EIC:2",  "ADC:10,Y8",        NULL, "SERCOM5:0",  "TC6:0", "TCC3:2",       NULL,     NULL } },
        { "PB03", {   "EIC:3",  "ADC:11,Y9",        NULL, "SERCOM5:1",  "TC6:1", "TCC3:3",       NULL,     NULL } },
        { "PB04", {   "EIC:4", "ADC:12,Y10",        NULL,        NULL,     NULL,     NULL,       NULL,     NULL } },
        { "PB05", {   "EIC:5", "ADC:13,Y11",        NULL,        NULL,     NULL,     NULL,       NULL,     NULL } },
        { "PB06", {   "EIC:6", "ADC:14,Y12",        NULL,        NULL,     NULL,     NULL,       NULL,     NULL } },
        { "PB07", {   "EIC:7", "ADC:15,Y13",        NULL,        NULL,     NULL,     NULL,       NULL,     NULL } },
        { "PB08", {   "EIC:8",  "ADC:2,Y14",        NULL, "SERCOM4:0",  "TC4:0", "TCC3:6",       NULL,     NULL } },
        { "PB09", {   "EIC:9",  "ADC:3,Y15",        NULL, "SERCOM4:1",  "TC4:1", "TCC3:7",       NULL,     NULL } },
        { "PB10", {  "EIC:10",         NULL,        NULL, "SERCOM4:2",  "TC5:0", "TCC0:4", "I2S:MCK1", "GCLK:4" } },
        { "PB11", {  "EIC:11",         NULL,        NULL, "SERCOM4:3",  "TC5:1", "TCC0:5", "I2S:SCK1", "GCLK:5" } },
        { "PB12", {  "EIC:12",        "X12", "SERCOM4:0",        NULL,  "TC4:0", "TCC0:6",  "I2S:FS1", "GCLK:6" } },
        { "PB13", {  "EIC:13",        "X13", "SERCOM4:1",        NULL,  "TC4:1", "TCC0:7",       NULL, "GCLK:7" } },
        { "PB14", {  "EIC:14",        "X14", "SERCOM4:2",        NULL,  "TC5:0",     NULL,       NULL, "GCLK:0" } },
        { "PB15", {  "EIC:15",        "X15", "SERCOM4:3",        NULL,  "TC5:1",     NULL,       NULL, "GCLK:1" } },
        { "PB16", {   "EIC:0",         NULL, "SERCOM5:0",        NULL,  "TC6:0", "TCC0:4",  "I2S:SD1", "GCLK:2" } },
        { "PB17", {   "EIC:1",         NULL, "SERCOM5:1",        NULL,  "TC6:1", "TCC0:5", "I2S:MCK0", "GCLK:3" } },
        {/*PB18*/NULL,{  NULL,         NULL,        NULL,        NULL,     NULL,     NULL,       NULL,     NULL } },
        {/*PB19*/NULL,{  NULL,         NULL,        NULL,        NULL,     NULL,     NULL,       NULL,     NULL } },
        {/*PB20*/NULL,{  NULL,         NULL,        NULL,        NULL,     NULL,     NULL,       NULL,     NULL } },
        {/*PB21*/NULL,{  NULL,         NULL,        NULL,        NULL,     NULL,     NULL,       NULL,     NULL } },
        { "PB22", {   "EIC:6",         NULL,        NULL, "SERCOM5:2",  "TC7:0", "TCC3:0",       NULL, "GCLK:0" } },
        { "PB23", {   "EIC:7",         NULL,        NULL, "SERCOM5:3",  "TC7:1", "TCC3:1",       NULL, "GCLK:1" } },
        {/*PB24*/NULL,{  NULL,         NULL,        NULL,        NULL,     NULL,     NULL,       NULL,     NULL } },
        {/*PB25*/NULL,{  NULL,         NULL,        NULL,        NULL,     NULL,     NULL,       NULL,     NULL } },
        {/*PB26*/NULL,{  NULL,         NULL,        NULL,        NULL,     NULL,     NULL,       NULL,     NULL } },
        {/*PB27*/NULL,{  NULL,         NULL,        NULL,        NULL,     NULL,     NULL,       NULL,     NULL } },
        {/*PB28*/NULL,{  NULL,         NULL,        NULL,        NULL,     NULL,     NULL,       NULL,     NULL } },
        {/*PB29*/NULL,{  NULL,         NULL,        NULL,        NULL,     NULL,     NULL,       NULL,     NULL } },
        { "PB30", {  "EIC:14",         NULL,        NULL, "SERCOM5:0", "TCC0:0", "TCC1:2",       NULL,     NULL } },
        { "PB31", {  "EIC:15",         NULL,        NULL, "SERCOM5:1", "TCC0:1", "TCC1:3",       NULL,     NULL } },
    },
};
void printZeroRegPORT_pin(ZeroRegOptions &opts, uint8_t gid, uint8_t pid) {
    uint32_t dir = (PORT->Group[gid].DIR.bit.DIR & (1 << pid));
    uint8_t inen = PORT->Group[gid].PINCFG[pid].bit.INEN;
    uint8_t pullen = PORT->Group[gid].PINCFG[pid].bit.PULLEN;
    uint8_t pmuxen = PORT->Group[gid].PINCFG[pid].bit.PMUXEN;
    if (pmuxen) {
        uint8_t pmux;
        if ((pid%2) == 0) {
            pmux = PORT->Group[gid].PMUX[pid/2].bit.PMUXE;
        } else {
            pmux = PORT->Group[gid].PMUX[pid/2].bit.PMUXO;
        }
        opts.out.print("pmux=");
        const char *pmuxName = ZeroRegsPORT_pins[gid][pid].pmux[pmux];
        if (!pmuxName) {
            pmuxName = ZeroRegsPORT_PMUXs[pmux].name;
        }
        opts.out.print(pmuxName);
        if (!dir && !inen && !pullen) {
            // [23.6.3.4 DSrevF] Digital Functionality Disabled
            return;
        }
        opts.out.print(" ");
    }
    if (dir) {
        opts.out.print("output");
        if (inen) {
            opts.out.print(" INEN");
        }
        PRINTFLAG(PORT->Group[gid].PINCFG[pid], DRVSTR);
    } else {
        opts.out.print("input");
        if (inen) {
            opts.out.print(" INEN");
        }
        if (PORT->Group[gid].CTRL.bit.SAMPLING & (1<<pid)) {
            opts.out.print(" SAMPLING");
        }
        if (pullen) {
            opts.out.print(" pull=");
            opts.out.print(
                    PORT->Group[gid].OUT.bit.OUT & (1<<pid) ?
                    "UP" : "DOWN"
                    );
        }
    }
}

void printZeroRegPORT(ZeroRegOptions &opts) {
    for (uint8_t gid = 0; gid < 2; gid++) {
        opts.out.print("--------------------------- PORT ");
        opts.out.print(char('A' + gid));
        PRINTNL();

        for (uint8_t pid = 0; pid < 32; pid++) {
            if (!ZeroRegsPORT_pins[gid][pid].name) {
                // pin not defined in datasheet [table 7-1 DSrevF]
                continue;
            }
            uint32_t dir = (PORT->Group[gid].DIR.bit.DIR & (1 << pid));
            uint8_t inen = PORT->Group[gid].PINCFG[pid].bit.INEN;
            uint8_t pullen = PORT->Group[gid].PINCFG[pid].bit.PULLEN;
            uint8_t pmuxen = PORT->Group[gid].PINCFG[pid].bit.PMUXEN;
            bool disabled = !dir && !inen && !pullen && !pmuxen;    // [23.6.3.4 DSrevF] Digital Functionality Disabled
            if (disabled && !opts.showDisabled) {
                continue;
            }
            opts.out.print(ZeroRegsPORT_pins[gid][pid].name);
            opts.out.print(":  ");
            printZeroRegPORT_pin(opts, gid, pid);
            PRINTNL();
        }
    }
}


void printZeroRegPORT_Arduino(ZeroRegOptions &opts) {
    opts.out.println("--------------------------- ARDUINO PINS");
    uint8_t aid = 0;
    for (uint8_t did = 0; did < PINS_COUNT; did++) {
        PinDescription pinDesc = g_APinDescription[did];
        uint8_t gid = pinDesc.ulPort;
        uint8_t pid = pinDesc.ulPin;
        if (gid == NOT_A_PORT || pinDesc.ulPinType == PIO_NOT_A_PIN) {
            continue;
        }
        if (pinDesc.ulPinType == PIO_ANALOG) {
            opts.out.print('A');
            opts.out.print(aid);
            aid++;
        } else if (pinDesc.ulPinAttribute & PIN_ATTR_DIGITAL) {
            opts.out.print('D');
            opts.out.print(did);
        } else {
            opts.out.print(ZeroRegsPORT_pins[gid][pid].name);
        }
        opts.out.print(":  ");
        uint32_t dir = (PORT->Group[gid].DIR.bit.DIR & (1 << pid));
        uint8_t inen = PORT->Group[gid].PINCFG[pid].bit.INEN;
        uint8_t pullen = PORT->Group[gid].PINCFG[pid].bit.PULLEN;
        uint8_t pmuxen = PORT->Group[gid].PINCFG[pid].bit.PMUXEN;
        bool disabled = !dir && !inen && !pullen && !pmuxen;    // [23.6.3.4 DSrevF] Digital Functionality Disabled
        if (disabled && !opts.showDisabled) {
            opts.out.println(ZeroRegs__DISABLED);
        } else {
            printZeroRegPORT_pin(opts, gid, pid);
            PRINTNL();
        }
    }
}


void printZeroRegRTC_FREQCORR(ZeroRegOptions &opts, volatile RTC_FREQCORR_Type &freqcorr) {
    opts.out.print("FREQCORR:  ");
    opts.out.print(freqcorr.bit.SIGN ? '-' : '+');
    opts.out.println(freqcorr.bit.VALUE);
}

void printZeroRegRTC_MODE0(ZeroRegOptions &opts, RtcMode0 &mode) {
    opts.out.println("--------------------------- RTC MODE0");

    opts.out.print("CTRL: ");
    PRINTFLAG(mode.CTRL, ENABLE);
    PRINTFLAG(mode.CTRL, MATCHCLR);
    opts.out.print(" PRESCALER=");
    PRINTHEX(mode.CTRL.bit.PRESCALER);
    opts.out.print("(GCLK_RTC/");
    PRINTSCALE(mode.CTRL.bit.PRESCALER);
    opts.out.print(")");
    PRINTNL();

    opts.out.print("EVCTRL: ");
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
    PRINTNL();

    printZeroRegRTC_FREQCORR(opts, mode.FREQCORR);

    opts.out.print("COMP0:  ");
    PRINTHEX(mode.COMP[0].reg);
    PRINTNL();
}

void printZeroRegRTC_MODE1(ZeroRegOptions &opts, RtcMode1 &mode) {
    opts.out.println("--------------------------- RTC MODE1");

    opts.out.print("CTRL: ");
    PRINTFLAG(mode.CTRL, ENABLE);
    opts.out.print(" PRESCALER=");
    PRINTHEX(mode.CTRL.bit.PRESCALER);
    opts.out.print("(GCLK_RTC/");
    PRINTSCALE(mode.CTRL.bit.PRESCALER);
    opts.out.print(")");
    PRINTNL();

    opts.out.print("EVCTRL: ");
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
    PRINTNL();

    printZeroRegRTC_FREQCORR(opts, mode.FREQCORR);

    opts.out.print("PER:  ");
    PRINTHEX(mode.PER.bit.PER);
    PRINTNL();

    opts.out.print("COMP0:  ");
    PRINTHEX(mode.COMP[0].reg);
    PRINTNL();

    opts.out.print("COMP1:  ");
    PRINTHEX(mode.COMP[1].reg);
    PRINTNL();
}

void printZeroRegRTC_MODE2(ZeroRegOptions &opts, RtcMode2 &mode) {
    opts.out.println("--------------------------- RTC MODE2");

    opts.out.print("CTRL: ");
    PRINTFLAG(mode.CTRL, ENABLE);
    PRINTFLAG(mode.CTRL, CLKREP);
    PRINTFLAG(mode.CTRL, MATCHCLR);
    opts.out.print(" PRESCALER=");
    PRINTHEX(mode.CTRL.bit.PRESCALER);
    opts.out.print("(GCLK_RTC/");
    PRINTSCALE(mode.CTRL.bit.PRESCALER);
    opts.out.print(")");
    PRINTNL();

    opts.out.print("EVCTRL: ");
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
    PRINTNL();

    printZeroRegRTC_FREQCORR(opts, mode.FREQCORR);

    opts.out.print("ALARM0:  ");
    PRINTPAD2(mode.Mode2Alarm[0].ALARM.bit.YEAR);
    opts.out.print("-");
    PRINTPAD2(mode.Mode2Alarm[0].ALARM.bit.MONTH);
    opts.out.print("-");
    PRINTPAD2(mode.Mode2Alarm[0].ALARM.bit.DAY);
    opts.out.print(" ");
    PRINTPAD2(mode.Mode2Alarm[0].ALARM.bit.HOUR);
    opts.out.print(":");
    PRINTPAD2(mode.Mode2Alarm[0].ALARM.bit.MINUTE);
    opts.out.print(":");
    PRINTPAD2(mode.Mode2Alarm[0].ALARM.bit.SECOND);
    PRINTNL();

    opts.out.print("MASK0:  ");
    switch (mode.Mode2Alarm[0].MASK.bit.SEL) {
        case 0x0: opts.out.print("OFF"); break;
        case 0x1: opts.out.print("SS"); break;
        case 0x2: opts.out.print("MM:SS"); break;
        case 0x3: opts.out.print("HH:MM:SS"); break;
        case 0x4: opts.out.print("DD HH:MM:SS"); break;
        case 0x5: opts.out.print("MM-DD HH:MM:SS"); break;
        case 0x6: opts.out.print("YY-MM-DD HH:MM:SS"); break;
        default: opts.out.print(ZeroRegs__RESERVED); break;
    }
    PRINTNL();
}

void printZeroRegRTC(ZeroRegOptions &opts) {
    while (RTC->MODE0.CTRL.bit.SWRST || RTC->MODE0.STATUS.bit.SYNCBUSY) {}
    if (!opts.showDisabled && !RTC->MODE0.CTRL.bit.ENABLE) {
        return;
    }
    switch (RTC->MODE0.CTRL.bit.MODE) {
        case 0x0: printZeroRegRTC_MODE0(opts, RTC->MODE0); break;
        case 0x1: printZeroRegRTC_MODE1(opts, RTC->MODE1); break;
        case 0x2: printZeroRegRTC_MODE2(opts, RTC->MODE2); break;
    }
}


void printZeroRegSCS(ZeroRegOptions &opts) {
    opts.out.println("--------------------------- SCS");

    opts.out.print("CPUID:  REV=");
    PRINTHEX(READSCS(SCB->CPUID, SCB_CPUID_REVISION));
    opts.out.print(" PARTNO=");
    PRINTHEX(READSCS(SCB->CPUID, SCB_CPUID_PARTNO));
    opts.out.print(" ARCH=");
    PRINTHEX(READSCS(SCB->CPUID, SCB_CPUID_ARCHITECTURE));
    opts.out.print(" VAR=");
    PRINTHEX(READSCS(SCB->CPUID, SCB_CPUID_VARIANT));
    opts.out.print(" IMPL=");
    PRINTHEX(READSCS(SCB->CPUID, SCB_CPUID_IMPLEMENTER));
    PRINTNL();

    opts.out.print("SysTick: ");
    if (READSCS(SysTick->CTRL, SysTick_CTRL_ENABLE)) {
        opts.out.print(" ENABLE");
    }
    if (READSCS(SysTick->CTRL, SysTick_CTRL_TICKINT)) {
        opts.out.print(" TICKINT");
    }
    opts.out.print(" CLKSOURCE=");
    opts.out.print(READSCS(SysTick->CTRL, SysTick_CTRL_CLKSOURCE) ? "CPU" : "EXT");
    opts.out.print(" RELOAD=");
    opts.out.print(READSCS(SysTick->LOAD, SysTick_LOAD_RELOAD));
    opts.out.print(" TENMS=");
    opts.out.print(READSCS(SysTick->CALIB, SysTick_CALIB_TENMS));
    if (READSCS(SysTick->CALIB, SysTick_CALIB_SKEW)) {
        opts.out.print(" SKEW");
    }
    if (READSCS(SysTick->CALIB, SysTick_CALIB_NOREF)) {
        opts.out.print(" NOREF");
    }
    PRINTNL();
    for (uint8_t pri = 0; pri < 4; pri++) {
        opts.out.print("irq pri");
        opts.out.print(pri);
        opts.out.print(": ");
        for (uint8_t irq = 0; irq < PERIPH_COUNT_IRQn; irq++) {
            if (pri == NVIC_GetPriority((IRQn_Type) irq)) {
                switch (irq) {
                    case 0: opts.out.print(" PM"); break;
                    case 1: opts.out.print(" SYSCTRL"); break;
                    case 2: opts.out.print(" WDT"); break;
                    case 3: opts.out.print(" RTC"); break;
                    case 4: opts.out.print(" EIC"); break;
                    case 5: opts.out.print(" NVMCTRL"); break;
                    case 6: opts.out.print(" DMAC"); break;
                    case 7: opts.out.print(" USB"); break;
                    case 8: opts.out.print(" EVSYS"); break;
                    case 9: opts.out.print(" SERCOM0"); break;
                    case 10: opts.out.print(" SERCOM1"); break;
                    case 11: opts.out.print(" SERCOM2"); break;
                    case 12: opts.out.print(" SERCOM3"); break;
                    case 13: opts.out.print(" SERCOM4"); break;
                    case 14: opts.out.print(" SERCOM5"); break;
                    case 15: opts.out.print(" TCC0"); break;
                    case 16: opts.out.print(" TCC1"); break;
                    case 17: opts.out.print(" TCC2"); break;
                    case 18: opts.out.print(" TC3"); break;
                    case 19: opts.out.print(" TC4"); break;
                    case 20: opts.out.print(" TC5"); break;
                    case 21: opts.out.print(" TC6"); break;
                    case 22: opts.out.print(" TC7"); break;
                    case 23: opts.out.print(" ADC"); break;
                    case 24: opts.out.print(" AC"); break;
                    case 25: opts.out.print(" DAC"); break;
                    case 26: opts.out.print(" PTC"); break;
                    case 27: opts.out.print(" I2S"); break;
                    default: PRINTHEX(irq); break;
                }
            }
        }
        PRINTNL();
    }
}


void printZeroRegSERCOM_I2CM(ZeroRegOptions &opts, SercomI2cm &i2cm) {
    opts.out.print("CTRLA: ");
    PRINTFLAG(i2cm.CTRLA, ENABLE);
    PRINTFLAG(i2cm.CTRLA, RUNSTDBY);
    PRINTFLAG(i2cm.CTRLA, PINOUT);
    opts.out.print(" sdahold=");
    switch (i2cm.CTRLA.bit.SDAHOLD) {
        case 0x0: opts.out.print("DIS"); break;
        case 0x1: opts.out.print("75NS"); break;
        case 0x2: opts.out.print("450NS"); break;
        case 0x3: opts.out.print("600NS"); break;
    }
    PRINTFLAG(i2cm.CTRLA, MEXTTOEN);
    PRINTFLAG(i2cm.CTRLA, SEXTTOEN);
    opts.out.print(" speed=");
    switch (i2cm.CTRLA.bit.SPEED) {
        case 0x0: opts.out.print("SM<100kHz,FM<400kHz"); break;
        case 0x1: opts.out.print("FM+<1MHz"); break;
        case 0x2: opts.out.print("HS<3.4MHz"); break;
        default: opts.out.print(ZeroRegs__RESERVED); break;
    }
    PRINTFLAG(i2cm.CTRLA, SCLSM);
    opts.out.print(" inactout=");
    switch (i2cm.CTRLA.bit.INACTOUT) {
        case 0x0: opts.out.print("DIS"); break;
        case 0x1: opts.out.print("55NS"); break;
        case 0x2: opts.out.print("105NS"); break;
        case 0x3: opts.out.print("205NS"); break;
    }
    PRINTFLAG(i2cm.CTRLA, LOWTOUTEN);
    PRINTNL();

    opts.out.print("CTRLB: ");
    PRINTFLAG(i2cm.CTRLB, QCEN);
    PRINTFLAG(i2cm.CTRLB, SMEN);
    opts.out.print(" ackact=");
    opts.out.print(i2cm.CTRLB.bit.ACKACT ? "NACK" : "ACK");
    PRINTNL();

    opts.out.print("BAUD: ");
    opts.out.print(" BAUD=");
    PRINTHEX(i2cm.BAUD.bit.BAUD);
    opts.out.print(" BAUDLOW=");
    PRINTHEX(i2cm.BAUD.bit.BAUDLOW);
    opts.out.print(" HSBAUD=");
    PRINTHEX(i2cm.BAUD.bit.HSBAUD);
    opts.out.print(" HSBAUDLOW=");
    PRINTHEX(i2cm.BAUD.bit.HSBAUDLOW);
    PRINTNL();

    opts.out.print("ADDR: ");
    opts.out.print(" ADDR=");
    PRINTHEX(i2cm.ADDR.bit.ADDR);
    PRINTFLAG(i2cm.ADDR, LENEN);
    PRINTFLAG(i2cm.ADDR, HS);
    PRINTFLAG(i2cm.ADDR, TENBITEN);
    opts.out.print(" LEN=");
    PRINTHEX(i2cm.ADDR.bit.LEN);
    PRINTNL();
}

void printZeroRegSERCOM_I2CS(ZeroRegOptions &opts, SercomI2cs &i2cs) {
    opts.out.print("CTRLA: ");
    PRINTFLAG(i2cs.CTRLA, ENABLE);
    PRINTFLAG(i2cs.CTRLA, RUNSTDBY);
    PRINTFLAG(i2cs.CTRLA, PINOUT);
    opts.out.print(" sdahold=");
    switch (i2cs.CTRLA.bit.SDAHOLD) {
        case 0x0: opts.out.print("DIS"); break;
        case 0x1: opts.out.print("75NS"); break;
        case 0x2: opts.out.print("450NS"); break;
        case 0x3: opts.out.print("600NS"); break;
    }
    PRINTFLAG(i2cs.CTRLA, SEXTTOEN);
    opts.out.print(" speed=");
    switch (i2cs.CTRLA.bit.SPEED) {
        case 0x0: opts.out.print("SM<100kHz,FM<400kHz"); break;
        case 0x1: opts.out.print("FM+<1MHz"); break;
        case 0x2: opts.out.print("HS<3.4MHz"); break;
        default: opts.out.print(ZeroRegs__RESERVED); break;
    }
    PRINTFLAG(i2cs.CTRLA, SCLSM);
    PRINTFLAG(i2cs.CTRLA, LOWTOUTEN);
    PRINTNL();

    opts.out.print("CTRLB: ");
    PRINTFLAG(i2cs.CTRLB, SMEN);
    PRINTFLAG(i2cs.CTRLB, GCMD);
    PRINTFLAG(i2cs.CTRLB, AACKEN);
    opts.out.print(" amode=");
    switch (i2cs.CTRLB.bit.AMODE) {
        case 0x0: opts.out.print("MASK"); break;
        case 0x1: opts.out.print("2ADDRS"); break;
        case 0x2: opts.out.print("RANGE"); break;
        default: opts.out.print(ZeroRegs__RESERVED); break;
    }
    opts.out.print(" ackact=");
    opts.out.print(i2cs.CTRLB.bit.ACKACT ? "NACK" : "ACK");
    PRINTNL();

    opts.out.print("ADDR: ");
    PRINTFLAG(i2cs.ADDR, GENCEN);
    opts.out.print(" ADDR=");
    PRINTHEX(i2cs.ADDR.bit.ADDR);
    PRINTFLAG(i2cs.ADDR, TENBITEN);
    opts.out.print(" ADDRMASK=");
    PRINTHEX(i2cs.ADDR.bit.ADDRMASK);
    PRINTNL();
}

void printZeroRegSERCOM_SPI(ZeroRegOptions &opts, SercomSpi &spi, bool master) {
    opts.out.print("CTRLA: ");
    PRINTFLAG(spi.CTRLA, ENABLE);
    PRINTFLAG(spi.CTRLA, RUNSTDBY);
    PRINTFLAG(spi.CTRLA, IBON);
    opts.out.print(master ? " miso=" : " mosi=");
    switch (spi.CTRLA.bit.DIPO) {
        case 0x0: opts.out.print("PAD0"); break;
        case 0x1: opts.out.print("PAD1"); break;
        case 0x2: opts.out.print("PAD2"); break;
        case 0x3: opts.out.print("PAD3"); break;
    }
    opts.out.print(master ? " mosi=" : " miso=");
    switch (spi.CTRLA.bit.DOPO) {
        case 0x0:
            opts.out.print("PAD0 sck=PAD1");
            if (!master || spi.CTRLB.bit.MSSEN) {
                opts.out.print(" ss=PAD2");
            }
            break;
        case 0x1:
            opts.out.print("PAD2 sck=PAD3");
            if (!master || spi.CTRLB.bit.MSSEN) {
                opts.out.print(" ss=PAD1");
            }
            break;
        case 0x2:
            opts.out.print("PAD3 sck=PAD1");
            if (!master || spi.CTRLB.bit.MSSEN) {
                opts.out.print(" ss=PAD2");
            }
            break;
        case 0x3:
            opts.out.print("PAD0 sck=PAD3");
            if (!master || spi.CTRLB.bit.MSSEN) {
                opts.out.print(" ss=PAD1");
            }
            break;
    }
    opts.out.print(" form=");
    switch (spi.CTRLA.bit.FORM) {
        case 0x0: opts.out.print("SPI"); break;
        /*   0x1 */
        case 0x2: opts.out.print("SPI_ADDR"); break;
        default: opts.out.print(ZeroRegs__RESERVED); break;
    }
    opts.out.print(" cpha=");
    opts.out.print(spi.CTRLA.bit.CPHA ? "TRAILING" : "LEADING");
    opts.out.print(" cpol=");
    opts.out.print(spi.CTRLA.bit.CPOL ? "HIGH" : "LOW");
    opts.out.print(" dord=");
    opts.out.print(spi.CTRLA.bit.DORD ? "LSB" : "MSB");
    PRINTNL();

    while (spi.SYNCBUSY.bit.CTRLB) {}
    opts.out.print("CTRLB: ");
    opts.out.print(" chsize=");
    switch (spi.CTRLB.bit.CHSIZE) {
        case 0x0: opts.out.print("8bit"); break;
        case 0x1: opts.out.print("9bit"); break;
        default: opts.out.print(ZeroRegs__RESERVED); break;
    }
    PRINTFLAG(spi.CTRLB, PLOADEN);
    PRINTFLAG(spi.CTRLB, SSDE);
    PRINTFLAG(spi.CTRLB, MSSEN);
    opts.out.print(" amode=");
    switch (spi.CTRLB.bit.AMODE) {
        case 0x0: opts.out.print("MASK"); break;
        case 0x1: opts.out.print("2ADDRS"); break;
        case 0x2: opts.out.print("RANGE"); break;
        default: opts.out.print(ZeroRegs__RESERVED); break;
    }
    PRINTFLAG(spi.CTRLB, RXEN);
    PRINTNL();

    opts.out.print("BAUD:  ");
    PRINTHEX(spi.BAUD.reg);
    PRINTNL();

    if (spi.CTRLA.bit.FORM == 0x2) {
        opts.out.print("ADDR: ");
        opts.out.print(" ADDR=");
        PRINTHEX(spi.ADDR.bit.ADDR);
        opts.out.print(" ADDRMASK=");
        PRINTHEX(spi.ADDR.bit.ADDRMASK);
        PRINTNL();
    }
}

void printZeroRegSERCOM_USART(ZeroRegOptions &opts, SercomUsart &usart) {
    opts.out.print("CTRLA: ");
    PRINTFLAG(usart.CTRLA, ENABLE);
    PRINTFLAG(usart.CTRLA, RUNSTDBY);
    PRINTFLAG(usart.CTRLA, IBON);
    opts.out.print(" SAMPR=");
    PRINTHEX(usart.CTRLA.bit.SAMPR);
    opts.out.print(" SAMPA=");
    PRINTHEX(usart.CTRLA.bit.SAMPA);
    opts.out.print(" cmode=");
    opts.out.print(usart.CTRLA.bit.CMODE ? "SYNC" : "ASYNC");
    opts.out.print(" cpol=");
    opts.out.print(usart.CTRLA.bit.CPOL ? "FALLING" : "RISING");
    opts.out.print(" dord=");
    opts.out.print(usart.CTRLA.bit.CPOL ? "LSB" : "MSB");
    opts.out.print(" form=");
    switch(usart.CTRLA.bit.FORM) {
        case 0x0: opts.out.print("USART"); break;
        case 0x1: opts.out.print("USART+PARITY"); break;
        /*   0x2 */
        /*   0x3 */
        case 0x4: opts.out.print("AUTOBAUD"); break;
        case 0x5: opts.out.print("AUTOBAUD+PARITY"); break;
        default: opts.out.print(ZeroRegs__RESERVED); break;
    }
    switch (usart.CTRLA.bit.RXPO) {
        case 0x0: opts.out.print(" rx=PAD0"); break;
        case 0x1: opts.out.print(" rx=PAD1"); break;
        case 0x2: opts.out.print(" rx=PAD2"); break;
        case 0x3: opts.out.print(" rx=PAD3"); break;
    }
    switch (usart.CTRLA.bit.TXPO) {
        case 0x0:
            opts.out.print(" tx=PAD0");
            if (usart.CTRLA.bit.MODE == 0) opts.out.print(" xck=PAD1");
            break;
        case 0x1:
            opts.out.print(" tx=PAD2");
            if (usart.CTRLA.bit.MODE == 0) opts.out.print(" xck=PAD3");
            break;
        case 0x2:
            opts.out.print(" tx=PAD0 rts=PAD2 cts=PAD3");
            break;
        default: opts.out.print(ZeroRegs__RESERVED); break;
    }
    PRINTNL();

    while (usart.SYNCBUSY.bit.CTRLB) {}
    opts.out.print("CTRLB:  chsize=");
    switch (usart.CTRLB.bit.CHSIZE) {
        case 0x0: opts.out.print("8bit"); break;
        case 0x1: opts.out.print("9bit"); break;
        /*   0x2 */
        /*   0x3 */
        /*   0x4 */
        case 0x5: opts.out.print("5bit"); break;
        case 0x6: opts.out.print("6bit"); break;
        case 0x7: opts.out.print("7bit"); break;
        default: opts.out.print(ZeroRegs__RESERVED); break;
    }
    PRINTFLAG(usart.CTRLB, SBMODE);
    PRINTFLAG(usart.CTRLB, COLDEN);
    PRINTFLAG(usart.CTRLB, SFDE);
    PRINTFLAG(usart.CTRLB, ENC);
    if (usart.CTRLA.bit.FORM == 1) {
        opts.out.print(" pmode=");
        opts.out.print(usart.CTRLB.bit.PMODE ? "ODD" : "EVEN");
    }
    PRINTFLAG(usart.CTRLB, TXEN);
    PRINTFLAG(usart.CTRLB, RXEN);
    PRINTNL();

    opts.out.print("BAUD:  ");
    PRINTHEX(usart.BAUD.reg);
    PRINTNL();

    if (usart.CTRLB.bit.ENC) {
        opts.out.print("RXPL:  ");
        PRINTHEX(usart.RXPL.reg);
        PRINTNL();
    }
}

void printZeroRegSERCOM(ZeroRegOptions &opts, Sercom* sercom, uint8_t idx) {
    while (sercom->I2CM.CTRLA.bit.SWRST || sercom->I2CM.SYNCBUSY.reg) {}
    if (!sercom->I2CM.CTRLA.bit.ENABLE && !opts.showDisabled) {
        return;
    }
    opts.out.print("--------------------------- SERCOM");
    opts.out.print(idx);
    switch (sercom->I2CM.CTRLA.bit.MODE) {
        case 0x0:
            opts.out.println(" USART (external clock)");
            printZeroRegSERCOM_USART(opts, sercom->USART);
            break;
        case 0x1:
            opts.out.println(" USART (internal clock)");
            printZeroRegSERCOM_USART(opts, sercom->USART);
            break;
        case 0x2:
            opts.out.println(" SPI slave");
            printZeroRegSERCOM_SPI(opts, sercom->SPI, false);
            break;
        case 0x3:
            opts.out.println(" SPI master");
            printZeroRegSERCOM_SPI(opts, sercom->SPI, true);
            break;
        case 0x4:
            opts.out.println(" I2C slave");
            printZeroRegSERCOM_I2CS(opts, sercom->I2CS);
            break;
        case 0x5:
            opts.out.println(" I2C master");
            printZeroRegSERCOM_I2CM(opts, sercom->I2CM);
            break;
    }
}


void printZeroRegSYSCTRL(ZeroRegOptions &opts) {
    opts.out.println("--------------------------- SYSCTRL");

    if (SYSCTRL->OSC32K.bit.ENABLE || opts.showDisabled) {
        opts.out.print("OSC32K: ");
        PRINTFLAG(SYSCTRL->OSC32K, ENABLE);
        PRINTFLAG(SYSCTRL->OSC32K, EN32K);
        PRINTFLAG(SYSCTRL->OSC32K, EN1K);
        PRINTFLAG(SYSCTRL->OSC32K, RUNSTDBY);
        PRINTFLAG(SYSCTRL->OSC32K, ONDEMAND);
        PRINTFLAG(SYSCTRL->OSC32K, WRTLOCK);
        opts.out.print(" STARTUP=");
        PRINTHEX(SYSCTRL->OSC32K.bit.STARTUP);
        opts.out.print(" CALIB=");
        PRINTHEX(SYSCTRL->OSC32K.bit.CALIB);
        PRINTNL();
    }

    opts.out.print("OSCULP32K:  CALIB=");
    PRINTHEX(SYSCTRL->OSCULP32K.bit.CALIB);
    PRINTFLAG(SYSCTRL->OSCULP32K, WRTLOCK);
    PRINTNL();

    if (SYSCTRL->OSC8M.bit.ENABLE || opts.showDisabled) {
        opts.out.print("OSC8M: ");
        PRINTFLAG(SYSCTRL->OSC8M, ENABLE);
        PRINTFLAG(SYSCTRL->OSC8M, RUNSTDBY);
        PRINTFLAG(SYSCTRL->OSC8M, ONDEMAND);
        opts.out.print(" presc=");
        PRINTSCALE(SYSCTRL->OSC8M.bit.PRESC);
        opts.out.print(" CALIB=");
        PRINTHEX(SYSCTRL->OSC8M.bit.CALIB);
        opts.out.print(" frange=");
        switch (SYSCTRL->OSC8M.bit.FRANGE) {
            case 0x0: opts.out.print("4-6MHz"); break;
            case 0x1: opts.out.print("6-8MHz"); break;
            case 0x2: opts.out.print("8-11MHz"); break;
            case 0x3: opts.out.print("11-15MHz"); break;
        }
        PRINTNL();
    }

    if (SYSCTRL->XOSC.bit.ENABLE || opts.showDisabled) {
        opts.out.print("XOSC: ");
        PRINTFLAG(SYSCTRL->XOSC, ENABLE);
        PRINTFLAG(SYSCTRL->XOSC, XTALEN);
        PRINTFLAG(SYSCTRL->XOSC, RUNSTDBY);
        PRINTFLAG(SYSCTRL->XOSC, ONDEMAND);
        PRINTFLAG(SYSCTRL->XOSC, AMPGC);
        opts.out.print(" gain=");
        switch (SYSCTRL->XOSC.bit.GAIN) {
            case 0x0: opts.out.print("2MHz"); break;
            case 0x1: opts.out.print("4MHz"); break;
            case 0x2: opts.out.print("8MHz"); break;
            case 0x3: opts.out.print("16MHz"); break;
            case 0x4: opts.out.print("30MHz"); break;
            default: opts.out.print(ZeroRegs__RESERVED); break;
        }
        opts.out.print(" STARTUP=");
        PRINTHEX(SYSCTRL->XOSC.bit.STARTUP);
        PRINTNL();
    }

    if (SYSCTRL->XOSC32K.bit.ENABLE || opts.showDisabled) {
        opts.out.print("XOSC32K: ");
        PRINTFLAG(SYSCTRL->XOSC32K, ENABLE);
        PRINTFLAG(SYSCTRL->XOSC32K, XTALEN);
        PRINTFLAG(SYSCTRL->XOSC32K, EN32K);
        PRINTFLAG(SYSCTRL->XOSC32K, EN1K);
        PRINTFLAG(SYSCTRL->XOSC32K, AAMPEN);
        PRINTFLAG(SYSCTRL->XOSC32K, RUNSTDBY);
        PRINTFLAG(SYSCTRL->XOSC32K, ONDEMAND);
        PRINTFLAG(SYSCTRL->XOSC32K, WRTLOCK);
        opts.out.print(" STARTUP=");
        PRINTHEX(SYSCTRL->XOSC32K.bit.STARTUP);
        PRINTNL();
    }

    if (SYSCTRL->DFLLCTRL.bit.ENABLE || opts.showDisabled) {
        opts.out.print("DFLL: ");
        PRINTFLAG(SYSCTRL->DFLLCTRL, ENABLE);
        opts.out.print(" mode=");
        opts.out.print(SYSCTRL->DFLLCTRL.bit.MODE ? "closed-loop" : "open-loop");
        PRINTFLAG(SYSCTRL->DFLLCTRL, STABLE);
        PRINTFLAG(SYSCTRL->DFLLCTRL, LLAW);
        PRINTFLAG(SYSCTRL->DFLLCTRL, USBCRM);
        PRINTFLAG(SYSCTRL->DFLLCTRL, RUNSTDBY);
        PRINTFLAG(SYSCTRL->DFLLCTRL, ONDEMAND);
        PRINTFLAG(SYSCTRL->DFLLCTRL, CCDIS);
        PRINTFLAG(SYSCTRL->DFLLCTRL, QLDIS);
        PRINTFLAG(SYSCTRL->DFLLCTRL, BPLCKC);
        PRINTFLAG(SYSCTRL->DFLLCTRL, WAITLOCK);
        if (SYSCTRL->DFLLCTRL.bit.MODE == 1) {
            opts.out.print(" MUL=");
            opts.out.print(SYSCTRL->DFLLMUL.bit.MUL);
        }
        PRINTNL();
    }

    if (SYSCTRL->DPLLCTRLA.bit.ENABLE || opts.showDisabled) {
        opts.out.print("DPLL: ");
        PRINTFLAG(SYSCTRL->DPLLCTRLA, ENABLE);
        PRINTFLAG(SYSCTRL->DPLLCTRLA, RUNSTDBY);
        PRINTFLAG(SYSCTRL->DPLLCTRLA, ONDEMAND);
        //FUTURE -- calculate actual clock speed [out = REFCLK * (LDR+1+(LDRFRAC/16))]
        opts.out.print(" refclk=");
        switch (SYSCTRL->DPLLCTRLB.bit.REFCLK) {
            case 0x0: opts.out.print("XOSC32"); break;
            case 0x1:
                      opts.out.print("XOSC/");
                      PRINTSCALE(SYSCTRL->DPLLCTRLB.bit.DIV + 1);
                      break;
            case 0x2: opts.out.print("GCLK_DPLL"); break;
            default: opts.out.print(ZeroRegs__RESERVED); break;
        }
        opts.out.print(" ldr=");
        opts.out.print(SYSCTRL->DPLLRATIO.bit.LDR);
        opts.out.print(".");
        opts.out.print(SYSCTRL->DPLLRATIO.bit.LDRFRAC);
        opts.out.print(" FILTER=");
        PRINTHEX(SYSCTRL->DPLLCTRLB.bit.FILTER);
        PRINTFLAG(SYSCTRL->DPLLCTRLB, LPEN);
        PRINTFLAG(SYSCTRL->DPLLCTRLB, WUF);
        opts.out.print(" LTIME=");
        PRINTHEX(SYSCTRL->DPLLCTRLB.bit.LTIME);
        PRINTFLAG(SYSCTRL->DPLLCTRLB, LBYPASS);
        PRINTNL();
    }

    if (SYSCTRL->BOD33.bit.ENABLE || opts.showDisabled) {
        opts.out.print("BOD33: ");
        PRINTFLAG(SYSCTRL->BOD33, ENABLE);
        PRINTFLAG(SYSCTRL->BOD33, HYST);
        PRINTFLAG(SYSCTRL->BOD33, RUNSTDBY);
        PRINTFLAG(SYSCTRL->BOD33, MODE);
        PRINTFLAG(SYSCTRL->BOD33, CEN);
        opts.out.print(" action=");
        switch (SYSCTRL->BOD33.bit.ACTION) {
            case 0x0: opts.out.print("NONE"); break;
            case 0x1: opts.out.print("RESET"); break;
            case 0x2: opts.out.print("INT"); break;
            default: opts.out.print(ZeroRegs__RESERVED); break;
        }
        opts.out.print(" psel=");
        PRINTSCALE(SYSCTRL->BOD33.bit.PSEL + 1);
        opts.out.print(" LEVEL=");
        PRINTHEX(SYSCTRL->BOD33.bit.LEVEL);
        PRINTNL();
    }

    opts.out.print("VREG: ");
    PRINTFLAG(SYSCTRL->VREG, RUNSTDBY);
    PRINTFLAG(SYSCTRL->VREG, FORCELDO);
    PRINTNL();

    opts.out.print("VREF: ");
    PRINTFLAG(SYSCTRL->VREF, TSEN);
    PRINTFLAG(SYSCTRL->VREF, BGOUTEN);
    opts.out.print(" CALIB=");
    PRINTHEX(SYSCTRL->VREF.bit.CALIB);
    PRINTNL();
}


void printZeroRegTC(ZeroRegOptions &opts, Tc* tc, uint8_t idx) {
    // FUTURE
}


void printZeroRegTCC(ZeroRegOptions &opts, Tcc* tcc, uint8_t idx) {
    // FUTURE
}


void printZeroRegUSB(ZeroRegOptions &opts) {
#ifdef USB
    //FUTURE -- print USB
#else
    //FUTURE -- print message about missing USB
#endif
}


void printZeroRegWDT(ZeroRegOptions &opts) {
    while (WDT->STATUS.bit.SYNCBUSY) {}
    if (!WDT->CTRL.bit.ENABLE || !opts.showDisabled) {
        return;
    }
    opts.out.println("--------------------------- WDT");

    opts.out.print("CTRL: ");
    PRINTFLAG(WDT->CTRL, ENABLE);
    PRINTFLAG(WDT->CTRL, WEN);
    PRINTFLAG(WDT->CTRL, ALWAYSON);
    PRINTNL();

    opts.out.print("CONFIG:  PER=");
    PRINTHEX(WDT->CONFIG.bit.PER);
    opts.out.print(" WINDOW=");
    PRINTHEX(WDT->CONFIG.bit.WINDOW);
    PRINTNL();

    opts.out.print("EWCTRL:  EWOFFSET=");
    PRINTHEX(WDT->EWCTRL.bit.EWOFFSET);
    PRINTNL();

    opts.out.print("NVM user row: ");
    opts.out.print(" ENABLE=");
    opts.out.print(READFUSE(WDT, ENABLE));
    opts.out.print(" ALWAYSON=");
    opts.out.print(READFUSE(WDT, ALWAYSON));
    opts.out.print(" PER=");
    PRINTHEX(READFUSE(WDT, PER));
    opts.out.print(" WINDOW=");
    PRINTHEX(READ2FUSES(WDT, WINDOW, 1));
    opts.out.print(" EWOFFSET=");
    PRINTHEX(READFUSE(WDT, EWOFFSET));
    opts.out.print(" WEN=");
    opts.out.print(READFUSE(WDT, WEN));
    PRINTNL();
}


void printZeroRegs(ZeroRegOptions &opts) {
    // show system basics
    printZeroRegSCS(opts);
    printZeroRegSYSCTRL(opts);
    printZeroRegGCLK(opts);

    // show core peripherals
    printZeroRegDMAC(opts);
    printZeroRegEVSYS(opts);
    printZeroRegPAC(opts);
    printZeroRegPM(opts);
    printZeroRegWDT(opts);

    // show other peripherals
    printZeroRegAC(opts);
    printZeroRegADC(opts);
    printZeroRegDAC(opts);
    printZeroRegEIC(opts);
#ifdef I2S
    printZeroRegI2S(opts);
#endif
    printZeroRegNVMCTRL(opts);
    printZeroRegPORT(opts);
    printZeroRegPORT_Arduino(opts);
    printZeroRegRTC(opts);
    printZeroRegSERCOM(opts, SERCOM0, 0);
    printZeroRegSERCOM(opts, SERCOM1, 1);
    printZeroRegSERCOM(opts, SERCOM2, 2);
    printZeroRegSERCOM(opts, SERCOM3, 3);
#ifdef SERCOM4
    printZeroRegSERCOM(opts, SERCOM4, 4);
#endif
#ifdef SERCOM5
    printZeroRegSERCOM(opts, SERCOM5, 5);
#endif
    printZeroRegTCC(opts, TCC0, 0);
    printZeroRegTCC(opts, TCC1, 1);
    printZeroRegTCC(opts, TCC2, 2);
#ifdef TCC3
    printZeroRegTCC(opts, TCC3, 3);
#endif
    printZeroRegTC(opts, TC3, 3);
    printZeroRegTC(opts, TC4, 4);
    printZeroRegTC(opts, TC5, 5);
#ifdef TC6
    printZeroRegTC(opts, TC6, 6);
#endif
#ifdef TC7
    printZeroRegTC(opts, TC7, 7);
#endif
#ifdef USB
    printZeroRegUSB(opts);
#endif
}


