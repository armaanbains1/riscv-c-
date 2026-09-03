#ifndef CPU_H
#define CPU_H

#include <vector>
#include <cstdint>
#include <iostream>
#include "bus.h"
#include "registers.h"


class cpu{
    private:
        int pc;
        registers* regMain;
        bus* cpuBus;
    
    public:
        cpu(int CPUpc, bus* b, registers* reg);
        void setPC(int val);
        int getPC();
        void process(uint32_t instruction);
        void tick();

};




#endif CPU_H