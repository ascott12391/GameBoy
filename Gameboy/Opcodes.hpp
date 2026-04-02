//
//  Opcodes.hpp
//  Gameboy
//
//  Created by Alex on 1/27/26.
//

#ifndef Opcodes_hpp
#define Opcodes_hpp

#include <stdio.h>
#include <cstdint>
#include "CPU.hpp"
#include "MMU.hpp"

//LAST ONE DONE WAS ALL OF THEM!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! NEED TO ALSO DO 0x10, 0xD9, 0xFB, and 0xF3!

extern int cycles; //Does nothing. Just here so shit don't break.
void NOOP(); //0x00
void JPNN(); //0xC3
void JRs8(); //0x18
void JRNZs8(); //0x20
void JRZs8(); //0x28
void Calla16(); //0xCD
void Ret(); //0xC9
void LD_rr(uint8_t opcode); //0x40-0x7F NOT INCLUDING 0x76!
void LD_d8(uint8_t opcode); //0x06, 0x0E, 0x16, 0x1E, 0x26, 0x2E, 0x3E
void LDAa16(); //0xFA
void LDa16A(); //0xEA
void LD_d16(uint8_t opcode); //0x31, 0x21, 0x11, 0x01
void XORA (); //0xAF
void CPA8(); //0xFE
void CPAr(uint8_t opcode); //0xB8-0xBF
void LD_rrA(uint8_t opcode); //0x02, 0x12, 0x22, 0x32
void inc_rr(uint8_t opcode); //0x04, 0x0C, 0x14, 0x1C, 0x24, 0x2C, 0x3C
void inc_RR(uint8_t opcode); //0x03, 0x13, 0x23, 0x33
void dec_rr(uint8_t opcode); //0x05, 0x0D, 0x15, 0x1D, 0x25, 0x2D, 0x3D
void dec_RR(uint8_t opcode); //0x0B, 0x1B, 0x2B, 0x3B
void pushrr(uint8_t opcode); //0xC5, 0xD5, 0xE5, 0xF5
void poprr(uint8_t opcode); //0xC1, 0xD1, 0xE1, 0xF1
void LD_a16SP(); //0x08
void add_HLrr(uint8_t opcode); //0x09,0x19,0x29,0x39
void RCRA(); //0x0F
void RCLA(); //0x07
void RLA(); //0x17
void RRA(); //0x1F
bool halt(); //0x76
void LD_Arr(uint8_t opcode); //0x0A, 0x1A, 0x2A, 0x3A
void DAA(); //0x27
void CPL(); //0x2F
void JRNCs8(); //0x30
void INCHL(); //0x34
void DECHL(); //0x35
void LDHL_d8(); //0x36
void SCF(); //0x37
void JRCs8(); //0x38
void CCF(); //0x3F
void add_rr(uint8_t opcode); //0x80-0x8F
void sub_rr(uint8_t opcode); //0x90-0x9F
void and_rr(uint8_t opcode); //0xA0-0xA7
void xor_rr(uint8_t opcode); //0xA8-0xAF
void or_rr(uint8_t opcode); //0xB0-0xB7
void cp_rr(uint8_t opcode); //0xB8-0xBF
void ret_nz(); //0xC0
void ret_nc(); //0xD0
void JPNZa16(); //0xC2
void JPNCa16(); //0xD2
void CALLNZa16(); //0xC4
void CALLNCa16(); //0xD4
void ADDAd8(); //0xC6
void SUBAd8(); //0xD6
void ANDAd8(); //0xE6
void ORAd8(); //0xF6
void RST(uint8_t opcode); //0xC7, 0xD7, 0xE7, 0xF7, 0xCF, 0xDF, 0xEF, 0xFF
void ret_z(); //0xC8
void ret_c(); //0xD8
void JPZa16(); //0xCA
void JPCa16(); //0xDA
void CALLCa16(); //0xDC
void CALLZa16(); //0xCC
void ADCAd8(); //0xCE
void SBCAd8(); //0xDE
void XORAd8(); //0xEE
void CPAd8(); //0xFE
void LDa8A(); //0xE0
void LDCA(); //0xE2
void ADDSPs8(); ///0xE8
void JPHL(); //0xE9
void LDHLSP_d8(); //0xF8
void LDSPHL(); //0xF9
void LDAa8(); //0xF0


//0xCB Opcodes. These are very pattern based and simplistic, so while there are technically a few more, they are much easier to implement.
void rotate(uint8_t opcode); //0xCB00-0xCB3F
void bit (uint8_t opcode); //0xCB40-0xCB7F
void res (uint8_t opcode); //0xCB80-0xCBBF
void set (uint8_t opcode); //0xCBC0-0XCBFF
#endif


/*
                                  ,'\
    _.----.        ____         ,'  _\   ___    ___     ____
_,-'       `.     |    |  /`.   \,-'    |   \  /   |   |    \  |`.
\      __    \    '-.  | /   `.  ___    |    \/    |   '-.   \ |  |
 \.    \ \   |  __  |  |/    ,','_  `.  |          | __  |    \|  |
   \    \/   /,' _`.|      ,' / / / /   |          ,' _`.|     |  |
    \     ,-'/  /   \    ,'   | \/ / ,`.|         /  /   \  |     |
     \    \ |   \_/  |   `-.  \    `'  /|  |    ||   \_/  | |\    |
      \    \ \      /       `-.`.___,-' |  |\  /| \      /  | |   |
       \    \ `.__,'|  |`-._    `|      |__| \/ |  `.__,'|  | |   |
        \_.-'       |__|    `-._ |              '-.|     '-.| |   |
                                `'                            '-._|
*/