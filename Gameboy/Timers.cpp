
//
//  Timers.cpp
//  Gameboy
//
//  Created by Alex on 4/6/26.
//

#include "Timers.hpp"

void doTimers()
{
	write_byte(0xFF04, read_byte(0xFF04) + 1);


}