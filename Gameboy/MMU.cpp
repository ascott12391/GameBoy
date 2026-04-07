//
//  MMU.cpp
//  Gameboy
//
//  Created by Alex on 1/24/26.
//
#include <chrono>
#include <iostream>
#include "MMU.hpp"

uint8_t memory[65536];

void write_byte(uint16_t addr, uint8_t data)
{
    // Mirror ECHO RAM → WRAM
    if (addr >= 0xE000 && addr <= 0xFDFF) {
        addr -= 0x2000;
    }

    // Unusable memory
    if (addr >= 0xFEA0 && addr <= 0xFEFF){
        return;
    }

    // Divider register reset
    if (addr == 0xFF04){
        memory[addr] = 0;
        return;
    }
    if (addr == 0xD81A)
    {
        data = data;
        addr = addr;
    }
    memory[addr] = data;

    // Debug output (test ROMs)
    if (addr == 0xFF02 && data == 0x81) {
        char c = memory[0xFF01];
        std::cout << c << std::flush;
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
    if (addr == 0xFF44)
    {
        return 0x90; //Supposedly prevents divergences in testing logs.
    }
    if (addr >= 0xE000 && addr <= 0xFDFF) {
        addr -= 0x2000;
    }
    return memory[addr];
}
uint16_t read_word(uint16_t addr)
{
    uint16_t lo = read_byte(addr);
    uint16_t hi = read_byte(static_cast<uint16_t>(addr + 1));
    return static_cast<uint16_t>(lo | (hi << 8));
}
