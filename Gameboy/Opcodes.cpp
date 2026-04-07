//
//  Opcodes.cpp
//  Gameboy
//
//  Created by Alex on 1/27/26.
//

#include "Opcodes.hpp"


int cycles = 0;
//MISC
void NOOP() //No operation
{
    incPC(1);
    cycles++;
    for (int i = 0; i < 4; i++)
    {
        doTimers();
    }
}

bool halt()
{
    incPC(1);
    cycles++;
    for (int i = 0; i < 4; i++)
    {
        doTimers();
    }
    return true;
} //Maybe better way to do this, but for now...

void SCF() //Set carry flag. Hey look, an easy one
{
    setC();
    zeroH();
    zeroN();
    incPC(1);
    for (int i = 0; i < 4; i++)
    {
        doTimers();
    }
    cycles++;
}

void CCF() //Flip carry flag. Another easy one
{
    Fc()?zeroC():setC();
    zeroH();
    zeroN();
    incPC(1);
    for (int i = 0; i < 4; i++)
    {
        doTimers();
    }
    cycles++;
}

void EI() //Enables interrupts.
{
    setIME();
    incPC(1);
    for (int i = 0; i < 4; i++)
    {
        doTimers();
    }
    cycles++;
}

void DI() //Disables interrupts.
{
    resIME();
    incPC(1);    
    for (int i = 0; i < 4; i++)
    {
        doTimers();
    }
    cycles++;
}

void bit(uint8_t opcode) //Writes the compliment of some bit into the zero flag
{
    uint8_t reg = opcode & 0x7;
    uint8_t bit = (opcode >> 3) & 0x7;
    bool isHL = (reg == 6);
    if (reg == 7){reg = 6;}
    uint16_t addr;
    uint8_t data;
    if (isHL)
    {
        addr = readReg(H);
        data = read_byte(addr);
        cycles++;
        for (int i = 0; i < 4; i++)
        {
            doTimers();
        }
    }
    else{data = reg_ret(reg);}
    (data&(1<<bit))==0?setZ():zeroZ();
    zeroN();
    setH();
    cycles+=2;
    for (int i = 0; i < 8; i++)
    {
        doTimers();
    }
    incPC(2);
}

void set(uint8_t opcode) //Sets a bit
{
    uint8_t reg = opcode & 0x7;
    uint8_t bit = (opcode >> 3) & 0x7;
    bool isHL = (reg == 6);
    if (reg == 7){reg = 6;}
    uint16_t addr = 0x0;
    uint8_t data;
    if (isHL)
    {
        addr = readReg(H);
        data = read_byte(addr);
        cycles++;
        for (int i = 0; i < 4; i++)
        {
            doTimers();
        }
    }
    else{data = reg_ret(reg);}
    data = data | (1<<bit);
    if (isHL) {write_byte(addr, data);}
    else {writeReg(reg, data);}
    zeroN();
    setH();
    cycles+=2;
    for (int i = 0; i < 8; i++)
    {
        doTimers();
    }
    incPC(2);
}

void res(uint8_t opcode) //Zeros a bit
{
    uint8_t reg = opcode & 0x7;
    uint8_t bit = (opcode >> 3) & 0x7;
    bool isHL = (reg == 6);
    if (reg == 7){reg = 6;}
    uint16_t addr = 0;
    uint8_t data;
    if (isHL)
    {
        addr = readReg(H);
        data = read_byte(addr);
        cycles++;
        for (int i = 0; i < 4; i++)
        {
            doTimers();
        }
    }
    else{data = reg_ret(reg);}
    data = data & ~(1<<bit);
    if (isHL) {write_byte(addr, data);}
    else {writeReg(reg, data);}
    zeroN();
    setH();
    cycles+=2;
    for (int i = 0; i < 8; i++)
    {
        doTimers();
    }
    incPC(2);
}

//JUMPS
void JPNN() //Jump to 16 bit immediate
{
    setPC(read_word(getPC()+1));
    cycles += 4;
    for (int i = 0; i < 4; i++)
    {
        doTimers();
    }
}

void JPNZa16() //Jump to 16 bit immediate if Z flag is 0
{
    if (!Z())
    {
        setPC(read_word(getPC()+1));
        cycles += 4;
        for (int i = 0; i < 16; i++)
        {
            doTimers();
        }
    }
    else
    {
        incPC(3);
        cycles+=3;
        for (int i = 0; i < 12; i++)
        {
            doTimers();
        }
    }
}

void JPNCa16() //Jump to 16 bit immediate if C flag is 0
{
    if (!Fc())
    {
        setPC(read_word(getPC()+1));
        cycles += 4;
        for (int i = 0; i < 16; i++)
        {
            doTimers();
        }
    }
    else
    {
        incPC(3);
        cycles+=3;
        for (int i = 0; i < 12; i++)
        {
            doTimers();
        }
    }
}

void JPZa16() //Jump to 16 bit immediate if Z flag is 1
{
    if (Z())
    {
        setPC(read_word(getPC()+1));
        cycles += 4;
        for (int i = 0; i < 16; i++)
        {
            doTimers();
        }
    }
    else
    {
        incPC(3);
        cycles+=3;
        for (int i = 0; i < 12; i++)
        {
            doTimers();
        }
    }
}

void JPCa16() //Jump to 16 bit immediate if C flag is 1
{
    if (Fc())
    {
        setPC(read_word(getPC()+1));
        cycles += 4;
        for (int i = 0; i < 16; i++)
        {
            doTimers();
        }
    }
    else
    {
        incPC(3);
        cycles+=3;
        for (int i = 0; i < 12; i++)
        {
            doTimers();
        }
    }
}

void JRs8() //Jump some steps ahead in the PC
{
    int8_t offset = int8_t(read_byte(getPC() + 1)); //These are signed
    setPC(getPC()+2+offset);
    cycles += 3;
    for (int i = 0; i < 12; i++)
    {
        doTimers();
    }
}

void JRNZs8() //Jump some steps ahead if zero flag is 0
{
    int8_t offset = int8_t(read_byte(getPC() + 1)); //These are signed
    if (Z()) {
        incPC(2); 
        cycles+=2;
        for (int i = 0; i < 8; i++)
        {
            doTimers();
        }
    }
    else {
        setPC(getPC()+2+offset); 
        cycles+=3;
        for (int i = 0; i < 12; i++)
        {
            doTimers();
        }
    }
    
}

void JRNCs8() //Jump some steps ahead if carry flag is 0
{
    int8_t offset = int8_t(read_byte(getPC() + 1)); //These are signed
    if (Fc()) {
        incPC(2); 
        cycles+=2;
        for (int i = 0; i < 8; i++)
        {
            doTimers();
        }
    }
    else {
        setPC(getPC()+2+offset);
        cycles+=3;
        for (int i = 0; i < 12; i++)
        {
            doTimers();
        }
    }
    
}

void JRZs8() //Jump some steps ahead if zero flag is 1
{
    int8_t offset = int8_t(read_byte(getPC() + 1)); //These are signed
    if (Z()) {
        setPC(getPC()+2+offset);
        cycles+=3;
        for (int i = 0; i < 12; i++)
        {
            doTimers();
        }
    }
    else {
        incPC(2);
        cycles+=2;
        for (int i = 0; i < 8; i++)
        {
            doTimers();
        }
    }
}

void JRCs8() //Jump some steps ahead if carry flag is 1
{
    int8_t offset = int8_t(read_byte(getPC() + 1)); //These are signed
    if (Fc()) {
        setPC(getPC()+2+offset); 
        cycles+=3;
        for (int i = 0; i < 12; i++)
        {
            doTimers();
        }
    }
    else {
        incPC(2);
        cycles+=2;
        for (int i = 0; i < 8; i++)
        {
            doTimers();
        }
    }
}

void Calla16() //Jump with return to the immediate 16 bit address
{
    uint16_t ret = getPC() + 3;
    changeSP(-1);
    write_byte(getSP(), (ret >> 8) & 0xFF);
    changeSP(-1);
    write_byte(getSP(), ret & 0xFF);
    setPC(read_word(getPC()+1));
    cycles+=6;
    for (int i = 0; i < 24; i++)
    {
        doTimers();
    }
}

void CALLNZa16() //You should get the idea by now, do the thing if the thing is zero
{
    if (!Z())
    {
        uint16_t ret = getPC() + 3;
        changeSP(-1);
        write_byte(getSP(), (ret >> 8) & 0xFF);
        changeSP(-1);
        write_byte(getSP(), ret & 0xFF);
        setPC(read_word(getPC()+1));
        cycles+=6;
        for (int i = 0; i < 24; i++)
        {
            doTimers();
        }
    }
    else
    {
        incPC(3);
        cycles+=3;
        for (int i = 0; i < 12; i++)
        {
            doTimers();
        }
    }
}

void CALLNCa16() //You should get the idea by now, do the thing if the thing is zero
{
    if (!Fc())
    {
        uint16_t ret = getPC() + 3;
        changeSP(-1);
        write_byte(getSP(), (ret >> 8) & 0xFF);
        changeSP(-1);
        write_byte(getSP(), ret & 0xFF);
        setPC(read_word(getPC()+1));
        cycles+=6;
        for (int i = 0; i < 24; i++)
        {
            doTimers();
        }
    }
    else
    {
        incPC(3);
        cycles+=3;
        for (int i = 0; i < 12; i++)
        {
            doTimers();
        }
    }
}

void CALLZa16() //You should get the idea by now, do the thing if the thing is one
{
    if (Z())
    {
        uint16_t ret = getPC() + 3;
        changeSP(-1);
        write_byte(getSP(), (ret >> 8) & 0xFF);
        changeSP(-1);
        write_byte(getSP(), ret & 0xFF);
        setPC(read_word(getPC()+1));
        cycles+=6;
        for (int i = 0; i < 24; i++)
        {
            doTimers();
        }

    }
    else
    {
        incPC(3);
        cycles+=3;
        for (int i = 0; i < 12; i++)
        {
            doTimers();
        }
    }
}

void CALLCa16() //You should get the idea by now, do the thing if the thing is one
{
    if (Fc())
    {
        uint16_t ret = getPC() + 3;
        changeSP(-1);
        write_byte(getSP(), (ret >> 8) & 0xFF);
        changeSP(-1);
        write_byte(getSP(), ret & 0xFF);
        setPC(read_word(getPC()+1));
        cycles+=6;
        for (int i = 0; i < 24; i++)
        {
            doTimers();
        }
    }
    else
    {
        incPC(3);
        cycles+=3;
        for (int i = 0; i < 12; i++)
        {
            doTimers();
        }
    }
}

void Ret() //Return
{
    uint8_t lo = read_byte(getSP());
    changeSP(1);
    uint8_t hi = read_byte(getSP());
    changeSP(1);
    setPC((hi << 8) | lo);
    cycles += 4;
    for (int i = 0; i < 16; i++)
    {
        doTimers();
    }
}

void RETI() //Return but enable interrupts
{
    setIME();
    uint8_t lo = read_byte(getSP());
    changeSP(1);
    uint8_t hi = read_byte(getSP());
    changeSP(1);
    setPC((hi << 8) | lo);
    cycles += 4;
    for (int i = 0; i < 16; i++)
    {
        doTimers();
    }
}

void ret_nz() //Return if the Z flag is 0
{
    if (!Z())
    {
        uint8_t lo = read_byte(getSP());
        changeSP(1);
        uint8_t hi = read_byte(getSP());
        changeSP(1);
        setPC((hi << 8) | lo);
        cycles += 5;
        for (int i = 0; i < 20; i++)
        {
            doTimers();
        }
    }
    else
    {
        incPC(1);
        cycles+=2;
        for (int i = 0; i < 8; i++)
        {
            doTimers();
        }
    }
}

void ret_nc() //Return if the C flag is 0
{
    if (!Fc())
    {
        uint8_t lo = read_byte(getSP());
        changeSP(1);
        uint8_t hi = read_byte(getSP());
        changeSP(1);
        setPC((hi << 8) | lo);
        cycles += 5;
        for (int i = 0; i < 20; i++)
        {
            doTimers();
        }
    }
    else
    {
        incPC(1);
        cycles+=2;
        for (int i = 0; i < 8; i++)
        {
            doTimers();
        }
    }
}

void ret_z() //Return if the Z flag is 1
{
    if (Z())
    {
        uint8_t lo = read_byte(getSP());
        changeSP(1);
        uint8_t hi = read_byte(getSP());
        changeSP(1);
        setPC((hi << 8) | lo);
        cycles += 5;
        for (int i = 0; i < 20; i++)
        {
            doTimers();
        }
    }
    else
    {
        incPC(1);
        cycles+=2;
        for (int i = 0; i < 8; i++)
        {
            doTimers();
        }
    }
}

void ret_c() //Return if the C flag is 1
{
    if (Fc())
    {
        uint8_t lo = read_byte(getSP());
        changeSP(1);
        uint8_t hi = read_byte(getSP());
        changeSP(1);
        setPC((hi << 8) | lo);
        cycles += 5;
        for (int i = 0; i < 20; i++)
        {
            doTimers();
        }
    }
    else
    {
        incPC(1);
        cycles+=2;
        for (int i = 0; i < 8; i++)
        {
            doTimers();
        }
    }
}

void RST(uint8_t opcode) //This one looks weird (at least it does to me), basically just jumps to a specific address in memory encoded by the opcode itself
{
    uint16_t addr = opcode & 0x38;

    uint16_t pc = getPC() + 1;

    changeSP(-1);
    write_byte(getSP(), pc & 0xFF);

    changeSP(-1);
    write_byte(getSP(), (pc >> 8) & 0xFF);

    setPC(addr);

    cycles += 4;
    for (int i = 0; i < 16; i++)
    {
        doTimers();
    }
}

void JPHL() //Jump to the address in HL. Pretty easy
{
    setPC(readReg(H));
    cycles++;
    for (int i = 0; i < 4; i++)
    {
        doTimers();
    }

}


//LOADS
void LD_rr(uint8_t opcode) //Load something into something else
{//This will actually write to F not A. I am way too braindead to fix now [fixed]
    int src = opcode&7; //Last three bits determine where load is coming from
    int dest = (opcode >> 3)&7; //Next three determine source
    if (src == 6) //This and the next one are cases for loading something into the address POINTED to by HL
    {
        if (dest == 7){dest = 6;}
        writeSmallReg(dest, read_byte(readReg(H)));
        cycles += 2;
        for (int i = 0; i < 8; i++)
        {
            doTimers();
        }
    }
    else if (dest == 6) //I know what you may be wondering. "But Alex, what if you have an opcode that reads HL into HL?"
    {
        if (src == 7){src = 6;}
        write_byte(readReg(H), reg_ret(src)); //We don't. Instead we have the HALT command there.
        cycles += 2;
        for (int i = 0; i < 8; i++)
        {
            doTimers();
        }
    }
    else
    {
        if (dest == 7){dest = 6;}
        if (src == 7){src = 6;}
        writeSmallReg(dest, reg_ret(src)); //Don't ask me why load B into B or whatever is a thing though (it is)
        cycles++;
        for (int i = 0; i < 8; i++)
        {
            doTimers();
        }
    }
    incPC(1);
}

void LD_d8(uint8_t opcode) //Load immediate 8-bit operand into w/e
{
    int dest = (opcode >> 3)&7; //These bits encode destination reg. As above 6 is a memory address, not an actual reg.
    if (dest == 6)
    {
        write_byte(readReg(H), read_byte(getPC()+1));
        cycles += 3;
        for (int i = 0; i < 12; i++)
        {
            doTimers();
        }
    }    
    else if (dest == 7)
    {
        writeSmallReg(A, read_byte(getPC()+1));
        cycles += 2;
        for (int i = 0; i < 8; i++)
        {
            doTimers();
        }
    }
    else
    {
        writeSmallReg(dest, read_byte(getPC()+1));
        cycles += 2;
        for (int i = 0; i < 8; i++)
        {
            doTimers();
        }
    }
    incPC(2);
}

void LDAa16() //Load the contents of the address specificed by the immediate 16 bit reg into A
{
    writeSmallReg(A, read_byte(read_word(getPC()+1)));
    incPC(3);
    cycles += 4;
    for (int i = 0; i < 16; i++)
    {
        doTimers();
    }
}

void LDa16A() //Opposite of above
{
    write_byte(read_word(getPC()+1), reg_ret(A));
    incPC(3);
    cycles += 4;
    for (int i = 0; i < 16; i++)
    {
        doTimers();
    }
}

void LD_d16(uint8_t opcode) //Load 16 bit immediate into shit
{
    int dest = (opcode >> 4)&0xF;
    uint16_t val = read_word(getPC()+1);
    switch(dest){
        case 0:  //Reg BC
            writeReg(B, val);
            break;
        case 1: //Reg DE
            writeReg(D, val);
            break;
        case 2: //Reg HL
            writeReg(H, val);
            break;
        case 3: //Stack Pointer
            writeReg(8, val);
            break;
        default:
            throw std::runtime_error("Invalid register");
    }
    incPC(3);
    cycles += 3;
    for (int i = 0; i < 12; i++)
    {
        doTimers();
    }
}

void LD_rrA(uint8_t opcode) //Load A into address specified by register
{
    int src = (opcode >> 4)&0xF;
    switch(src){
        case 2: //Special case for HL. It's a little weird
            write_byte(readReg(H), reg_ret(A));
            writeReg(H, readReg(H)+1);
            break;
        case 3:
            write_byte(readReg(H), reg_ret(A));
            writeReg(H, readReg(H)-1);
            break;
        case 1:
            write_byte(readReg(D), reg_ret(A));
            break;
        case 0:
            write_byte(readReg(B), reg_ret(A));
            break;
    }
    incPC(1);
    cycles+=2;
    for (int i = 0; i < 8; i++)
    {
        doTimers();
    }
}

void LD_Arr(uint8_t opcode) //Opposite of above, write into A
{
    int src = (opcode >> 4)&0xF;
    switch(src){
        case 2: //Special case for thing pointed to by HL. It's a little weird
            writeSmallReg(A, read_byte(readReg(H)));
            writeReg(H, readReg(H)+1);
            break;
        case 3:
            writeSmallReg(A, read_byte(readReg(H)));
            writeReg(H, readReg(H)-1);
            break;
        case 1:
            writeSmallReg(A, read_byte(readReg(D)));
            break;
        case 0:
            writeSmallReg(A, read_byte(readReg(B)));
            break;
            
    }
    incPC(1);
    cycles+=2;
    for (int i = 0; i < 8; i++)
    {
        doTimers();
    }
}

void LDHL_d8() //Load 8-bit immediate into location shown by HL
{
    write_byte(readReg(H), read_byte(getPC()+1));
    incPC(2);
    cycles += 3;
    for (int i = 0; i < 12; i++)
    {
        doTimers();
    }
}

void LDa8A() //Stores register A into the 8-bit immediate address
{ //Kind of interesting factoid. You may be wondering why we are using 8 bit immediate, not 16
    uint8_t offset = read_byte(getPC() + 1);
    write_byte(0xFF00 + offset, reg_ret(A)); //Well, as you can see here, the beginning for this address is always 0xFF
    incPC(2); //Neat!
    cycles+=3;
    for (int i = 0; i < 12; i++)
    {
        doTimers();
    }
}

void LDAa8() //Opposite of above
{
    uint8_t offset = read_byte(getPC() + 1);
    writeSmallReg(A, read_byte(0xFF00 + offset));
    incPC(2);
    cycles+=3;
    for (int i = 0; i < 12; i++)
    {
        doTimers();
    }
}

void LDCA() //Stores A into the register pointed to by C
{ //This is one of my favorite things about emulation. Like why C? Who knows
    write_byte(reg_ret(C)+0xFF00, reg_ret(A)); //But it's fun to think about why the developers did that
    incPC(1); //Also, as above the address always starts w/ 0xFF
    cycles += 2; //The reason for that I do know. It just has to do with the way memory is mapped
    for (int i = 0; i < 8; i++)
    {
        doTimers();
    }
}

//MATH

void XORA () //A XOR A. The observant among you will notice this always zeros it out. Yay!
{
    writeSmallReg(A, 0x00);
    writeSmallReg(F, 0x80);
    incPC(1);
    cycles++;
    for (int i = 0; i < 4; i++)
    {
        doTimers();
    }
}

void CPA8() //Compare A to the 8-bit immediate, if they are the same, set 0 flag. Does not change data
{
    int comp = reg_ret(A) - read_byte(getPC()+1);
    if (comp == 0) {setZ();}
    else {zeroZ();}
    setN();
    cycles += 2;
    incPC(2);
    for (int i = 0; i < 8; i++)
    {
        doTimers();
    }
}

void CPAr(uint8_t opcode) //Same as above but w/ various registers instead of immediate
{
    int src = opcode&7; //Last three bits determine what to compare
    int comp;
    if (src == 6) //This and the next one are cases for loading something into the address POINTED to by HL
    {
        comp = reg_ret(A) - read_byte(readReg(H));
        cycles += 2;
        for (int i = 0; i < 8; i++)
        {
            doTimers();
        }
    }
    else
    {
        comp = reg_ret(A) - reg_ret(src);
        cycles++;
        for (int i = 0; i < 4; i++)
        {
            doTimers();
        }
    }
    if (comp == 0) {setZ();}
    else {zeroZ();}
    setN();
    incPC(1);
}

void inc_rr(uint8_t opcode) //Increases the value stored in some register by one.
{
    int dest = (opcode >> 3)&7;
    uint8_t data; //Keep it here to make checking flags easier
    if (dest == 6) //Thingie pointed to by HL
    {
        data = read_byte(readReg(H));
        if ((data &0xF) == 0xF) {setH();}
        else {zeroH();}
        data++;
        write_byte(readReg(H), data);
        cycles += 3;    
        for (int i = 0; i < 12; i++)
        {
            doTimers();
        }
    }
    else if (dest == 7) //This one is actually A, not F. Exact same as below, but different encoding on the opcode
    {
        data = reg_ret(A);
        if ((data &0xF)+1 > 0xF) {setH();}
        else {zeroH();}
        data++;
        writeSmallReg(A, data);
        cycles++;
        for (int i = 0; i < 4; i++)
        {
            doTimers();
        }
    }
    else
    {
        data = reg_ret(dest);
        if ((data &0xF)+1 > 0xF) {setH();}
        else {zeroH();}
        data++;
        writeSmallReg(dest, data);
        cycles++;
        for (int i = 0; i < 4; i++)
        {
            doTimers();
        }
    }
    if (data == 0) {setZ();}
    else {zeroZ();}
    zeroN();
    incPC(1);
}

void dec_rr(uint8_t opcode) //Same as above, but decreases instead
{
    int dest = (opcode >> 3)&7;
    uint8_t data; //Keep it here to make checking flags easier
    if (dest == 6) //Thingie pointed to by HL
    {
        data = read_byte(readReg(3));
        if ((data &0xF) == 0x00) {setH();}
        else {zeroH();}
        data--;
        write_byte(readReg(3), data);
        cycles += 3;
        for (int i = 0; i < 12; i++)
        {
            doTimers();
        }
    }
    else if (dest == 7)
    {
        data = reg_ret(A);
        if ((data &0xF) == 0x00) {setH();}
        else {zeroH();}
        data--;
        writeSmallReg(A, data);
        cycles++;
        for (int i = 0; i < 4; i++)
        {
            doTimers();
        }
    }
    else
    {
        data = reg_ret(dest);
        if ((data &0xF) == 0x00) {setH();}
        else {zeroH();}
        data--;
        writeSmallReg(dest, data);
        cycles++;
        for (int i = 0; i < 4; i++)
        {
            doTimers();
        }
    }
    if (data == 0) {setZ();}
    else {zeroZ();}
    setN();
    incPC(1);
}

void add_rr(uint8_t opcode) //Add regs together, store in A
{
    uint8_t a = reg_ret(A);
    uint8_t carry = (opcode & 0x08) ? Fc() : 0; //This checks if carry is needed. It is for 0x87-0x8F
    uint8_t value;
    int dest = opcode & 0x07;

    if (dest == 6) // Thingie pointed to by HL
    {
        value = read_byte(readReg(H));
        cycles += 1; // extra cycle for memory access
        for (int i = 0; i < 4; i++)
        {
            doTimers();
        }
    }
    else if(dest == 7)
    {
        value = a; //This is when we add A to A. I have it as enum 6, so I need a special case here
    }
    else
    {
        value = reg_ret(dest);
    }

    uint16_t result = a + value + carry;

    ((a & 0xF) + (value & 0xF) + carry > 0xF) ? setH() : zeroH();
    (result > 0xFF) ? setC() : zeroC();
    ((result & 0xFF) == 0) ? setZ() : zeroZ();
    zeroN();

    writeSmallReg(A, result & 0xFF);

    incPC(1);
    cycles += 1;
    for (int i = 0; i < 4; i++)
    {
        doTimers();
    }
}

void sub_rr(uint8_t opcode) //Subs regs, store in A
{
    uint8_t a = reg_ret(A);
    uint8_t carry = (opcode & 0x08) ? Fc() : 0; //This checks if carry is needed. It is for 0x97-0x9F
    uint8_t value;
    int dest = opcode & 0x07;

    if (dest == 6) // Thingie pointed to by HL
    {
        value = read_byte(readReg(H));
        cycles += 1; // extra cycle for memory access
        for (int i = 0; i < 4; i++)
        {
            doTimers();
        }
    }
    else if(dest == 7)
    {
        value = a; //This is when we sub A to A. Mostly zero, but gets weird when carry gets involved
    }
    else
    {
        value = reg_ret(dest);
    }

    uint16_t result = a - value - carry;

    ((a & 0xF) < ((value & 0xF) + carry)) ? setH() : zeroH();
    (a < (value + carry)) ? setC() : zeroC();
    ((result & 0xFF) == 0) ? setZ() : zeroZ();
    setN();

    writeSmallReg(A, result & 0xFF);

    incPC(1);
    cycles += 1;
    for (int i = 0; i < 4; i++)
    {
        doTimers();
    }
}

void cp_rr(uint8_t opcode) //Subs regs, but only sets flags. Nothing in A is affected
{
    uint8_t a = reg_ret(A);
    uint8_t value;
    int dest = opcode & 0x07;

    if (dest == 6) // Thingie pointed to by HL
    {
        value = read_byte(readReg(H));
        cycles += 1; // extra cycle for memory access
        for (int i = 0; i < 4; i++)
        {
            doTimers();
        }
    }
    else if(dest == 7)
    {
        value = a; //This is when we sub A to A. Always 0
    }
    else
    {
        value = reg_ret(dest);
    }

    uint16_t result = a - value;

    ((a & 0xF) < (value & 0xF)) ? setH() : zeroH();
    (a < value) ? setC() : zeroC();
    ((result & 0xFF) == 0) ? setZ() : zeroZ();
    setN();


    incPC(1);
    cycles += 1;
    for (int i = 0; i < 4; i++)
    {
        doTimers();
    }
}

void and_rr(uint8_t opcode) //Ands regs, store in A
{
    uint8_t a = reg_ret(A);
    uint8_t value;
    int dest = opcode & 0x07;

    if (dest == 6) // Thingie pointed to by HL
    {
        value = read_byte(readReg(H));
        cycles += 1; // extra cycle for memory access
        for (int i = 0; i < 4; i++)
        {
            doTimers();
        }
    }
    else if(dest == 7)
    {
        value = a;
    }
    else
    {
        value = reg_ret(dest);
    }

    uint8_t result = a & value;

    setH(); //Half carry is always set on this
    zeroC();
    (result == 0) ? setZ() : zeroZ();
    zeroN();

    writeSmallReg(A, result);

    incPC(1);
    cycles += 1;
    for (int i = 0; i < 4; i++)
    {
        doTimers();
    }
}

void xor_rr(uint8_t opcode) //XORs regs, store in A
{
    uint8_t a = reg_ret(A);
    uint8_t value;
    int dest = opcode & 0x07;

    if (dest == 6) // Thingie pointed to by HL
    {
        value = read_byte(readReg(H));
        cycles += 1; // extra cycle for memory access
        for (int i = 0; i < 4; i++)
        {
            doTimers();
        }
    }
    else if(dest == 7)
    {
        value = a;
    }
    else
    {
        value = reg_ret(dest);
    }

    uint8_t result = a ^ value;

    zeroH();
    zeroC();
    (result == 0) ? setZ() : zeroZ();
    zeroN();

    writeSmallReg(A, result);

    incPC(1);
    cycles += 1;
    for (int i = 0; i < 4; i++)
    {
        doTimers();
    }
}

void or_rr(uint8_t opcode) //ORs regs, store in A
{
    uint8_t a = reg_ret(A);
    uint8_t value;
    int dest = opcode & 0x07;

    if (dest == 6) // Thingie pointed to by HL
    {
        value = read_byte(readReg(H));
        cycles += 1; // extra cycle for memory access
        for (int i = 0; i < 4; i++)
        {
            doTimers();
        }
    }
    else if(dest == 7)
    {
        value = a;
    }
    else
    {
        value = reg_ret(dest);
    }

    uint8_t result = a | value;

    zeroH();
    zeroC();
    (result == 0) ? setZ() : zeroZ();
    zeroN();

    writeSmallReg(A, result);

    incPC(1);
    cycles += 1;
    for (int i = 0; i < 4; i++)
    {
        doTimers();
    }
}

void inc_RR(uint8_t opcode) //Time for the whole registers. Supposedly doesn't check flags, so...
{
    int src = (opcode >> 4)&0x3;
    switch(src){
        case 3: //Stack Pointer
            changeSP(1);
            break;
        case 0:
            writeReg(B, readReg(B)+1);
            break;
        case 1:
            writeReg(D, readReg(D)+1);
            break;
        case 2:
            writeReg(H, readReg(H)+1);
            break;
    }
    incPC(1);
    cycles +=2;
    for (int i = 0; i < 8; i++)
    {
        doTimers();
    }
}

void dec_RR(uint8_t opcode) //And now we subtract
{
    int src = (opcode >> 4)&0x3;
    switch(src){
        case 3: //Stack Pointer
            changeSP(-1);
            break;
        case 0:
            writeReg(B, readReg(B)-1);
            break;
        case 1:
            writeReg(D, readReg(D)-1);
            break;
        case 2:
            writeReg(H, readReg(H)-1);
            break;
    }
    incPC(1);
    cycles +=2;
    for (int i = 0; i < 8; i++)
    {
        doTimers();
    }
}

void add_HLrr(uint8_t opcode) //Add some register pair into HL
{
    int dest = (opcode >> 4)&0x3;
    uint32_t result = 0;
    switch(dest){
        case 3:
            result = readReg(H) + getSP();
            (readReg(H)&0x0FFF)+(getSP()&0x0FFF)>0x0FFF?setH():zeroH();
            break;
        case 0:
            result = readReg(B) + readReg(H);
            (readReg(H)&0x0FFF)+(readReg(B)&0x0FFF)>0x0FFF?setH():zeroH();
            break;
        case 1:
            result = readReg(D) + readReg(H);
            (readReg(H)&0x0FFF)+(readReg(D)&0x0FFF)>0x0FFF?setH():zeroH();
            break;
        case 2:
            result = readReg(H) + readReg(H);
            (readReg(H)&0x0FFF)+(readReg(H)&0x0FFF)>0x0FFF?setH():zeroH();
            break;
    }
    result>0xFFFF?setC():zeroC();
    zeroN();
    writeReg(H,result& 0xFFFF);
    incPC(1);
    for (int i = 0; i < 8; i++)
    {
        doTimers();
    }
    cycles += 2;
}

void DAA() //converts A into BCD. This one was a pain
{
    if (!N()) {
        if (Fc() || reg_ret(A) > 0x99) { writeSmallReg(A, reg_ret(A)+0x60); setC();}
        if (Hc() || (reg_ret(A) & 0x0F) > 0x09) { writeSmallReg(A, reg_ret(A)+0x6); }
    }
    else {
        if (Fc()) { writeSmallReg(A, reg_ret(A)-0x60); setC();}
        if (Hc()) { writeSmallReg(A, reg_ret(A)-0x06); }
    }
    
    reg_ret(A) == 0?setZ():zeroZ();
    zeroH();
    incPC(1);
    cycles++;
    for (int i = 0; i < 4; i++)
    {
        doTimers();
    }
}

void CPL() //Flip A's bits
{
    writeSmallReg(A, reg_ret(A)^0xFF);
    setN();
    setH();
    incPC(1);
    for (int i = 0; i < 4; i++)
    {
        doTimers();
    }
    cycles++;
}

void INCHL() //Increase the contents of memory specified by HL
{
    (read_byte(readReg(H))&0xF)+1 > 0xFF?setH():zeroH();
    write_byte(readReg(H), read_byte(readReg(H))+1);
    zeroN();
    read_byte(readReg(H)) == 0?setZ():zeroZ();
    incPC(1);
    cycles+=3;
    for (int i = 0; i < 12; i++)
    {
        doTimers();
    }
}

void DECHL() //Decrease the contents of memory specified by HL
{
    (read_byte(readReg(H))&0xF)-1 < 0?setH():zeroH();
    write_byte(readReg(H), read_byte(readReg(H))-1);
    setN();
    read_byte(readReg(H)) == 0?setZ():zeroZ();
    incPC(1);
    cycles+=3;
    for (int i = 0; i < 12; i++)
    {
        doTimers();
    }
}

void ADDAd8() //Add 8 bit immediate to A
{
    uint8_t a = reg_ret(A); //Just trying to minimize function calls here
    uint8_t value = read_byte(getPC()+1);
    uint16_t result = a + value;

    ((a & 0xF) + (value & 0xF) > 0xF) ? setH() : zeroH();
    (result > 0xFF) ? setC() : zeroC();
    ((result & 0xFF) == 0) ? setZ() : zeroZ();
    zeroN();

    writeSmallReg(A, result&0xFF);
    incPC(2);
    cycles+=2;
    for (int i = 0; i < 8; i++)
    {
        doTimers();
    }
}

void SUBAd8() //Subs 8 bit immediate to A
{
    uint8_t a = reg_ret(A);
    uint8_t value = read_byte(getPC()+1);
    uint16_t result = a - value;

    ((a & 0xF) < ((value & 0xF))) ? setH() : zeroH();
    (a < (value)) ? setC() : zeroC();
    ((result & 0xFF) == 0) ? setZ() : zeroZ();
    setN();

    writeSmallReg(A, result & 0xFF);

    incPC(2);
    cycles += 2;
    for (int i = 0; i < 8; i++)
    {
        doTimers();
    }
}

void ANDAd8() //Ands 8 bit immediate to A
{
    uint8_t a = reg_ret(A); //Just trying to minimize function calls here
    uint8_t value = read_byte(getPC()+1);
    uint8_t result = a & value;

    setH();
    zeroC();
    (result== 0) ? setZ() : zeroZ();
    zeroN();

    writeSmallReg(A, result);

    incPC(2);
    cycles += 2;
    for (int i = 0; i < 8; i++)
    {
        doTimers();
    }
}

void ORAd8() //Ors 8 bit immediate to A
{
    uint8_t a = reg_ret(A); //Just trying to minimize function calls here
    uint8_t value = read_byte(getPC()+1);
    uint8_t result = a | value;

    zeroH();
    zeroC();
    ((result) == 0) ? setZ() : zeroZ();
    zeroN();

    writeSmallReg(A, result);

    incPC(2);
    cycles += 2;
    for (int i = 0; i < 8; i++)
    {
        doTimers();
    }
}

void ADCAd8() //Add 8 bit immediate with carry to A
{
    uint8_t a = reg_ret(A); //There's an argument to be had over wether I should use opcode decoding like other places
    uint8_t value = read_byte(getPC()+1); //for this set of math with A and the 8 bit immediate
    uint16_t result = a + value +Fc(); //but I feel like it'd get real ugly without too much benefit

    ((a & 0xF) + (value & 0xF) + Fc()> 0xF) ? setH() : zeroH(); //just a ton of switch statments for practically the same
    (result > 0xFF) ? setC() : zeroC(); //thing written out as functions like I have here
    ((result & 0xFF) == 0) ? setZ() : zeroZ();
    zeroN();

    writeSmallReg(A, result&0xFF);
    incPC(2);
    cycles+=2;
    for (int i = 0; i < 8; i++)
    {
        doTimers();
    }
}

void SBCAd8() //Subs 8 bit immediate to A w/ carry
{
    uint8_t a = reg_ret(A);
    uint8_t value = read_byte(getPC()+1);
    uint16_t result = a - value - Fc();

    ((a & 0xF) < ((value & 0xF) + Fc())) ? setH() : zeroH();
    (a < (value + Fc())) ? setC() : zeroC();
    ((result & 0xFF) == 0) ? setZ() : zeroZ();
    setN();

    writeSmallReg(A, result & 0xFF);

    incPC(2);
    cycles += 2;
    for (int i = 0; i < 8; i++)
    {
        doTimers();
    }
}

void XORAd8() //Ors 8 bit immediate to A
{
    uint8_t a = reg_ret(A);
    uint8_t value = read_byte(getPC()+1);
    uint8_t result = a ^ value;

    zeroH();
    zeroC();
    ((result) == 0) ? setZ() : zeroZ();
    zeroN();

    writeSmallReg(A, result);

    incPC(2);
    cycles += 2;
    for (int i = 0; i < 8; i++)
    {
        doTimers();
    }
}

void CPAd8() //Just in case you forgot, this subs, but doesn't affect A. Just flags
{
    uint8_t a = reg_ret(A); //or is it effect?
    uint8_t value = read_byte(getPC()+1);//I can never remember
    uint16_t result = a - value;//My english professor mom and Marshall Eriksen probably hate me

    ((a & 0xF) < ((value & 0xF))) ? setH() : zeroH();
    (a < (value)) ? setC() : zeroC();
    ((result & 0xFF) == 0) ? setZ() : zeroZ();
    setN();

    incPC(2);
    cycles += 2;
    for (int i = 0; i < 8; i++)
    {
        doTimers();
    }
}

void ADDSPs8() // Adds the SIGNED 8-bit immediate to SP
{
    int8_t imm = (int8_t)read_byte(getPC()+1);
    uint16_t sp = getSP();
    uint16_t result = sp + imm;

    ((sp ^ imm ^ result) & 0x10) ? setH() : zeroH();
    ((sp ^ imm ^ result) & 0x100) ? setC() : zeroC();
    zeroZ();
    zeroN();
    changeSP(imm);
    incPC(2);
    cycles+=4;
    for (int i = 0; i < 16; i++)
    {
        doTimers();
    }
}

//STACK

void pushrr(uint8_t opcode) //Push 16-bit reg onto stack
{
    switch((opcode>>3)&0x7){ //Src is encoded in opcode, goes up by 2s
        case 0:
            changeSP(-1);
            write_byte(getSP(), reg_ret(B)); //Also little-endian drives me insane sometimes, just thought I'd mention this after many, many mistakes
            changeSP(-1);
            write_byte(getSP(), reg_ret(C));
            break;
        case 2:
            changeSP(-1);
            write_byte(getSP(), reg_ret(D));
            changeSP(-1);
            write_byte(getSP(), reg_ret(E));
            break;
        case 4:
            changeSP(-1);
            write_byte(getSP(), reg_ret(H));
            changeSP(-1);
            write_byte(getSP(), reg_ret(L));
            break;
        case 6:
            changeSP(-1);
            write_byte(getSP(), reg_ret(A));
            changeSP(-1);
            write_byte(getSP(), reg_ret(F));
            break;
        default:
            throw std::runtime_error("Invalid register");
    }
    incPC(1);
    cycles += 4;
    for (int i = 0; i < 16; i++)
    {
        doTimers();
    }
}

void poprr(uint8_t opcode) //See above, but pop
{
    switch((opcode>>3)&0x7){ //Src is encoded in opcode, goes up by 2s
        case 0:
            writeSmallReg(C,read_byte(getSP()));
            changeSP(1);
            writeSmallReg(B, read_byte(getSP()));
            changeSP(1);
            break;
        case 2:
            writeSmallReg(E,read_byte(getSP()));
            changeSP(1);
            writeSmallReg(D, read_byte(getSP()));
            changeSP(1);
            break;
        case 4:
            writeSmallReg(L,read_byte(getSP()));
            changeSP(1);
            writeSmallReg(H, read_byte(getSP()));
            changeSP(1);
            break;
        case 6:
            writeSmallReg(F,read_byte(getSP())&0xF0); //Lower 4 bits of F always need to be zero
            changeSP(1); //And I'm not sure why you would want to directly store in a flag register
            writeSmallReg(A,read_byte(getSP())); //Seems like a way to break something
            changeSP(1); //Oh well, not really my problem
            break;
        default:
            throw std::runtime_error("Invalid register");
    }
    incPC(1);
    cycles += 4;
    for (int i = 0; i < 16; i++)
    {
        doTimers();
    }
}

void LD_a16SP() //Load stack pointer into 16-bit address given by next 2 bytes
{
    uint16_t addr = read_byte(getPC() + 1) | (read_byte(getPC() + 2) << 8);
    write_byte(addr, SP&0xFF);
    write_byte(addr+1, SP >> 8);
    incPC(3);
    cycles+=5;
    for (int i = 0; i < 20; i++)
    {
        doTimers();
    }
}

void LDHLSP_d8() //adds the SIGNED immediate to SP and stores in HL
{
    int8_t imm = (int8_t)read_byte(getPC()+1);
    uint16_t sp = getSP();
    uint16_t result = sp + imm;
    ((sp ^ imm ^ result) & 0x10) ? setH() : zeroH();
    ((sp ^ imm ^ result) & 0x100) ? setC() : zeroC();
    zeroZ();
    zeroN();
    writeReg(H, result);
    incPC(2);
    cycles+=3;
    for (int i = 0; i < 12; i++)
    {
        doTimers();
    }
}

void LDSPHL() //Load HL into SP
{
    setSP(readReg(H));
    incPC(1);
    cycles+=2;
    for (int i = 0; i < 8; i++)
    {
        doTimers();
    }
}

//Rotates

void RCLA() //Rotate A left with a carry
{
    uint8_t carry = readReg(A) >> 7;
    writeSmallReg(A, (readReg(A) << 1) | carry);
    writeSmallReg(F, carry << 4); //F is reset except for carry flag
    incPC(1);
    cycles++;
    for (int i = 0; i < 4; i++)
    {
        doTimers();
    }
}

void RCRA() //Rotate A right with a carry
{
    uint8_t carry = readReg(A) << 7;
    writeSmallReg(A, (readReg(A)>>1) | carry);
    writeSmallReg(F, carry >> 4);
    incPC(1);
    cycles++;
    for (int i = 0; i < 4; i++)
    {
        doTimers();
    }
}

void RLA() //This is same as above, but count the carry flag in A sorta thing. Pretty simple
{
    uint8_t oldCarry = Fc();
    zeroN(); //Flags in this one are apparantley zeroed out.
    zeroH();
    zeroZ();
    reg_ret(A) >> 7 == 1?setC():zeroC();
    writeSmallReg(A, (reg_ret(A) << 1)|oldCarry);
    incPC(1);
    cycles++;
    for (int i = 0; i < 4; i++)
    {
        doTimers();
    }
}

void RRA() //Take it right now ya'll...
{
    uint8_t oldCarry = Fc();
    zeroN(); //Flags in this one are apparantley zeroed out.
    zeroH();
    zeroZ();
    (reg_ret(A) &0x01) == 1?setC():zeroC();
    writeSmallReg(A, (reg_ret(A) >> 1)|(oldCarry<<7));
    incPC(1);
    cycles++;
    for (int i = 0; i < 4; i++)
    {
        doTimers();
    }
}

void rotate(uint8_t opcode) //Rotates or shifts regs. This is the most complicated CB opcode.
{
    uint8_t op = (opcode >> 3) & 0x7;
    uint8_t reg = opcode & 0x7;

    uint8_t data;
    uint16_t addr = 0;
    bool isHL = (reg == 6);

    if (isHL) {
        addr = readReg(H);
        data = read_byte(addr);
        cycles += 2;
        for (int i = 0; i < 8; i++)
        {
            doTimers();
        }
    } else {
        data = reg_ret(reg);
    }
    if (reg == 7){reg = 6;} //Enums kinda make this weird as A is usually 6 but sometimes 7, and I'm just un-weirding it here
    uint8_t result = 0;
    bool carryOut = false;

    switch (op) {
        case 0: //Rotate left w/ carry
            carryOut = (data & 0x80) != 0;
            result = (data << 1) | (data >> 7);
            break;

        case 1: //Rotate right w/ carry
            carryOut = (data & 0x01) != 0;
            result = (data >> 1) | (data << 7);
            break;

        case 2: //Rotate left w/o carry
            carryOut = (data & 0x80) != 0;
            result = (data << 1) | Fc();
            break;

        case 3: //Rotate right w/o carry
            carryOut = (data & 0x01) != 0;
            result = (data >> 1) | (Fc() << 7);
            break;

        case 4: //Shift left
            carryOut = (data & 0x80) != 0;
            result = data << 1;
            break;

        case 5: //Shift right, but maintain the 7th bit
            carryOut = (data & 0x01) != 0;
            result = (data >> 1) | (data & 0x80);
            break;

        case 6: //Swaps the nibbles
            carryOut = 0;
            result = (data << 4) | (data >> 4);
            break;

        case 7: //Shift right
            carryOut = (data & 0x01) != 0;
            result = data >> 1;
            break;
    }

    if (isHL) {
        write_byte(addr, result);
    } else {
        writeSmallReg(reg, result);
    }

    (result == 0) ? setZ() : zeroZ();
    zeroN();
    zeroH();
    carryOut ? setC() : zeroC();

    incPC(2);
    cycles += 2;
    for (int i = 0; i < 8; i++)
    {
        doTimers();
    }
}




/*

                                               :
                                              ::
                                             ::
                                             ::
                                            ::
                                            ::
                              __           ::
   _..-'/-¯¯--/_          ,.--. ''.     |`\=,..
-:--.---''-..  /_         |\\_\..  \    `-.=._/
.-|¯         '.  \         \_ \-`/\ |    ::`
  /  @  \      \  -_   _..--|-\¯¯``'--.-/_\
  |   .-'|      \  \\-'\_/     ¯/-.|-.\_\_/
  \_./` /        \_//-''/    .-'
       |           '-/'@====/              _.--.
   __.'             /¯¯'-. \-'.          _/   /¯'
.''____|   /       |'--\__\/-._        .'    |
 \ \_. \  |       _| -/        \-.__  /     /
  \___\ '/   _.  ('-..| /       '_  ''   _.'
        /  .'     ¯¯¯¯ /        | ``'--''
       (  / ¯```¯¯¯¯¯|-|        |
        \ \_.         \ \      /
         \___\         '.'.   /
                         /    |
                        /   .'
                       /  .' |
                     .'  / \  \
                    /___| /___'
*/
