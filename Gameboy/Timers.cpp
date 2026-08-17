
//
//  Timers.cpp
//  Gameboy
//
//  Created by Alex on 4/6/26.
//

#include "Timers.hpp"

bool last_result = false;
uint16_t divCounter = 0;
int timer_reload_delay = false;

void doTimers()
{
	divCounter++;
    if (timer_reload_delay >= 0) {
        timer_reload_delay--;
        if (timer_reload_delay == 0) {
            write_byte(0xFF05, read_byte(0xFF06));
            write_byte(0xFF0F, read_byte(0xFF0F) | (1 << 2));
            timer_reload_delay = -1;
        }
    }
    write_div(divCounter >> 8); 
	uint16_t control = (read_byte(0xFF07)); //This is a little wonky, there's not really a pattern. It's called the TAC register, or Timer Control. There's a lot of info in this little guy.
	int control_bit = 0;
	switch (control & 0x3)
	{
	case 0:
		control_bit = 9;
		break;
	case 1:
		control_bit = 3;
		break;
	case 2:
		control_bit = 5;
		break;
	case 3:
		control_bit = 7;
		break;
	}
	bool control_bit_active = divCounter & (1 << control_bit); //This is how the GameBoy does falling edge timing. Basically we need the thing to be one in the last cycle and zero in the current one
	bool timer_enable = control & (1 << 2);
	bool current_result = control_bit_active && timer_enable;
    bool falling_edge = last_result && !current_result;
    last_result = current_result;
    if (!falling_edge) return;
	uint8_t TIMA = read_byte(0xFF05); //TIMA register
	if (TIMA == 0xFF) {
		write_byte(0xFF05, 0x00); //If the TIMA register overflows, it resets at the value at 0xFF06 after the next 4 cycles
        timer_reload_delay = 5; //It also requests an interrupt
	}
	else { write_byte(0xFF05, TIMA + 1); }

}

//Should be good enough for tonight. I have to do interrupts tomorrow, but for now I'm going to Ann Arbor to watch the boys in blue win the natty.
//LETS GO BLUE
