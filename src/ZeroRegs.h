// vim: ts=4 sts=4 sw=4 et si ai
/*
ZeroRegs.h - Library for printing the low-level SAM D21 configuration registers
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

#ifndef ZERO_REGS_H
#define ZERO_REGS_H
#include <Arduino.h>


// This is a wild experiment in API design. The idea is to make it easy to add
// new configuration options in the future without a long list of arguments.
struct ZeroRegOptions {
    Print   &out;
    bool    showDisabled;
};


void printZeroRegAC(ZeroRegOptions &opts);
void printZeroRegADC(ZeroRegOptions &opts);
void printZeroRegDAC(ZeroRegOptions &opts);
void printZeroRegDMAC(ZeroRegOptions &opts);
void printZeroRegEIC(ZeroRegOptions &opts);
void printZeroRegEVSYS(ZeroRegOptions &opts);
void printZeroRegGCLK(ZeroRegOptions &opts);
void printZeroRegI2S(ZeroRegOptions &opts);
void printZeroRegNVMCTRL(ZeroRegOptions &opts);
void printZeroRegPAC(ZeroRegOptions &opts);
void printZeroRegPM(ZeroRegOptions &opts);
void printZeroRegPORT(ZeroRegOptions &opts);
void printZeroRegPORT_Arduino(ZeroRegOptions &opts);
void printZeroRegRTC(ZeroRegOptions &opts);
void printZeroRegSCS(ZeroRegOptions &opts);
void printZeroRegSERCOM(ZeroRegOptions &opts, Sercom* sercom, uint8_t idx);
void printZeroRegSYSCTRL(ZeroRegOptions &opts);
void printZeroRegTC(ZeroRegOptions &opts, Tc* tc, uint8_t idx);
void printZeroRegTCC(ZeroRegOptions &opts, Tcc* tcc, uint8_t idx);
void printZeroRegUSB(ZeroRegOptions &opts);
void printZeroRegWDT(ZeroRegOptions &opts);
void printZeroRegs(ZeroRegOptions &opts);

#endif // ZERO_REGS_H
