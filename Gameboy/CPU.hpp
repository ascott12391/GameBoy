//
//  CPU.hpp
//  Gameboy
//
//  Created by Alex on 1/24/26.
//

#ifndef CPU_hpp
#define CPU_hpp

#include <stdio.h>
#include <cstdint>
#include <stack>
struct reg
{
    uint8_t hi;
    uint8_t lo;
};
extern reg AF, BC, DE, HL; //Yes, gloabals are bad. So sue me, trying to get this working at a baseline b4 Starbies closes. [asdf]
extern uint16_t pc;
extern int timer;
enum Reg8 {B, C, D, E, H, L, A, F};
extern uint16_t SP;
void incPC(uint16_t cycles);
void setPC(uint16_t location);
uint16_t getPC();
uint16_t getSP();
void changeSP(int amount);
int getTimer();
bool Z(); //These check flags
bool N();
bool Hc();
bool Fc();
void setZ(); //Set flags
void setN();
void setC();
void setH();
void zeroZ(); //Zero flags
void zeroN();
void zeroC();
void zeroH();
uint8_t& reg_ret(int r);
uint16_t readReg(int r);
void writeReg(int r, uint16_t data);
void writeSmallReg(int r, uint8_t data);
#endif /* CPU_hpp */
