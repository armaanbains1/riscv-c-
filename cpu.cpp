#include "cpu.h"
#include <vector>
#include <cstdint>
#include <iostream>
using namespace std;


cpu::cpu(int CPUpc, bus* b, registers* reg){
    pc = CPUpc;
    cpuBus = b;
    regMain = reg;
}

void cpu::setPC(int val){
    pc = val;
}

int cpu::getPC(){
    return pc;
}

void cpu::process(uint32_t instruction){

    //FMT Dictionary
    //1 -----> R-type
    //2 -----> I-type
    //3 -----> S-type
    //4 -----> B-type
    //5 -----> U-type
    //6 -----> J-type

    cout << instruction << endl;
    int fmtPre = instruction & 0x7F;
    cout << "fmtPRE " << fmtPre << endl;
    int fmt;
    if (fmtPre == 0b0110011){
        fmt = 1;
    }   
    else if (fmtPre == 0b0010011){
        fmt = 2;
    }
    else if (fmtPre == 0b0000011){
        fmt = 2;
    }
    else if (fmtPre == 0b0100011){
        fmt = 3;
    }
    else if (fmtPre == 0b1100011){
        fmt = 4;
    }
    else if (fmtPre == 0b1101111){
        fmt = 6;
    }
    else if (fmtPre == 0b1100111){
        fmt = 2;
    }
    else if (fmtPre == 0b0110111){
        fmt = 5;
    }
    else if (fmtPre == 0b0010111){
        fmt = 5;
    }
    else if (fmtPre == 0b1110011){
        fmt = 2;
    }

    cout << "fmt is : " << fmtPre << endl;

    uint8_t opcode = -1;
    uint8_t rd = -1;
    uint8_t func3 = -1;
    uint8_t rs1 = -1;
    uint8_t rs2 = -1;
    uint8_t func7 = -1;
    int32_t imm = 0;
    int32_t imm1 = 0;
    int32_t imm2 = 0;

    if (fmt == 1){
        opcode = instruction & 0x7F;
        instruction >>= 7;
        rd = instruction & 0x1F;
        instruction >>= 5;
        func3 = instruction & 0x7;
        instruction >>= 3;
        rs1 = instruction & 0x1F;
        instruction >>= 5;
        rs2 = instruction & 0x1F;
        instruction >>= 5;
        func7 = instruction & 0x7F;
    }

    else if (fmt == 2){
        cout << "yessuh2" << endl;
        opcode = instruction & 0x7F;
        instruction >>= 7;
        rd = instruction & 0x1F;
        instruction >>= 5;
        func3 = instruction & 0x7;
        instruction >>= 3;
        rs1 = instruction & 0x1F;
        instruction >>= 5;
        imm = instruction & 0xFFF;
        int sign = 0x1 & (instruction >> 11);
        if (sign == 0x1){
            imm |= 0xFFFFF000;
        }
    }

    else if (fmt ==3){
        cout << "yessuh3" << endl;
        opcode = instruction & 0x7F;
        instruction >>= 7;
        imm1 = instruction & 0x1F;
        instruction >>= 5;
        func3 = instruction & 0x7;
        instruction >>= 3;
        rs1 = instruction & 0x1F;
        instruction >>= 5;
        rs2 = instruction & 0x1F;
        instruction >>= 5;
        imm2 = instruction & 0x7F;
        imm2 <<= 5;
        imm = imm1 | imm2;
        int sign = 0x1 & (imm >> 11);
        if (sign == 0x1){
            imm |= 0xFFFFF000;
        }        
    }

    else if (fmt == 4) { 
        // 1. Pull the Opcode (Bits 0-6)
        opcode = instruction & 0x7F;
        instruction >>= 7;

        // 2. Pull imm[11] and imm[4:1] (Bits 7-11)
        imm1 = instruction & 0x1F; 
        instruction >>= 5;

        // 3. Pull func3 (Bits 12-14)
        func3 = instruction & 0x7;
        instruction >>= 3;

        // 4. Pull rs1 and rs2 (Bits 15-24)
        rs1 = instruction & 0x1F;
        instruction >>= 5;
        rs2 = instruction & 0x1F;
        instruction >>= 5;

        // 5. Pull imm[12] and imm[10:5] (Bits 25-31)
        imm2 = instruction & 0x7F;

        // --- RECONSTRUCT THE IMM ---
        // Extract the individual scrambled bits from your chunks
        int32_t bit11      = (imm1 & 0x1);        // Bit 7 of original
        int32_t bits4to1   = (imm1 >> 1) & 0xF;   // Bits 8-11 of original
        int32_t bits10to5  = (imm2 & 0x3F);       // Bits 25-30 of original
        int32_t bit12      = (imm2 >> 6) & 0x1;   // Bit 31 of original

        // Stitch them into the final 13-bit branch offset
        imm = (bit12 << 12) | (bit11 << 11) | (bits10to5 << 5) | (bits4to1 << 1);

        // 6. Sign Extend (from bit 12)
        if (imm & 0x1000) {
            imm |= 0xFFFFE000;
        }
    }

    else if (fmt == 5){
        opcode = instruction & 0x7F;
        instruction >>= 7;
        rd = instruction & 0x1F;
        instruction >>= 5;
        imm = instruction & 0xFFFFF;
        imm <<= 12;
    }

    else if (fmt == 6) { 
        cout << "yuh" << endl;
        opcode = instruction & 0x7F;
        rd = (instruction >> 7) & 0x1F;
        uint32_t jImm = (instruction >> 12) & 0xFFFFF;

        int32_t twenty = (jImm >> 19) & 0x1;
        int32_t nineteenToTwelve = jImm & 0xFF;
        int32_t eleventh = (jImm >> 8) & 0x1;
        int32_t tenToOne = (jImm >> 9) & 0x3FF;

        imm = twenty;
        imm <<= 8;
        imm |= nineteenToTwelve;
        imm <<= 1;
        imm |= eleventh;
        imm <<= 10;
        imm |= tenToOne;
        imm <<= 1;

        int sign = (imm >> 20) & 0x1;
        if (sign == 0x1) {
            imm |= 0xFFE00000;
        }
    }

    //R-type instructions

    //The add function
    //std::cout << fmt << " +  " << func3 << " +  " << func7 << "ENd" << endl;
    if (fmt == 1 && func3 == 0 && func7 == 0){
        std::cout << "ENTERED" << std::endl;
        uint32_t r1 = regMain->readRS1(rs1);
        //std::cout << r1 << std::endl;
        uint32_t r2 = regMain->readRS2(rs2);
        //std::cout << r2 << std::endl;
        uint32_t result = r1+r2;
        regMain->writeRD(rd, result);
    }

    //Subtract

    if (fmt == 1 && func3 == 0 && func7 == 0x20){
        std::cout << "ENTERED" << std::endl;
        uint32_t r1 = regMain->readRS1(rs1);
        //std::cout << r1 << std::endl;
        uint32_t r2 = regMain->readRS2(rs2);
        //std::cout << r2 << std::endl;
        uint32_t result = r1-r2;
        regMain->writeRD(rd, result);
    }

    //XOR

    if (fmt == 1 && func3 == 0x4 && func7 == 0){
        std::cout << "ENTERED" << std::endl;
        uint32_t r1 = regMain->readRS1(rs1);
        //std::cout << r1 << std::endl;
        uint32_t r2 = regMain->readRS2(rs2);
        //std::cout << r2 << std::endl;
        uint32_t result = r1 ^ r2;
        regMain->writeRD(rd, result);
    }


    //OR

    if (fmt == 1 && func3 == 0x6 && func7 == 0){
        std::cout << "ENTERED" << std::endl;
        uint32_t r1 = regMain->readRS1(rs1);
        //std::cout << r1 << std::endl;
        uint32_t r2 = regMain->readRS2(rs2);
        //std::cout << r2 << std::endl;
        uint32_t result = r1 | r2;
        regMain->writeRD(rd, result);
    }

    //AND

    if (fmt == 1 && func3 == 0x7 && func7 == 0){
        std::cout << "ENTERED" << std::endl;
        uint32_t r1 = regMain->readRS1(rs1);
        //std::cout << r1 << std::endl;
        uint32_t r2 = regMain->readRS2(rs2);
        //std::cout << r2 << std::endl;
        uint32_t result = r1 & r2;
        regMain->writeRD(rd, result);
    }


    //Shift Left Logical

    if (fmt == 1 && func3 == 0x1 && func7 == 0){
        std::cout << "ENTERED" << std::endl;
        uint32_t r1 = regMain->readRS1(rs1);
        //std::cout << r1 << std::endl;
        uint32_t r2 = regMain->readRS2(rs2);
        //std::cout << r2 << std::endl;
        uint32_t result = r1 << r2;
        regMain->writeRD(rd, result);
    }

    //Shift Right Logical

    if (fmt == 1 && func3 == 0x5 && func7 == 0){
        std::cout << "ENTERED" << std::endl;
        uint32_t r1 = regMain->readRS1(rs1);
        //std::cout << r1 << std::endl;
        uint32_t r2 = regMain->readRS2(rs2);
        //std::cout << r2 << std::endl;
        uint32_t result = r1 >> r2;
        regMain->writeRD(rd, result);
    }

    //Set Less Than Unsigned

    if (fmt == 1 && func3 == 0x3 && func7 == 0){
        std::cout << "ENTERED" << std::endl;
        uint32_t r1 = regMain->readRS1(rs1);
        //std::cout << r1 << std::endl;
        uint32_t r2 = regMain->readRS2(rs2);
        //std::cout << r2 << std::endl;
        uint32_t result = (r1 < r2)?1:0;
        regMain->writeRD(rd, result);
    }

    //Immediate type instructions

    //ADD IMMEDIATE

    if (fmt == 2 && func3 == 0 && opcode == 0b0010011){
        std::cout << "ENTERED" << std::endl;
        uint32_t r1 = regMain->readRS1(rs1);
        //std::cout << r1 << std::endl;
        //std::cout << imm << std::endl;
        uint32_t result = r1+imm;
        regMain->writeRD(rd, result);
    }

    //XOR IMMEDIATE

    else if (fmt == 2 && func3 == 0x4 && opcode == 0b0010011){
        std::cout << "ENTERED" << std::endl;
        uint32_t r1 = regMain->readRS1(rs1);
        //std::cout << r1 << std::endl;
        //std::cout << imm << std::endl;
        uint32_t result = r1^imm;
        regMain->writeRD(rd, result);
    }

    //OR IMMEDIATE
    else if (fmt == 2 && func3 == 0x6 && opcode == 0b0010011){
        std::cout << "ENTERED" << std::endl;
        uint32_t r1 = regMain->readRS1(rs1);
        //std::cout << r1 << std::endl;
        //std::cout << imm << std::endl;
        uint32_t result = r1|imm;
        regMain->writeRD(rd, result);
    }

    //AND IMMEDIATE
    else if (fmt == 2 && func3 == 0x7 && opcode == 0b0010011){
        std::cout << "ENTERED" << std::endl;
        uint32_t r1 = regMain->readRS1(rs1);
        //std::cout << r1 << std::endl;
        //std::cout << imm << std::endl;
        uint32_t result = r1 & imm;
        regMain->writeRD(rd, result);
    }

    //Shift Left Logical Imm
    else if (fmt == 2 && func3 == 0x1 && opcode == 0b0010011){
        std::cout << "ENTERED" << std::endl;
        uint32_t imm0to4 = imm << 27;
        imm0to4 >>= 27;
        uint32_t r1 = regMain->readRS1(rs1);
        //std::cout << r1 << std::endl;
        //std::cout << imm << std::endl;
        uint32_t result = r1 << imm0to4;
        regMain->writeRD(rd, result);
    }

    //Shift Right Logical Imm
    else if (fmt == 2 && func3 == 0x5) {
        // Bit 30 of the instruction (Bit 10 of the I-type immediate) 
        // determines if it is LOGICAL (0) or ARITHMETIC (1)
        bool isArithmetic = (imm >> 10) & 0x1; 

        if (!isArithmetic) {
            std::cout << "ENTERED SRLI" << std::endl;
            uint32_t r1 = regMain->readRS1(rs1);
            uint32_t shamt = imm & 0x1F; // Use only the bottom 5 bits
            
            uint32_t result = r1 >> shamt; 
            regMain->writeRD(rd, result);
        }
    }

    //Set Less Than Unsigned Imm

    else if (fmt == 2 && func3 == 0x3 && opcode == 0b0010011){
        std::cout << "ENTERED" << std::endl;
        uint32_t imm0to4 = imm << 27;
        imm0to4 >>= 27;
        uint32_t r1 = regMain->readRS1(rs1);
        //std::cout << r1 << std::endl;
        //std::cout << imm << std::endl;
        uint32_t result = (r1 < imm0to4)?1:0
;
        regMain->writeRD(rd, result);
    }


/*                          JUMP AND LINK                                 */

    //jal
    if (fmt == 6 && opcode == 0b1101111){
        cout << "pre " << pc << endl;
        cout << "ENTERED jump and link" << endl;
        regMain->writeRD(rd, pc+4);
        //Casting to signed, to ensure that if the immediate is a -ve
        //the calculation will still work
        pc += (int32_t)imm - 4;
        cout << "post " << pc << endl;
    }
    
    //jalr

    if (fmt == 2 && opcode == 0b1100111 && func3 == 0){
        cout << "pre " << pc << endl;
        cout << "ENTERED jump and link" << endl;
        regMain->writeRD(rd, pc+4);

        // 1. Treat imm as signed for the math
        // 2. Add to register
        // 3. Mask the last bit (& ~1) to ensure it's even

        uint32_t target = (regMain->readRS1(rs1) + (int32_t)imm) & ~1;
        pc = target - 4;       
        cout << "post " << pc << endl;
    }

/*                          BRANCHING FUNCTIONS                                 */

    //Branch ==

    if (fmt == 4 && func3 == 0x0){
        cout << "ENTERED EQUAL" << endl;
        if ((int32_t)regMain->readRS1(rs1) == (int32_t)regMain->readRS2(rs2)){
            pc += (int32_t)imm - 4;
        }
    }

    //Brach !=

    else if (fmt == 4 && func3 == 0x1){
        cout << "ENTERED NOT EQUAL" << endl;
        if ((int32_t)regMain->readRS1(rs1) != (int32_t)regMain->readRS2(rs2)){
            cout << (int32_t)imm << endl;
            pc += (int32_t)imm - 4;
        }
    }

    //Branch <
    else if (fmt == 4 && func3 == 0x4){
        if ((int32_t)regMain->readRS1(rs1) < (int32_t)regMain->readRS2(rs2)){
            pc += (int32_t)imm - 4;
        }
    }

    //Branch >=
    else if (fmt == 4 && func3 == 0x5){
        if ((int32_t)regMain->readRS1(rs1) >= (int32_t)regMain->readRS2(rs2)){
            pc += (int32_t)imm - 4;
        }
    }

    //Branch unsigned <
    else if (fmt == 4 && func3 == 0x6){
        if (regMain->readRS1(rs1) < regMain->readRS2(rs2)){
            pc += (int32_t)imm - 4;
        }
    }

    //Branch unsigned >=
    else if (fmt == 4 && func3 == 0x7){
        if (regMain->readRS1(rs1) >= regMain->readRS2(rs2)){
            pc += (int32_t)imm - 4;
        }
    }


/*                          LOAD/STORE INSTRUCTIONS                                 */

    //Load Word
    if (fmt == 2 && func3 == 0x2 && opcode == 0b0000011){
        std::cout << "ENTERED load word" << std::endl;
        uint32_t r1 = regMain->readRS1(rs1);
        uint32_t address = r1+ imm;
        regMain->writeRD(rd, cpuBus->loadWord(address));
    }

    //Store Word
    else if (fmt == 3 && func3 == 0x2 && opcode == 0b0100011){
        std::cout << "ENTERED store word" << std::endl;
        uint32_t targetAddress = regMain->readRS1(rs1) + imm;
        uint32_t val = regMain->readRS2(rs2);
        cpuBus->storeWord(targetAddress, val);
        cout << targetAddress << endl;
    }

    //Load Half-Word
    else if (fmt == 2 && func3 == 0x1 && opcode == 0b0000011){
        std::cout << "ENTERED load half word" << std::endl;
        uint32_t r1 = regMain->readRS1(rs1);
        uint32_t address = r1+ imm;
        regMain->writeRD(rd, (int32_t)(int16_t)(cpuBus->loadHalfWord(address)));

    }

    //Load Unsigned Half-Word
    else if (fmt == 2 && func3 == 0x5 && opcode == 0b0000011){
        std::cout << "ENTERED load half word" << std::endl;
        uint32_t r1 = regMain->readRS1(rs1);
        uint32_t address = r1+ imm;
        regMain->writeRD(rd, cpuBus->loadHalfWord(address));
    }

    //Store Half-Word
    else if (fmt == 3 && func3 == 0x1 && opcode == 0b0100011){
        std::cout << "ENTERED store half word" << std::endl;
        uint32_t targetAddress = regMain->readRS1(rs1) + imm;
        uint32_t val = regMain->readRS2(rs2);
        cpuBus->storeHalfWord(targetAddress, val);
        cout << targetAddress << endl;
    }

    //Load Byte
    else if (fmt == 2 && func3 == 0x0 && opcode == 0b0000011){
        std::cout << "ENTERED load word" << std::endl;
        uint32_t r1 = regMain->readRS1(rs1);
        uint32_t address = r1+ imm;
        regMain->writeRD(rd, (int32_t)(int8_t)(cpuBus->loadByte(address)));
    }

    //Load Unsigned Byte
    else if (fmt == 2 && func3 == 0x4 && opcode == 0b0000011){
        std::cout << "ENTERED load word" << std::endl;
        uint32_t r1 = regMain->readRS1(rs1);
        uint32_t address = r1+ imm;
        regMain->writeRD(rd, cpuBus->loadByte(address));
    }

    //Store Byte
    else if (fmt == 3 && func3 == 0x0 && opcode == 0b0100011){
        std::cout << "ENTERED store word" << std::endl;
        uint32_t targetAddress = regMain->readRS1(rs1) + imm;
        uint32_t val = regMain->readRS2(rs2);
        cpuBus->storeByte(targetAddress, val);
        cout << targetAddress << endl;
    }
}

void cpu::tick(){
    //std::cout << cpuBus->loadWord(pc) << std::endl;
    cpu::process(cpuBus->loadWord(pc));
    cout << "PC cpu at address: " << pc << endl;
    pc += 4;
    cout << "PC cpu at address: " << pc << endl;
}