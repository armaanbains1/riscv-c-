#include "bus.h"
using namespace std;

//Word Loading/Storing

uint32_t bus::loadWord(int address){

    if (address % 4 != 0){
        cout << "Address misallignment, must be word-addressable"
        return -1;
    }

    uint32_t word32bit = 0;
    uint8_t temp = 0;
    for (int i = 0; i<= 3; i++){
        temp = (*memory)[address+3-i];
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
        cout << "Address misallignment, must be word-addressable"
        return;
    }
    
    for (int i = 0; i<=3; i++){
        (*memory)[address + i] = value & 0xFF;
        value >>= 8;

    }

}



// Byte Loading/Storing



uint8_t bus::loadByte(int address){

    uint8_t word8bit = (*memory)[address];
    return word8bit;

}

void bus::storeByte(int address, uint8_t value){
    
    (*memory)[address] = value;

}


// Half-Word Loading/Storing


uint16_t bus::loadHalfWord(int address){

    if (address % 2 != 0){
        cout << "Address misallignment, must be word-addressable"
        return -1;
    }

    uint16_t word16bit = 0;
    uint8_t temp = 0;
    for (int i = 0; i<= 1; i++){
        temp = (*memory)[address+1-i];
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
        cout << "Address misallignment, must be word-addressable"
        return;
    }
    
    for (int i = 0; i<=1; i++){
        (*memory)[address + i] = value & 0xFF;
        value >>= 8;

    }

}


