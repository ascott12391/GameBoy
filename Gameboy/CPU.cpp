//
//  CPU.cpp
//  Gameboy
//
//  Created by Alex on 1/24/26.
//

#include "CPU.hpp"

reg AF{0x01, 0x00}, BC{0xFF, 0x13}, DE{0x00, 0xC1}, HL{0x84, 0x03}; //Yes, gloabals are bad. So sue me, trying to get this working at a baseline b4 Starbies closes. [asdf]
uint16_t pc = 0x0150; //Skip Boot ROM for now [asdf]
int timer = 0; //Probably wrong, fix later [asdf]
uint16_t SP = 0xFFFE;
void incPC(uint16_t count) {pc += count;}
void setPC(uint16_t location) {pc = location;}
void changeSP(int amount) {SP+=amount;}
void setSP(uint16_t location) {SP = location;} //Pretty sure this is only needed for a single opcode (0xF9), but the word 'needed' is in there
bool Z() { return AF.lo & 0x80; }
bool N() { return AF.lo & 0x40; }
bool Hc() { return AF.lo & 0x20; }
bool Fc() { return AF.lo & 0x10; }
void setZ() {AF.lo = AF.lo | 0x80; }
void setN() {AF.lo = AF.lo | 0x40; }
void setC() {AF.lo = AF.lo | 0x20; }
void setH() {AF.lo = AF.lo | 0x10; }
void zeroZ() {AF.lo = AF.lo & 0x70; }
void zeroN() {AF.lo = AF.lo & 0xB0; }
void zeroC() {AF.lo = AF.lo & 0xD0; }
void zeroH() {AF.lo = AF.lo & 0xE0; }
uint8_t& reg_ret(int r) {
    switch (r) {
        case 0: return BC.hi; // B
        case 1: return BC.lo; // C
        case 2: return DE.hi; // D
        case 3: return DE.lo; // E
        case 4: return HL.hi; // H
        case 5: return HL.lo; // L
        case 6: return AF.hi; // A
        case 7: return AF.lo; // F
        default:
            throw std::runtime_error("Invalid register");
    }
}
uint16_t readReg(int r) //Reads a full register. Come back to this w/ enums when I feel like it. [fixed]
{
    uint16_t fullReg;
    uint8_t lo;
    uint8_t hi;
    switch (r){
        case 0:
            lo = BC.lo;
            hi = BC.hi;
            break;
        case 2:
            lo = DE.lo;
            hi = DE.hi;
            break;
        case 4:
            lo = HL.lo;
            hi = HL.hi;
            break;
        case 6:
            lo = AF.lo & 0xF0; //Last 4 of F need to be 0.
            hi = AF.hi;
            break;
        default:
            throw std::runtime_error("Invalid register");
    }
    fullReg = lo;
    fullReg = fullReg | (hi<<8);
    return fullReg;
}

void writeReg(int r, uint16_t data)
{
    switch(r){
        case 0:
            BC.lo = data&0xFF;
            BC.hi = (data>>8)&0xFF;
            break;
        case 2:
            DE.lo = data&0xFF;
            DE.hi = (data>>8)&0xFF;
            break;
        case 4:
            HL.lo = data&0xFF;
            HL.hi = (data>>8)&0xFF;
            break;
        case 6:
            AF.lo = data&0xF0;
            AF.hi = (data>>8)&0xFF;
            break;
        default:
            throw std::runtime_error("Invalid register");
    }
}

void writeSmallReg(int r, uint8_t data)
{
    switch (r) {
        case 0: BC.hi = data; break; // B
        case 1: BC.lo = data; break; // C
        case 2: DE.hi = data; break; // D
        case 3: DE.lo = data; break; // E
        case 4: HL.hi = data; break; // H
        case 5: HL.lo = data; break; // L
        case 6: AF.hi = data; break; // A
        case 7: AF.lo = data; break; // F
        default:
            throw std::runtime_error("Invalid register");
    }
}

uint16_t getSP() {return SP;}
uint16_t getPC() {return pc;}
int getTimer () {return timer;}

