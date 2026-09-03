#include "bus.h"
using namespace std;

//Word Loading/Storing

bus::bus(std::vector<std::uint8_t> * memory){
    memoryMain = memory;
}

//ai slop, fix, only for debugging
void bus::dumpMemory() {
    std::cout << "\n--- EFFICIENT MEMORY DUMP (HEX) ---" << std::endl;
    std::cout << "  Address    00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F" << std::endl;
    std::cout << "  ---------  -----------------------------------------------" << std::endl;

    bool skipping = false;

    // Iterate in chunks of 16 bytes
    for (uint32_t i = 0; i < (*memoryMain).size(); i += 16) {
        // Check if the current 16-byte block is all zeros
        bool allZero = true;
        for (uint32_t j = 0; j < 16 && (i + j) < (*memoryMain).size(); j++) {
            if ((*memoryMain)[i + j] != 0) {
                allZero = false;
                break;
            }
        }

        // Only print if there is data, OR if it's the very first/last block
        if (!allZero || i == 0 || i >= ((*memoryMain).size() - 16)) {
            if (skipping) {
                std::cout << "  [ ... empty memory ... ]" << std::endl;
                skipping = false;
            }

            std::printf("  0x%08X ", i);
            for (uint32_t j = 0; j < 16 && (i + j) < (*memoryMain).size(); j++) {
                std::printf("%02X ", (*memoryMain)[i + j]);
            }
            std::cout << std::endl;
        } else {
            skipping = true;
        }
    }
    std::cout << "-----------------------------------------------------------\n" << std::endl;
}

uint32_t bus::loadWord(int address){

    if (address % 4 != 0){
        cout << "Address misallignment, must be word-addressable";
        return -1;
    }

    uint32_t word32bit = 0;
    uint8_t temp = 0;
    for (int i = 0; i<= 3; i++){
        temp = (*memoryMain)[address+3-i];
        word32bit <<= 0x8;
        word32bit = word32bit | temp;
    }
    return word32bit;

}

void bus::storeWord(int address, uint32_t value){

    //First check to see if the address is word alligned
    //Risc-V (Word == 32 bits (4 bytes))
    //
    if (address % 4 != 0){
        cout << "Address misallignment, must be word-addressable";
        return;
    }
    
    for (int i = 0; i<=3; i++){
        (*memoryMain)[address + i] = value & 0xFF;
        value >>= 8;

    }

}



// Byte Loading/Storing



uint8_t bus::loadByte(int address){

    uint8_t word8bit = (*memoryMain)[address];
    return word8bit;

}

void bus::storeByte(int address, uint8_t value){
    
    (*memoryMain)[address] = value;

}


// Half-Word Loading/Storing


uint16_t bus::loadHalfWord(int address){

    if (address % 2 != 0){
        cout << "Address misallignment, must be word-addressable";
        return -1;
    }

    uint16_t word16bit = 0;
    uint8_t temp = 0;
    for (int i = 0; i<= 1; i++){
        temp = (*memoryMain)[address+1-i];
        word16bit <<= 0x8;
        word16bit = word16bit | temp;
    }
    return word16bit;

}

void bus::storeHalfWord(int address, uint32_t value){

    //First check to see if the address is half-word alligned
    //Risc-V (Word == 16 bits (2 bytes))
    //
    if (address % 2 != 0){
        cout << "Address misallignment, must be word-addressable";
        return;
    }
    
    for (int i = 0; i<=1; i++){
        (*memoryMain)[address + i] = value & 0xFF;
        value >>= 8;

    }

}


