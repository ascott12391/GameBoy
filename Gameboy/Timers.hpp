
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

extern bool last_result;
extern uint16_t divCounter;
extern int timer_reload_delay;

void doTimers();
#endif /* Timers_hpp */
