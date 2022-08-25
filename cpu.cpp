//
//  cpu.cpp
//  Gameboy
//
//  Created by Alex on 8/15/22.
//

#include "cpu.hpp"

//MEMORY STUFF
uint8_t CPU::readByte(uint16_t loc) {return memory[loc];}

uint16_t CPU::readWord(uint16_t loc) {return (memory[loc]|memory[loc+1]<<8);}

void CPU::writeByte(uint16_t loc, uint8_t data) {memory[loc] = data;}

void CPU::writeWord(uint16_t loc, uint16_t data)
{
    memory[loc] = data&0x00FF;
    memory[loc+1] = data&0xFF00>>8;
}

//FLAG STUFF
void CPU::setSub() {registers[0].lo|=0x40;}

void CPU::resSub() {registers[0].lo&=0xBF;}

bool CPU::checkSub() {return ((registers[0].lo&0x40)==1);}

void CPU::setCarry() {registers[0].lo|=0x10;}

void CPU::resCarry() {registers[0].lo&=0xEF;}

bool CPU::checkCarry() {return (registers[0].lo&0x10)==1;}

void CPU::setHalfCarry() {registers[0].lo|=0x20;}

void CPU::resHalfCarry() {registers[0].lo&=0xDF;}

bool CPU::checkHalfCarry() {return (registers[0].lo&0x20)==1;}

void CPU::setZero() {registers[0].lo|=0x80;}

void CPU::resZero() {registers[0].lo&=0x7F;}

bool CPU::checkZero() {return (registers[0].lo&0x80)==1;}

uint16_t CPU::findAddr(int reg)
{
    uint16_t loc = 0;
    loc = registers[reg].hi << 8;
    loc += registers[reg].lo;
    return loc;
}

//OPCODES
void CPU::OP_00() {return;} //no-op

void CPU::OP_01() //Load 16-bit immediate into BC
{
    registers[1].lo = readByte(pc);
    pc+=1;
    registers[1].hi = readByte(pc);
    pc+=1;
    timer-=2;
}

void CPU::OP_02() //Load the data in A into the location specified by BC
{
    writeByte((registers[1].hi<<8)+registers[1].lo, registers[0].hi);
    timer--;
}

void CPU::OP_03() //Increment BC by one
{
    registers[1].lo++;
    registers[1].lo==0?registers[1].hi++:registers[1].hi=registers[1].hi;
    timer--;
}

void CPU::OP_04() //Increment B by one
{
    registers[1].hi++;
    registers[1].hi==0?setZero():resZero(); //Set 0 flag
    resSub();
    (registers[1].hi&0x0F)==0?setHalfCarry():resHalfCarry(); //Set half carry flag
}

void CPU::OP_05() //Decrement B by one
{
    registers[1].hi--;
    registers[1].hi==0?setZero():resZero(); //Set 0 flag
    setSub();
    (registers[1].hi&0x0F)==0xFF?setHalfCarry():resHalfCarry(); //Set half carry flag
}

void CPU::OP_06() //Load 8-bit immediate into B
{
    registers[1].hi = readByte(pc);
    pc++;
    timer--;
}

void CPU::OP_07() //Rotate regiser A left one
{
    uint8_t temp = registers[0].hi&0x80;
    registers[0].hi<<=1;
    registers[0].lo = 0; //Set all flags to 0;
    if(temp==0x80){
        registers[0].hi++;
        setCarry();
    }
}

void CPU::OP_08() //Store stack pointer into the 16 bit immediate (This honestly seems wrong, so maybe need to change, because otherwise what the actual fuck?)
{
    uint16_t temp = callStack.top();
    writeByte(pc, temp&0x00FF);
    pc++;
    writeByte(pc, temp&0xFF00>>8);
    pc++;
    timer-=4;
}

void CPU::OP_09() //Register HL += register BC
{
    uint16_t temp = (registers[3].hi<<8)+registers[3].lo;
    temp += (registers[1].hi<<8)+registers[1].lo;
    registers[3].hi = (temp&0xFF00)>>8;
    registers[3].lo = temp&0x00FF;
    setSub();
    (temp&0xF000)==0?setCarry():resCarry();
    (temp&0x0F00)==0?setHalfCarry():resHalfCarry();
    timer--;
}

void CPU::OP_0A() //Load into 8 the data pointed to by BC
{
    registers[0].hi = readByte((registers[1].hi<<8)+registers[1].lo);
    timer--;
}

void CPU::OP_0B() //Decrement BC by one
{
    registers[1].lo--;
    if (registers[1].lo == 0xFF) {registers[1].hi--;}
    timer--;
}

void CPU::OP_0C() //Increase register C by one
{
    registers[1].lo++;
    registers[1].lo==0?setZero():resZero();
    resSub();
    (registers[1].lo&0x0F)==0?setHalfCarry():resHalfCarry();
}

void CPU::OP_0D() //Decrement C by one
{
    registers[1].lo--;
    registers[1].lo==0?setZero():resZero();
    setSub();
    (registers[1].lo&0x0F)==0?setHalfCarry():resHalfCarry();
}

void CPU::OP_0E() //Load 8-bit immediate into C
{
    registers[1].lo = readByte(pc);
    pc++;
    timer--;
}

void CPU::OP_0F() //Rotate A right
{
    uint8_t temp = registers[0].hi&0x1;
    registers[0].hi >>= 1;
    registers[0].lo = 0; //Reset flags
    if (temp == 1)
    {
        registers[0].hi |= 0x80;
        setCarry();
    }
}

void CPU::OP_10() //STOP, puts GB in some kind of rest mode. I don't *think* it's necessary. Right now I'm just gonna crash it and hopefully that will be good enough
{
    //And if you really want to take the word crash literally, just uncomment the line below
    //*(int*)0=0xDEADBEEF;
    exit(10); //Exits w/ 10 so if I see that code I know opcode 0x10 was called
}

void CPU::OP_11() //Load 16-bit immediate into DE
{
    registers[2].lo = readByte(pc);
    pc+=1;
    registers[2].hi = readByte(pc);
    pc+=1;
    timer-=2;
}

void CPU::OP_12() //Load the data in A into the location specified by DE
{
    writeByte((registers[2].hi<<8)+registers[2].lo, registers[0].hi);
    timer--;
}

void CPU::OP_13() //Increment DE by one
{
    registers[2].lo++;
    registers[2].lo==0?registers[2].hi++:registers[2].hi=registers[2].hi;
    timer--;
}

void CPU::OP_14() //Increment D by one
{
    registers[2].hi++;
    registers[2].hi==0?setZero():resZero();
    setSub();
    (registers[2].hi&0x0F)==0?setHalfCarry():resHalfCarry();
}

void CPU::OP_15() //Decrement D by one
{
    registers[2].hi--;
    registers[2].hi==0?setZero():resZero();
    setSub();
    (registers[2].hi&0x0F)==0xFF?setHalfCarry():resHalfCarry();
}

void CPU::OP_16() //Load 8-bit immediate into D
{
    registers[2].hi = readByte(pc);
    pc++;
    timer--;
}

void CPU::OP_17() //Rotate regiser A left one through carry
{
    uint8_t temp = registers[0].hi&0x80;
    registers[0].hi<<=1;
    if((registers[0].lo&0x10)==0x10){registers[0].hi++;}
    registers[0].lo = 0; //Set all flags to 0;
    if(temp==0x80){setCarry();} //Set carry flag
}

void CPU::OP_18() //Jump ahead [8 bit immediate] steps;
{
    pc+=readByte(pc);
    timer-=2;
}

void CPU::OP_19() //Register HL += register DE
{
    uint16_t temp = (registers[3].hi<<8)+registers[3].lo;
    temp += (registers[2].hi<<8)+registers[2].lo;
    registers[3].hi = (temp&0xFF00)>>8;
    registers[3].lo = temp&0x00FF;
    resSub();
    (temp&0xF000)==0?setCarry():resCarry();
    (temp&0x0F00)==0?setHalfCarry():resHalfCarry();
    timer--;
}

void CPU::OP_1A() //Load into 8 the data pointed to by DE
{
    registers[0].hi = readByte((registers[2].hi<<8)+registers[2].lo);
    timer--;
}

void CPU::OP_1B() //Decrement DE by one
{
    registers[2].lo--;
    if (registers[2].lo == 0xFF) {registers[2].hi--;}
    timer--;
}

void CPU::OP_1C() //Increase register D by one
{
    registers[2].lo++;
    registers[2].lo==0?setZero():resZero();
    setSub();
    (registers[2].lo&0x0F)==0?setHalfCarry():resHalfCarry();
}

void CPU::OP_1D() //Decrement D by one
{
    registers[2].lo--;
    registers[2].lo==0?setZero():resZero();
    setSub();
    (registers[2].lo&0x0F)==0?setHalfCarry():resHalfCarry();
}

void CPU::OP_1E() //Load 8-bit immediate into C
{
    registers[2].lo = readByte(pc);
    pc++;
    timer--;
}

void CPU::OP_1F() //Rotate A right through carry
{
    uint8_t temp = registers[0].hi&0x1;
    registers[0].hi >>= 1;
    if((registers[0].lo&0x10)==0x10){registers[0].hi|=0x80;}
    registers[0].lo = 0; //Reset flags
    if(temp == 1){setCarry();} //Set carry flag
}

void CPU::OP_20() //Jump if not zero
{
    if(!checkZero())
    {
        pc += readByte(pc)+1;
        timer-=2;
    }
    else
    {
        timer--;
        pc++;
    }
}

void CPU::OP_21() //Load 16-bit immediate into HL
{
    registers[3].lo = readByte(pc);
    pc+=1;
    registers[3].hi = readByte(pc);
    pc+=1;
    timer-=2;
}

void CPU::OP_22() //Load the data in A into the location specified by HL and then increase it by one
{
    writeByte((registers[3].hi<<8)+registers[3].lo, registers[0].hi);
    OP_23();
}

void CPU::OP_23() //Increment HL by one
{
    registers[3].lo++;
    registers[3].lo==0?registers[3].hi++:registers[3].hi=registers[3].hi;
    timer--;
}

void CPU::OP_24() //Increment H by one
{
    registers[3].hi++;
    registers[3].hi==0?setZero():resZero();
    setSub();
    (registers[3].hi&0x0F)==0?setHalfCarry():resHalfCarry();
}

void CPU::OP_25() //Decrement H by one
{
    registers[3].hi--;
    registers[3].hi==0?setZero():resZero();
    setSub();
    (registers[3].hi&0x0F)==0xFF?setHalfCarry():resHalfCarry();
}

void CPU::OP_26() //Load 8-bit immediate into H
{
    registers[3].hi = readByte(pc);
    pc++;
    timer--;
}

void CPU::OP_27() {exit(27);} //Ahh, good ol' DAA. Super comlicated. Not really feelin it rn, I'll do this later :)

void CPU::OP_28() //Jump if zero
{
    if(checkZero())
    {
        pc += readByte(pc)+1;
        timer-=2;
    }
    else
    {
        timer--;
        pc++;
    }
}

void CPU::OP_29() //Register HL += register HL
{
    uint16_t temp = (registers[3].hi<<8)+registers[3].lo;
    temp += (registers[3].hi<<8)+registers[3].lo;
    registers[3].hi = (temp&0xFF00)>>8;
    registers[3].lo = temp&0x00FF;
    resSub();
    (temp&0xF000)==0?setCarry():resCarry();
    (temp&0x0F00)==0?setHalfCarry():resHalfCarry();
    timer--;
}

void CPU::OP_2A() //Load into 8 the data pointed to by HL and add 1 to HL
{
    registers[0].hi = readByte((registers[3].hi<<8)+registers[3].lo);
    OP_23();
}

void CPU::OP_2B() //Decrement HL by one
{
    registers[3].lo--;
    if (registers[3].lo == 0xFF) {registers[3].hi--;}
    timer--;
}

void CPU::OP_2C() //Increase register L by one
{
    registers[3].lo++;
    registers[3].lo==0?setZero():resZero();
    setSub();
    (registers[3].lo&0x0F)==0?setHalfCarry():resHalfCarry();
}

void CPU::OP_2D() //Decrement L by one
{
    registers[3].lo--;
    registers[3].lo==0?setZero():resZero();
    setSub();
    (registers[3].lo&0x0F)==0?setHalfCarry():resHalfCarry();
}

void CPU::OP_2E() //Load 8-bit immediate into L
{
    registers[3].lo = readByte(pc);
    pc++;
    timer--;
}

void CPU::OP_2F() //Flip all bits of A
{
    registers[0].hi = ~registers[0].hi;
    setSub(); //Not sure why these are set for this inst, but they are ¯\_(ツ)_/¯
    setHalfCarry();
}

void CPU::OP_30() //Jump if not carry
{
    if(!checkCarry())
    {
        pc += readByte(pc)+1;
        timer-=2;
    }
    else
    {
        timer--;
        pc++;
    }
}

void CPU::OP_31() //Load immediate into the stack
{
    callStack.push(readWord(pc));
    pc+=2;
    timer-=2;
}

void CPU::OP_32() //Load the data in A into the location specified by HL and then decrease it by one
{
    writeByte((registers[3].hi<<8)+registers[3].lo, registers[0].hi);
    OP_2B();
}

void CPU::OP_33(){callStack.top()++;} //Increase SP by one

void CPU::OP_34() //Increase the memory contents pointed to by HL by one
{
    uint16_t loc = findAddr(3);
    memory[loc]++;
    memory[loc]==0?setZero():resZero();
    resSub();
    (memory[loc]&0x0F)==0?setHalfCarry():resHalfCarry();
    timer-=2;
}

void CPU::OP_35() //Decrease the memory contents pointed to by HL by one
{
    uint16_t loc = findAddr(3);
    memory[loc]--;
    memory[loc]==0?setZero():resZero();
    setSub();
    (memory[loc]&0x0F)==0?setHalfCarry():resHalfCarry();
    timer-=2;
}

void CPU::OP_36() //Load 8-bit immediate into location pointed to by HL
{
    uint16_t loc = findAddr(3);
    memory[loc] = readByte(pc);
    pc++;
    timer-=2;
}

void CPU::OP_37() //Set carry flag (probably obvious)
{
    setCarry();
    resSub();
    resHalfCarry();
}

void CPU::OP_38() //Jump if carry
{
    if(checkCarry())
    {
        pc += readByte(pc)+1;
        timer-=2;
    }
    else
    {
        timer--;
        pc++;
    }
}

void CPU::OP_39() //Register HL += SP
{
    uint16_t temp = (registers[3].hi<<8)+registers[3].lo;
    temp += callStack.top();
    registers[3].hi = (temp&0xFF00)>>8;
    registers[3].lo = temp&0x00FF;
    resSub();
    (temp&0xF000)==0?setCarry():resCarry();
    (temp&0x0F00)==0?setHalfCarry():resHalfCarry();
    timer--;
}

void CPU::OP_3A() //Load into 8 the data pointed to by HL and add -1 to HL
{
    registers[0].hi = readByte((registers[3].hi<<8)+registers[3].lo);
    OP_2B();
}

void CPU::OP_3B(){callStack.top()--;} //Decrease SP by one

void CPU::OP_3C() //Increase register A by one
{
    registers[0].hi++;
    registers[0].hi==0?setZero():resZero();
    setSub();
    (registers[0].hi&0x0F)==0?setHalfCarry():resHalfCarry();
}

void CPU::OP_3D() //Decrement A by one
{
    registers[0].hi--;
    registers[0].hi==0?setZero():resZero();
    setSub();
    (registers[0].hi&0x0F)==0?setHalfCarry():resHalfCarry();
}

void CPU::OP_3E() //Load 8-bit immediate into A
{
    registers[0].hi = readByte(pc);
    pc++;
    timer--;
}

void CPU::OP_3F() //Flip carry
{
    if (checkCarry()){resCarry();}
    else {setCarry();}
    resSub();
    resHalfCarry();
}

//Now begins the long slog of dozens of load instructions

//And after that literal hundreds of simple math instructions :/

//At least I can copy paste for most of it


void CPU::OP_40() {registers[1].hi = registers[1].hi;} //Load B into B. Yes, that was my reaction as well
void CPU::OP_41() {registers[1].hi = registers[1].lo;} //C into B
void CPU::OP_42() {registers[1].hi = registers[2].hi;} //D into B
void CPU::OP_43() {registers[1].hi = registers[2].lo;} //etc
void CPU::OP_44() {registers[1].hi = registers[3].hi;}
void CPU::OP_45() {registers[1].hi = registers[3].lo;}
void CPU::OP_46() {registers[1].hi = memory[findAddr(3)]; timer--;} //Memory address pointed to by HL into B
void CPU::OP_47() {registers[1].hi = registers[0].hi;}

void CPU::OP_48() {registers[1].lo = registers[1].hi;} //Same as above but w/ C instead of B for these
void CPU::OP_49() {registers[1].lo = registers[1].lo;}
void CPU::OP_4A() {registers[1].lo = registers[2].hi;}
void CPU::OP_4B() {registers[1].lo = registers[2].lo;}
void CPU::OP_4C() {registers[1].lo = registers[3].hi;}
void CPU::OP_4D() {registers[1].lo = registers[3].lo;}
void CPU::OP_4E() {registers[1].lo = memory[findAddr(3)]; timer--;}
void CPU::OP_4F() {registers[1].lo = registers[0].hi;}

void CPU::OP_50() {registers[2].hi = registers[1].hi;} //Next verse, same as the first... (but with D)
void CPU::OP_51() {registers[2].hi = registers[1].lo;}
void CPU::OP_52() {registers[2].hi = registers[2].hi;}
void CPU::OP_53() {registers[2].hi = registers[2].lo;}
void CPU::OP_54() {registers[2].hi = registers[3].hi;}
void CPU::OP_55() {registers[2].hi = registers[3].lo;}
void CPU::OP_56() {registers[2].hi = memory[findAddr(3)]; timer--;}
void CPU::OP_57() {registers[2].hi = registers[0].hi;}

void CPU::OP_58() {registers[2].lo = registers[1].hi;} //E
void CPU::OP_59() {registers[2].lo = registers[1].lo;}
void CPU::OP_5A() {registers[2].lo = registers[2].hi;}
void CPU::OP_5B() {registers[2].lo = registers[2].lo;}
void CPU::OP_5C() {registers[2].lo = registers[3].hi;}
void CPU::OP_5D() {registers[2].lo = registers[3].lo;}
void CPU::OP_5E() {registers[2].lo = memory[findAddr(3)]; timer--;}
void CPU::OP_5F() {registers[2].lo = registers[0].hi;}

void CPU::OP_60() {registers[3].hi = registers[1].hi;} //H
void CPU::OP_61() {registers[3].hi = registers[1].lo;}
void CPU::OP_62() {registers[3].hi = registers[2].hi;}
void CPU::OP_63() {registers[3].hi = registers[2].lo;}
void CPU::OP_64() {registers[3].hi = registers[3].hi;}
void CPU::OP_65() {registers[3].hi = registers[3].lo;}
void CPU::OP_66() {registers[3].hi = memory[findAddr(3)]; timer--;}
void CPU::OP_67() {registers[3].hi = registers[0].hi;}

void CPU::OP_68() {registers[3].lo = registers[1].hi;} //I'm so bored. Also L
void CPU::OP_69() {registers[3].lo = registers[1].lo;}
void CPU::OP_6A() {registers[3].lo = registers[2].hi;}
void CPU::OP_6B() {registers[3].lo = registers[2].lo;}
void CPU::OP_6C() {registers[3].lo = registers[3].hi;}
void CPU::OP_6D() {registers[3].lo = registers[3].lo;}
void CPU::OP_6E() {registers[3].lo = memory[findAddr(3)]; timer--;}
void CPU::OP_6F() {registers[3].lo = registers[0].hi;}

void CPU::OP_70() {memory[findAddr(3)] = registers[1].hi; timer--;} //Pointed to by HL
void CPU::OP_71() {memory[findAddr(3)] = registers[1].lo; timer--;}
void CPU::OP_72() {memory[findAddr(3)] = registers[2].hi; timer--;}
void CPU::OP_73() {memory[findAddr(3)] = registers[2].lo; timer--;}
void CPU::OP_74() {memory[findAddr(3)] = registers[3].hi; timer--;}
void CPU::OP_75() {memory[findAddr(3)] = registers[3].lo; timer--;}
void CPU::OP_76() {exit(76);} //Hey, this one's different! It's actually the halt code! Interesting! Not really implemented yet though, just gonna exit.
void CPU::OP_77() {memory[findAddr(3)] = registers[0].hi; timer--;}

void CPU::OP_78() {registers[0].hi = registers[1].hi;} //A
void CPU::OP_79() {registers[0].hi = registers[1].lo;}
void CPU::OP_7A() {registers[0].hi = registers[2].hi;}
void CPU::OP_7B() {registers[0].hi = registers[2].lo;}
void CPU::OP_7C() {registers[0].hi = registers[3].hi;}
void CPU::OP_7D() {registers[0].hi = registers[3].lo;}
void CPU::OP_7E() {registers[0].hi = memory[findAddr(3)]; timer--;}
void CPU::OP_7F() {registers[0].hi = registers[0].hi;} //...and scene. Onto the adds
