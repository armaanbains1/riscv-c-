#ifndef BUS_H
#define BUS_H

#include <vector>
#include <cstdint>
#include <iostream>

class bus{

    private:
        
        std::vector<std::uint8_t> * memory;
    
    public:

        //Word Loading/Storing
        std::uint32_t loadWord(int address);

        void storeWord(int address, std::uint32_t value);

        // Byte Loading/Storing

        std::uint8_t loadByte(int address);

        void storeByte(int address, std::uint8_t value);

        // Half-Word Loading/Storing

        std::uint16_t loadHalfWord(int address);

        void storeHalfWord(int address, std::uint32_t value);


};

#endif