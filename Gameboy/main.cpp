//
//  main.cpp
//  Gameboy
//
//  Created by Alex on 1/25/26.
//
// Heads up, I tag stuff [asdf] so I ctrl+F for it later to come back to. Otherwise I'd probably forget what I wanted to fix later. I use [asdf] because it (probably) won't show up in actual code.

#include <iostream>
#include <fstream>
#include <vector>
#include "Opcodes.hpp"
typedef unsigned char BYTE;
void logData(FILE* logger, uint8_t opcode);

int main(int argc, const char * argv[]) {
    pc  = 0x0150;
    FILE* logger = fopen("log.txt", "w");
    long size;
    std::ifstream game("cpu_instrs.gb", std::ios::binary| std::ios::in);
    if (!game) {std::cout << "No ROM detected"; return 0;} //Update for error code here [asdf]
    game.seekg(0, std::ios::end);
    size = game.tellg();
    game.seekg(0, std::ios::beg);
    std::vector<BYTE> romData(size);
    game.read((char*) &romData[0], size);
    for (int i = 0; i < 0x7FFF; i++) //Very bad doesn't work w/ banking, fix later [asdf]
    {
        memory[i] = romData[i];
    }
    bool halted = false;
    BYTE opcode;
    while (!halted) //This is ugly as hell, but I just wanna get this working rn, and Sbux closes in 30 mins. Come back to later [asdf]
    {
        opcode = romData[pc];
        logData(logger, opcode);
        if ((opcode >= 0x40 && opcode <= 0x7F) && (opcode != 0x76)) {
            LD_rr(opcode);
        }
        else if (opcode == 0x06 || opcode == 0x16 || opcode == 0x26 || opcode == 0x0E || opcode == 0x1E || opcode == 0x2E){
            LD_d8(opcode);
        }
        else if (opcode == 0x31 || opcode == 0x21 || opcode == 0x11 || opcode == 0x01){
            LD_d16(opcode);
        }
        else if (opcode >= 0xB8 && opcode <= 0xBF){
            CPAr(opcode);
        }
        else if (opcode == 0x04 || opcode == 0x0C || opcode == 0x14 || opcode == 0x1C || opcode == 0x24 || opcode == 0x2C){
            inc_rr(opcode);
        }
        else if (opcode == 0x05 || opcode == 0x0D || opcode == 0x15 || opcode == 0x1D || opcode == 0x25 || opcode == 0x2D){
            dec_rr(opcode);
        }
        else if (opcode == 0xC5 || opcode == 0xD5 || opcode == 0xE5 || opcode == 0xF5){
            pushrr(opcode);
        }
        else if (opcode == 0xC1 || opcode == 0xD1 || opcode == 0xE1 || opcode == 0xF1){
            poprr(opcode);
        }
        else{
            switch (opcode){
                case 0x00:
                    NOOP();
                    break;
                case 0xC3:
                    JPNN();
                    break;
                case 0x18:
                    JRs8();
                    break;
                case 0x20:
                    JRNZs8();
                    break;
                case 0x28:
                    JRZs8();
                    break;
                case 0xCD:
                    Calla16();
                    break;
                case 0xC9:
                    Ret();
                    break;
                case 0xFA:
                    LDAa16();
                    break;
                case 0xEA:
                    LDa16A();
                    break;
                case 0xAF:
                    XORA();
                    break;
                case 0xFE:
                    CPA8();
                    break;
                case 0x76:
                    halted = halt();
                    break;
                default:
                    incPC(1); //Just here for testing purposes. Remove when finished w/ all opcodes
            }
        }
    }
    
}

void logData(FILE* logger, uint8_t opcode)
{
    fprintf(logger,
        "%04X %02X  "
        "A:%02X F:%02X B:%02X C:%02X D:%02X E:%02X H:%02X L:%02X "
        "SP:%04X  \n",
        pc, opcode,
        AF.hi, AF.lo, BC.hi, BC.lo, DE.hi, DE.lo, HL.hi, HL.lo, SP //Fix later. See Sbux comments for context. [fixed]
    );
}
