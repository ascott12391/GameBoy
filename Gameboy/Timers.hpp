
//
//  Timers.hpp
//  Gameboy
//
//  Created by Alex on 4/6/26
//

#ifndef Timers_hpp
#define Timers_hpp

#include <stdio.h>
#include <cstdint>
#include "MMU.hpp"

extern bool last_result = false;
extern uint16_t divCounter;

void doTimers();
#endif /* Timers_hpp */