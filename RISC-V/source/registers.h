#ifndef REGISTERS_H
#define REGISTERS_H

#include <vector>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <iomanip>

class registers{

    private:

        // 32 Registers of 32 byte words, size never modified
        std::vector<uint32_t> registers32;

    public:
    
    registers();

    //Used to access Register Source 1
    uint32_t readRS1(int regNumber);

    //Used to access Register Source 2
    uint32_t readRS2(int regNumber);

    //Used to write to a destination register with a 32 bit value
    void writeRD(int destinationRegister, uint32_t value);

    //Reset the registers to zero
    void regReset();

    //Debugging use (prints values of all registers)
    void dumpAll();

    //Debugging use (prints values of one register)
    void dumpReg(int reg);

    //Used for UI register value access
    std::string registers::dumpRegUI(int reg);



};

#endif