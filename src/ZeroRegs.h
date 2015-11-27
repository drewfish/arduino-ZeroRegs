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

void printZeroRegAC(Stream &ser);
void printZeroRegADC(Stream &ser);
void printZeroRegDAC(Stream &ser);
void printZeroRegDMAC(Stream &ser);
void printZeroRegDSU(Stream &ser);
void printZeroRegEIC(Stream &ser);
void printZeroRegEVSYS(Stream &ser);
void printZeroRegGCLK(Stream &ser);
void printZeroRegI2S(Stream &ser);
void printZeroRegMTB(Stream &ser);
void printZeroRegNVMCTRL(Stream &ser);
void printZeroRegPACs(Stream &ser);
void printZeroRegPM(Stream &ser);
void printZeroRegPORT(Stream &ser);
void printZeroRegRTC(Stream &ser);
void printZeroRegSBMATRIX(Stream &ser);
void printZeroRegSERCOM(Stream &ser, Sercom* sercom, uint8_t idx);
void printZeroRegSYSCTRL(Stream &ser);
void printZeroRegTC(Stream &ser, Tc* tc, uint8_t idx);
void printZeroRegTCC(Stream &ser, Tcc* tcc, uint8_t idx);
void printZeroRegUSB(Stream &ser);
void printZeroRegWDT(Stream &ser);
void printZeroRegs(Stream &ser);

#endif // ZERO_REGS_H
