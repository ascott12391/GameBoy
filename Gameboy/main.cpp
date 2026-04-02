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

usin OpFunc = void(*)(uint8_t);

int main(int argc, const char * argv[]) 
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
    OpFunc opcode_table[256];
    OpFunc CBopcode_table[256];
    for (int i = 0; i < 0x7FFF; i++) //Very bad doesn't work w/ banking, fix later [asdf]
    {
        memory[i] = romData[i];
    }
    init_table(opcode_table, CBopcode_table);
    bool halted = false;
    BYTE opcode;
    while (!halted) //This is ugly as hell, but I just wanna get this working rn, and Sbux closes in 30 mins. Come back to later [fixed]
    {
        opcode = romData[pc];
        logData(logger, opcode);
        if (opcode == 0xCB) {
            opcode = romData[pc+1];
            CBopcode_table[opcode](opcode);
        }
        else{opcode_table[opcode](opcode);}
    }
    
}

void logData(FILE* logger, uint8_t opcode)
{
    fprintf(//logger,
        "%04X %02X  "
        "A:%02X F:%02X B:%02X C:%02X D:%02X E:%02X H:%02X L:%02X "
        "SP:%04X  \n",
        pc, opcode,
        AF.hi, AF.lo, BC.hi, BC.lo, DE.hi, DE.lo, HL.hi, HL.lo, SP //Fix later. See Sbux comments for context. [fixed]
    );
}

void init_table(OpFunc &opcode_table[], OpFunc &CBopcode_table[]) //Sets up pointers of functions for opcodes
{
    opcode_table[0x00] = [](uint8_t){NOOP();};
    opcode_table[0x01] = LD_d16;
    opcode_table[0x11] = LD_d16;
    opcode_table[0x21] = LD_d16;
    opcode_table[0x31] = LD_d16;
    opcode_table[0x02] = LD_rrA;
    opcode_table[0x12] = LD_rrA;
    opcode_table[0x22] = LD_rrA;
    opcode_table[0x32] = LD_rrA;
    opcode_table[0x03] = inc_RR;
    opcode_table[0x13] = inc_RR;
    opcode_table[0x23] = inc_RR;
    opcode_table[0x33] = inc_RR;
    opcode_table[0x04] = inc_rr;
    opcode_table[0x14] = inc_rr;
    opcode_table[0x1C] = inc_rr;
    opcode_table[0x24] = inc_rr;
    opcode_table[0x2C] = inc_rr;
    opcode_table[0x0C] = inc_rr;
    opcode_table[0x3C] = inc_rr;
    opcode_table[0x05] = dec_rr;
    opcode_table[0x0D] = dec_rr;
    opcode_table[0x15] = dec_rr;
    opcode_table[0x1D] = dec_rr;
    opcode_table[0x25] = dec_rr;
    opcode_table[0x2D] = dec_rr;
    opcode_table[0x3D] = dec_rr;
    opcode_table[0x06] =  LD_d8;
    opcode_table[0x0E] =  LD_d8;
    opcode_table[0x16] =  LD_d8;
    opcode_table[0x1E] =  LD_d8;
    opcode_table[0x26] =  LD_d8;
    opcode_table[0x2E] =  LD_d8;
    opcode_table[0x3E] =  LD_d8;
    opcode_table[0x07] = [](uint8_t){RCLA();};
    opcode_table[0x08] = [](uint8_t){LD_a16SP();};
    opcode_table[0x09] = add_HLrr;
    opcode_table[0x19] = add_HLrr;
    opcode_table[0x29] = add_HLrr;
    opcode_table[0x39] = add_HLrr;
    opcode_table[0x0A] =   LD_Arr;
    opcode_table[0x1A] =   LD_Arr;
    opcode_table[0x2A] =   LD_Arr;
    opcode_table[0x3A] =   LD_Arr;
    opcode_table[0x0B] =   dec_RR;
    opcode_table[0x1B] =   dec_RR;
    opcode_table[0x2B] =   dec_RR;
    opcode_table[0x3B] =   dec_RR;
    opcode_table[0x0F] = [](uint8_t){RCRA();};
    opcode_table[0x17] = [](uint8_t){RLA();};
    opcode_table[0x18] = [](uint8_t){JRs8();};
    opcode_table[0x1F] = [](uint8_t){RRA();};
    opcode_table[0x20] = [](uint8_t){JRNZs8();};
    opcode_table[0x27] = [](uint8_t){DAA();}; //This is so long...
    opcode_table[0x28] = [](uint8_t){JRZs8();}
    opcode_table[0x2F] = [](uint8_t){CPL();}
    opcode_table[0x30] = [](uint8_t){JRNCs8();};
    opcode_table[0x34] = [](uint8_t){INCHL();};
    opcode_table[0x35] = [](uint8_t){DECHL();};
    opcode_table[0x36] = [](uint8_t){LDHL_d8();};
    opcode_table[0x37] = [](uint8_t){SCF();};
    opcode_table[0x38] = [](uint8_t){JRCs8();};
    opcode_table[0x3F] = [](uint8_t){CCF();};
    for (int i = 0x40; i <= 0x7F; i++) //Can speed things up here
    {
        if(i != 0x76){opcode_table[i] = LD_rr;} //Remember, 0x76 is HALT
    }
    for (int i = 0x80; i <= 0x8F; i++)
    {
        opcode_table[i] = add_rr;
    }
    for (int i = 0x90; i <= 0x9F; i++)
    {
        opcode_table[i] = sub_rr;
    }
    for (int i = 0xA0; i <= 0xA7; i++)
    {
        opcode_table[i] = and_rr;
    }
    for (int i = 0xA8; i <= 0xAF; i++)
    {
        opcode_table[i] = xor_rr;
    }
    for (int i = 0xB0; i <= 0xB7; i++)
    {
        opcode_table[i] = or_rr;
    }
    for (int i = 0xB0; i <= 0xB7; i++)
    {
        opcode_table[i] = cp_rr;
    }
    opcode_table[0xC0] = [](uint8_t){ret_nz();}; //And now back to pain
    opcode_table[0xC1] = poprr;
    opcode_table[0xD1] = poprr;
    opcode_table[0xE1] = poprr;
    opcode_table[0xF1] = poprr;
    opcode_table[0xC2] = [](uint8_t){JPNZa16();};
    opcode_table[0xC3] = [](uint8_t){JPNN();};
    opcode_table[0xC4] = [](uint8_t){CALLNZa16();};
    opcode_table[0xC5] = pushrr;
    opcode_table[0xD5] = pushrr;
    opcode_table[0xE5] = pushrr;
    opcode_table[0xF5] = pushrr;
    opcode_table[0xC6] = [](uint8_t){ADDAd8();};
    opcode_table[0xC7] = RST;
    opcode_table[0xD7] = RST;
    opcode_table[0xE7] = RST;
    opcode_table[0xF7] = RST;
    opcode_table[0xCF] = RST;
    opcode_table[0xDF] = RST;
    opcode_table[0xEF] = RST;
    opcode_table[0xFF] = RST;
    opcode_table[0xC8] = [](uint8_t){ret_z();}
    opcode_table[0xC9] = [](uint8_t){Ret();}
    opcode_table[0xCA] = [](uint8_t){JPZa16();}
    opcode_table[0xCC] = [](uint8_t){CALLZa16();}
    opcode_table[0xCD] = [](uint8_t){Calla16();}
    opcode_table[0xCE] = [](uint8_t){ADCAd8();}
    opcode_table[0xD0] = [](uint8_t){ret_nc();}
    opcode_table[0xD2] = [](uint8_t){JPNCa16();}
    opcode_table[0xD4] = [](uint8_t){CALLNCa16();}
    opcode_table[0xD6] = [](uint8_t){SUBAd8();}
    opcode_table[0xD8] = [](uint8_t){ret_c();}
    opcode_table[0xDA] = [](uint8_t){JPCa16();}
    opcode_table[0xDC] = [](uint8_t){CALLCa16();}
    opcode_table[0xDE] = [](uint8_t){SBCAd8();}
    opcode_table[0xE0] = [](uint8_t){LDa8A();}
    opcode_table[0xE2] = [](uint8_t){LDCA();}
    opcode_table[0xE6] = [](uint8_t){ANDAd8();}
    opcode_table[0xE8] = [](uint8_t){ADDSPs8();}
    opcode_table[0xE9] = [](uint8_t){JPHL();}
    opcode_table[0xEA] = [](uint8_t){LDa16A();}
    opcode_table[0xEE] = [](uint8_t){XORAd8();}
    opcode_table[0xF0] = [](uint8_t){LDAa8();}
    opcode_table[0xF6] = [](uint8_t){ORAd8();}
    opcode_table[0xF8] = [](uint8_t){LDHLSP_d8();}
    opcode_table[0xF9] = [](uint8_t){LDSPHL();}
    opcode_table[0xFA] = [](uint8_t){LDAa16();}
    opcode_table[0xFE] = [](uint8_t){CPAd8();} //Now that that's done I got the easy part

    for(int i = 0; i <= 0x3F; i++)
    {
        CBopcode_table[i] = rotate;
    }
    for(int i = 0x40; i <= 0x7F; i++)
    {
        CBopcode_table[i] = bit;
    }
    for(int i = 0x80; i <= 0xBF; i++)
    {
        CBopcode_table[i] = res;
    }
    for(int i = 0xC0; i <= 0xFF; i++)
    {
        CBopcode_table[i] = set;
    }

}
