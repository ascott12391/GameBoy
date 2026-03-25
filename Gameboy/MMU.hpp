//
//  MMU.hpp
//  Gameboy
//
//  Created by Alex on 1/24/26.
//

#ifndef MMU_hpp
#define MMU_hpp

#include <stdio.h>
#include <cstdint>
extern uint8_t memory[65536];
void write_word(uint16_t addr, uint16_t data);
void write_byte(uint16_t addr, uint8_t data);
uint8_t read_byte(uint16_t addr);
uint16_t read_word(uint16_t addr);
#endif /* MMU_hpp */
