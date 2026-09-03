#include "registers.h"
using namespace std;
#include <sstream>
#include <iomanip>

registers::registers(){
    //Empty Initialization (Registers live within this class, so we don't need a formal intitializiation)
    registers32.assign(32, 0);
    registers32[0] = 0;
}

//Used to access Register Source 1
uint32_t registers::readRS1(int regNumber){
    if (regNumber == 0){
        cout << "Unable to read x0" << endl;
        return registers32[regNumber];
    }

    return registers32[regNumber];
}

//Used to access Register Source 2
uint32_t registers::readRS2(int regNumber){
    if (regNumber == 0){
        cout << "Unable to read x0" << endl;
        return registers32[regNumber]; 
    }

    return registers32[regNumber];
}


//Used to write to a destination register with a 32 bit value
void registers::writeRD(int destinationRegister, uint32_t value){
    if (destinationRegister == 0){
        cout << "Unable to read x0" << endl;
        return;
    }
    
    registers32[destinationRegister] = value;
}

//Reset the registers to zero
void registers::regReset(){
    for (auto& val: registers32){
        val = 0;
    }
}

//Debugging use (prints values of all registers)
void registers::dumpAll(){
    int i = 0;
    for (auto& val: registers32){
        
        cout << i << "          0x" << std::hex<< (val) << endl;
        i++;
    }
}

void registers::clearAll(){
    for (auto& val: registers32){
    
        val = 0;
    }   
}

//Debugging use (prints values of one register)
void registers::dumpReg(int reg){
    cout << "printing" << endl;
    cout << "0x" << std::hex << (registers32[reg]) << endl;;
}

std::string registers::dumpRegUI(int reg){
    std::stringstream ss;
    ss << "0x" << std::setfill('0') << std::setw(8) << std::hex << registers32[reg];
    return ss.str();
}


