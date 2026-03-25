//
//  MMU.cpp
//  Gameboy
//
//  Created by Alex on 1/24/26.
//

#include "MMU.hpp"

uint8_t memory[65536];

void write_byte(uint16_t addr, uint8_t data)
{
    memory[addr] = data;
    if (addr >= 0xC000 && addr <= 0xDDFF){ //Echo ROM area
        write_byte(addr + 0x2000, data);
    }
    if (addr >= 0xFEA0 && addr <= 0xFEFF){ //Unuseable
        return;
    }
    if (addr == 0xFF04){ //Divder timer thingie. If written to it zeros out
        memory[addr] = 0;
    }
}
void write_word(uint16_t addr, uint16_t data)
{
    write_byte(addr, static_cast<uint8_t>(data & 0xFF));
    if (addr != 0xFFFF) {
        write_byte(addr+1, static_cast<uint8_t>((data >> 8) & 0xFF));
    }
}
uint8_t read_byte(uint16_t addr)
{
    if (addr >= 0xFEA0 && addr <= 0xFEFF){ //Not sure what behavior here should be TBH, but shouldn't be accessed by games anyway ¯\_(ツ)_/¯
        return 0;
    }
    return memory[addr];
}
uint16_t read_word(uint16_t addr)
{
    uint16_t lo = read_byte(addr);
    uint16_t hi = read_byte(static_cast<uint16_t>(addr + 1));
    return static_cast<uint16_t>(lo | (hi << 8));
}
