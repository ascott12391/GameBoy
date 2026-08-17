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

uint8_t framebuffer[192][128];
bool LCDEnabled();
bool tileMapArea();
bool windowEnabled();
bool BGTileDataArea();
bool BGTileMapArea();
bool OBJSize();
bool OBJEnable();
bool BGWindowEnable();

void tileViewer()
{
    Tile tileArray[384];
    for (int tile = 0; tile < 384; tile++)
    {
        uint16_t address = 0x8000 + (tile * 16);
        tileArray[tile].decode(address);
    }
    for (int tile = 0; tile < 384; tile++)
    {
        int tileX = (tile % 16) * 8;
        int tileY = (tile / 16) * 8;

        for (int y = 0; y < 8; y++)
        {
            for (int x = 0; x < 8; x++)
            {
                framebuffer[tileY + y][tileX + x] =
                    tileArray[tile].pixels[y][x];
            }
        }
    }
    const char* shades = " .:#";
    for (int i = 0; i < 192; i++)
    {
        for (int j = 0; j < 128; j++)
        {
            std::cout << shades[framebuffer[i][j]];
        }
        std::cout << std::endl;
    }
    
} //This is only for debugging.


#endif /* PPU_hpp */
