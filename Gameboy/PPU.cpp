//
//  PPU.cpp
//  Gameboy
//
//  Created by Alex on 4/29/26.
//

#include "PPU.hpp"

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
