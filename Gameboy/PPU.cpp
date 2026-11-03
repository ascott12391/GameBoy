//
//  PPU.cpp
//  Gameboy
//
//  Created by Alex on 4/29/26.
//

#include "PPU.hpp"

uint8_t framebuffer[144][160];
const uint8_t shades[4] = { //https://gbstudiocentral.com/resources/gb-studio-numbers-cheat-sheet/ For actual colors [asdf]
    255,  // White
    170,  // Light gray
    85,   // Dark gray
    0     // Black
};

bool LCDEnabled() //Check if screen is enabled. Bit seven of 0xFF40
{return (read_byte(0xFF40) & 0x80) != 0;}

bool tileMapArea() //This shows what area the tile map is at. False for 9800–9BFF, true for 9C00–9FFF
{return (read_byte(0xFF40) & 0x40) != 0;}

bool windowEnabled() //Check if window is enabled.
{return (read_byte(0xFF40) & 0x20) != 0;}

bool BGTileDataArea() //This shows where BG Tile data is at. False for 8800–97FF, true for 8000–8FFF
{return (read_byte(0xFF40) & 0x10) != 0;}

bool BGTileMapArea() //This shows where BG Tile map is at. False for 9800–9BFF, true for 9C00–9FFF
{return (read_byte(0xFF40) & 0x8) != 0;}

bool OBJSize() //This shows where OBJ Size. False for 8×8, true for 8x16
{return (read_byte(0xFF40) & 0x4) != 0;}

bool OBJEnable() //Check if OBJ enabled
{return (read_byte(0xFF40) & 0x2) != 0;}

bool BGWindowEnable() //Enables BG Window
{return (read_byte(0xFF40) & 0x1) != 0;}

void renderBackground()
{
    uint16_t tileMapAddress = 0x9800; //Needs to be variable, fix later just wanna get something on the screen[asdf]
    uint16_t tileDataAddress = 0x8000; //See above
    
    Tile tiles[384];
    
    for (int i = 0; i < 384; ++i)
    {
        tiles[i].decode(tileDataAddress + (i*16));
    }
    for (int Y = 0; Y < 32; ++Y)
    {
        for (int X = 0; X < 32; ++X)
        {
            uint16_t tileID = tileMapAddress + (Y*32) + X;
            tileID /= 16;
            tileID = tileID % 256;
            int pixelX = X*8;
            int pixelY = Y*8;
            for (int i = 0; i < 8; ++i)
            {
                for (int j = 0; j < 8; ++j)
                {
                    if (pixelX + j < 160 && pixelY + i < 144)
                    {
                        framebuffer[pixelY + i][pixelX + j] = tiles[tileID].pixels[i][j];
                    }
                }
            }
        }
    }
}
