//
//  PPU.hpp
//  Gameboy
//
//  Created by Alex on 4/29/26.
//

#ifndef PPU_hpp
#define PPU_hpp

#include <stdio.h>
#include <iostream>
#include "MMU.hpp"

class Tile
{
    public:
        uint8_t pixels[8][8];
        void decode(const uint16_t address)
        {
            for (int i = 0; i < 16; i+=2)
            {
                uint8_t lo = read_byte(address+i);
                uint8_t hi = read_byte(address+i+1);
                for (int j = 7; j > -1; j--)
                {
                    uint8_t color = ((hi>>j) & 1) << 1 | ((lo>>j) & 1);
                    pixels[i/2][j] = color;
                }
            }
        }
    /* So, tile data is,
     a little weird. Basically,
     you combine two bytes.
     I realized in line three that I had a Haiku.
     But seriously, if you have c3,25 in binary, you go through each bit and combine them, like 00 means color 0, 01 means 1, etc.
     It's hard to explain in short here, PanDocs is a friend of you and me.
     Also color 0 is always transparent in sprites*/
};

extern const uint8_t shades[4]; 

extern uint8_t framebuffer[144][160];
bool LCDEnabled();
bool tileMapArea();
bool windowEnabled();
bool BGTileDataArea();
bool BGTileMapArea();
bool OBJSize();
bool OBJEnable();
bool BGWindowEnable();
void renderBackground();



#endif /* PPU_hpp */
