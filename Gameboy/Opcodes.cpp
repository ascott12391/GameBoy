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
}

bool halt()
{
    incPC(1);
    cycles++;
    return true;
} //Maybe better way to do this, but for now...

void SCF() //Set carry flag. Hey look, an easy one
{
    setC();
    zeroH();
    zeroN();
    incPC(1);
    cycles++;
}

void CCF() //Flip carry flag. Another easy one
{
    Fc()?zeroC():setC();
    zeroH();
    zeroN();
    incPC(1);
    cycles++;
}


//JUMPS
void JPNN() //Jump to 16 bit immediate
{
    setPC(read_word(getPC()+1));
    cycles += 4;
}

void JRs8() //Jump 8 steps ahead in the PC
{
    int8_t offset = int8_t(read_byte(getPC() + 1)); //These are signed
    setPC(getPC()+2+offset);
    cycles += 3;
}

void JRNZs8() //Jump some steps ahead if zero flag is 0
{
    int8_t offset = int8_t(read_byte(getPC() + 1)); //These are signed
    if (Z()) {incPC(2); cycles+=3;}
    else {   setPC(getPC()+2+offset); cycles+=2;}
    
}

void JRNZs8() //Jump some steps ahead if carry flag is 0
{
    int8_t offset = int8_t(read_byte(getPC() + 1)); //These are signed
    if (Fc()) {incPC(2); cycles+=3;}
    else {   setPC(getPC()+2+offset); cycles+=2;}
    
}

void JRZs8() //Jump some steps ahead if zero flag is 1
{
    int8_t offset = int8_t(read_byte(getPC() + 1)); //These are signed
    if (Z()) {setPC(getPC()+2+offset); cycles+=3;}
    else {incPC(2); cycles+=2;}
}

void JRCs8() //Jump some steps ahead if carry flag is 1
{
    int8_t offset = int8_t(read_byte(getPC() + 1)); //These are signed
    if (Fc()) {setPC(getPC()+2+offset); cycles+=3;}
    else {incPC(2); cycles+=2;}
}

void Calla16() //Jump with return to the immediate 16 bit address
{
    write_byte(getSP(),(getPC()+3)>>8);
    changeSP(-1);
    write_byte(SP, (getPC()+3)&0xFF);
    changeSP(-1);
    setPC(read_word(getPC()+1));
    cycles+=6;
}

void Ret() //Return
{
    uint8_t lo = read_byte(getSP());
    changeSP(1);
    uint8_t hi = read_byte(getSP());
    changeSP(1);
    setPC((hi << 8) | lo);
    cycles += 4;
}


//LOADS
void LD_rr(uint8_t opcode) //Load something into something else
{//This will actually write to F not A. I am way too braindead to fix now [asdf]
    int src = opcode&7; //Last three bits determine where load is coming from
    int dest = (opcode >> 3)&7; //Next three determine source
    if (src == 6) //This and the next one are cases for loading something into the address POINTED to by HL
    {
        writeSmallReg(dest, read_byte(readReg(H)));
        cycles += 2;
    }
    else if (dest == 6) //I know what you may be wondering. "But Alex, what if you have an opcode that reads HL into HL?"
    {
        write_byte(readReg(H), reg_ret(src)); //We don't. Instead we have the HALT command there.
        cycles += 2;
    }
    else
    {
        writeSmallReg(dest, reg_ret(src)); //Don't ask me why load B into B or whatever is a thing though (it is)
        cycles++;
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
    }    
    else if (dest == 7)
    {
        writeSmallReg(A, read_byte(getPC()+1));
        cycles += 2;
    }
    else
    {
        writeSmallReg(dest, read_byte(getPC()+1));
        cycles += 2;
    }
    incPC(2);
}

void LDAa16() //Load the contents specificed by the immediate 16 bit reg into A
{
    writeSmallReg(A, read_word(getPC()+1));
    incPC(3);
    cycles += 4;
}

void LDa16A() //Opposite of above
{
    write_byte(read_word(getPC()+1), reg_ret(A));
    incPC(3);
    cycles += 4;
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
            write_byte(SP, val >> 8);
            changeSP(-1);
            write_byte(SP, val & 0xFF);
            changeSP(-1);
            break;
        default:
            throw std::runtime_error("Invalid register");
    }
    incPC(3);
    cycles += 3;
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
        default:
            write_byte(readReg(src), reg_ret(A)); //Needs to be updated for ENUMS [fixed]
    }
    incPC(1);
    cycles+=2;
}

void LD_Arr(uint8_t opcode) //Opposite of above, write into A
{
    int src = (opcode >> 4)&0xF;
    switch(src){
        case 2: //Special case for HL. It's a little weird
            writeSmallReg(A, readReg(H));
            writeReg(H, readReg(H)+1);
            break;
        case 3:
            writeSmallReg(A, readReg(H));
            writeReg(H, readReg(H)-1);
            break;
        default:
            writeSmallReg(A, readReg(src));
    }
    incPC(1);
    cycles+=2;
}

void LDHL_d8() //Load 8-bit immediate into location shown by HL
{
    write_byte(read_byte(getPC()+1), readReg(H));
    incPC(2);
    cycles += 3;
}

//MATH

void XORA () //A XOR A. The observant among you will notice this always zeros it out. Yay!
{
    writeSmallReg(A, 0x00);
    writeSmallReg(F, 0x80);
    incPC(1);
    cycles++;
}

void CPA8() //Compare A to the 8-bit immediate, if they are the same, set 0 flag. Does not change data
{
    int comp = reg_ret(A) - read_byte(getPC()+1);
    if (comp == 0) {setZ();}
    else {zeroZ();}
    setN();
    cycles += 2;
    incPC(2);
}

void CPAr(uint8_t opcode) //Same as above but w/ various registers instead of immediate
{
    int src = opcode&7; //Last three bits determine what to compare
    int comp;
    if (src == 6) //This and the next one are cases for loading something into the address POINTED to by HL
    {
        comp = reg_ret(A) - read_byte(readReg(H));
        cycles += 2;
    }
    else
    {
        comp = reg_ret(A) - reg_ret(src);
        cycles++;
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
    }
    else if (dest == 7) //This one is actually A, not F. Exact same as below, but different encoding on the opcode
    {
        data = reg_ret(A);
        if ((data &0xF)== 0xFF) {setH();}
        else {zeroH();}
        data++;
        writeSmallReg(A, data);
        cycles++;
    }
    else
    {
        data = reg_ret(dest);
        if ((data &0xF)== 0xFF) {setH();}
        else {zeroH();}
        data++;
        writeSmallReg(dest, data);
        cycles++;
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
    }
    else if (dest == 7)
    {
        data = reg_ret(A);
        if ((data &0xF) == 0x00) {setH();}
        else {zeroH();}
        data--;
        writeSmallReg(A, data);
        cycles++;
    }
    else
    {
        data = reg_ret(dest);
        if ((data &0xF) == 0x00) {setH();}
        else {zeroH();}
        data--;
        writeSmallReg(dest, data);
        cycles++;
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

    ((a & 0xF) < ((value & 0xF) + carry))) ? setH() : zeroH();
    (a < (value + carry)) ? setC() : zeroC();
    ((result & 0xFF) == 0) ? setZ() : zeroZ();
    setN();

    writeSmallReg(A, result & 0xFF);

    incPC(1);
    cycles += 1;
}

void compares_rr(uint8_t opcode) //Subs regs, but only sets flags. Nothing in A is affected
{
    uint8_t a = reg_ret(A);
    uint8_t carry = (opcode & 0x08) ? Fc() : 0; //This checks if carry is needed. It is for 0x97-0x9F
    uint8_t value;
    int dest = opcode & 0x07;

    if (dest == 6) // Thingie pointed to by HL
    {
        value = read_byte(readReg(H));
        cycles += 1; // extra cycle for memory access
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

    ((a & 0xF) < ((value & 0xF) + carry))) ? setH() : zeroH();
    (a < (value + carry)) ? setC() : zeroC();
    ((result & 0xFF) == 0) ? setZ() : zeroZ();
    setN();


    incPC(1);
    cycles += 1;
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
}

void inc_RR(uint8_t opcode) //Time for the whole registers. Supposedly doesn't check flags, so...
{
    int src = (opcode >> 4)&0x3;
    switch(src){
        case 3: //Stack Pointer
            changeSP(1);
            break;
        default:
            writeReg(src, readReg(src)+1);
    }
    incPC(1);
    cycles +=2;
}

void dec_RR(uint8_t opcode) //And now we subtract
{
    int src = (opcode >> 4)&0x3;
    switch(src){
        case 3: //Stack Pointer
            changeSP(-1);
            break;
        default:
            writeReg(src, readReg(src)-1);
    }
    incPC(1);
    cycles +=2;
}

void add_HLrr(uint8_t opcode) //Add some register pair into HL
{
    int dest = (opcode >> 4)&0x3;
    uint32_t result;
    switch(dest){
        case 3:
            result = readReg(2) + getSP();
            (readReg(2)&0x0FFF)+(getSP()&0x0FFF)>0x0FFF?setH():zeroH();
            break;
        default:
            result = readReg(dest) + readReg(2);
            (readReg(2)&0x0FFF)+(readReg(dest)&0x0FFF)>0xFFFF?setH():zeroH();
    }
    result>0xFFFF?setC():zeroC();
    zeroN();
    writeReg(2,result& 0xFFFF);
    incPC(1);
    cycles += 2;
}

void DAA() //converts A into BCD. This one was a pain
{
    if (!N()) {
        if (Fc() || reg_ret(A) > 0x99) { writeSmallReg(A, reg_ret(A)+0x60); setC();}
        if (Hc() || (reg_ret(A) & 0x0F) > 0x09) { writeSmallReg(A, reg_ret(A)+0x6); }
    }
    else {
      if (Fc()) { writeSmallReg(A, reg_ret(A)-0x60); }
      if (Hc()) { writeSmallReg(A, reg_ret(A)-0x06); }
    }
    
    reg_ret(A) == 0?setZ():zeroZ();
    zeroH();
    incPC(1);
    cycles++;
}

void CPL() //Flip A's bits
{
    writeSmallReg(A, reg_ret(A)^0xFF);
    setN();
    setH();
    incPC(1);
    cycles++;
}

void INCHL() //Increase the contents of memory specified by HL
{
    read_byte(readReg(H))+1 > 0xFF?setH():zeroH();
    write_byte(readReg(H), read_byte(readReg(H))+1);
    zeroN();
    read_byte(readReg(H)) == 0?setZ():zeroZ();
    incPC(3);
    cycles+=3;
}

void DECHL() //Increase the contents of memory specified by HL
{
    read_byte(readReg(H))-1 < 0?setH():zeroH();
    write_byte(readReg(H), read_byte(readReg(H))-1);
    setN();
    read_byte(readReg(H)) == 0?setZ():zeroZ();
    incPC(3);
    cycles+=3;
}


//STACK

void pushrr(uint8_t opcode) //Push 16-bit reg onto stack
{
    switch((opcode>>3)&0x7){ //Src is encoded in opcode, goes up by 2s
        case 0:
            write_byte(getSP(), reg_ret(B)); //Also little-endian drives me insane sometimes, just thought I'd mention this after many, many mistakes
            changeSP(-1);
            write_byte(getSP(), reg_ret(C));
            changeSP(-1);
            break;
        case 2:
            write_byte(getSP(), reg_ret(D));
            changeSP(-1);
            write_byte(getSP(), reg_ret(E));
            changeSP(-1);
            break;
        case 4:
            write_byte(getSP(), reg_ret(H));
            changeSP(-1);
            write_byte(getSP(), reg_ret(L));
            changeSP(-1);
            break;
        case 6:
            write_byte(getSP(), reg_ret(A));
            changeSP(-1);
            write_byte(getSP(), reg_ret(F));
            changeSP(-1);
            break;
        default:
            throw std::runtime_error("Invalid register");
    }
    incPC(1);
    cycles += 4;
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
            writeSmallReg(D,read_byte(getSP()));
            changeSP(1);
            writeSmallReg(E, read_byte(getSP()));
            changeSP(1);
            break;
        case 4:
            writeSmallReg(H,read_byte(getSP()));
            changeSP(1);
            writeSmallReg(L, read_byte(getSP()));
            changeSP(1);
            break;
        case 6:
            writeSmallReg(F,read_byte(getSP())); //Pretty sure that lower 4 bits of F always need to be zero
            changeSP(1); //And I'm not sure why you would want to directly store in a flag register
            writeSmallReg(A,read_byte(getSP())); //Seems like a way to break something
            changeSP(1); //Oh well, not really my problem
            break;
        default:
            throw std::runtime_error("Invalid register");
    }
    incPC(1);
    cycles += 4;
}

void LD_a16SP() //Load stack pointer into 16-bit address given by next 2 bytes
{
    uint16_t addr = read_byte(getPC() + 1) | (read_byte(getPC() + 2) << 8);
    write_byte(addr, SP&0xFF);
    write_byte(addr+1, SP >> 8);
    incPC(3);
    cycles+=5;
}

//Rotates

void RCLA() //Rotate A left with a carry
{
    uint8_t carry = readReg(A) >> 7;
    writeSmallReg(A, (readReg(A) << 1) | carry);
    writeSmallReg(F, carry << 4); //F is reset except for carry flag
    incPC(1);
    cycles++;
}

void RCRA() //Rotate A right with a carry
{
    uint8_t carry = readReg(A) << 7;
    writeSmallReg(A, (readReg(A)>>1) | carry);
    writeSmallReg(F, carry >> 4);
    incPC(1);
    cycles++;
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
}
