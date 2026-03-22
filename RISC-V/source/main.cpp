#include <vector>
#include <cstdint>
#include <iostream>

#include "registers.h"
#include "cpu.h"
#include "bus.h"
using namespace std;




int main(){

    //Initialize sp to the second register

    

    std::vector<std::uint8_t> onchipRam(1024 * 1024, 0); 

    bus systemBus(&onchipRam);
    registers systemRegister;

    //Initialize sp to the second register

    systemRegister.writeRD(2, 1048572);
    //std::cout << "hello" << std::endl;
    int pc = 0;
    cpu systemCpu(pc, &systemBus, &systemRegister);

    //std::cout << "hello" << std::endl;

    // 5. Pre-load the registers with test data
    //systemRegister.writeRD(1, 20); // x1 = 20


    //systemRegister.writeRD(2, 30); // x2 = 30

    // 1. x10 = 3 (The number we want the factorial of)
    systemBus.storeWord(0, 0x00300513);      // addi x10, x0, 3

    // 2. x11 = 1 (Our multiplier/result tracker)
    systemBus.storeWord(4, 0x00100593);      // addi x11, x0, 1

    // --- LOOP START (Address 8) ---

    // 3. x11 = x11 * x10 (Simulated multiplication via repeated addition or just x11 = x11 + x10 for simplicity)
    // Let's just do a result check: x11 = x11 + x10
    systemBus.storeWord(8, 0x00a585b3);      // add x11, x11, x10

    // 4. x10 = x10 - 1
    systemBus.storeWord(12, 0xfff50513);     // addi x10, x10, -1

    // 5. IF x10 != 0, JUMP BACK TO LOOP START (Address 8)
    // PC is 16. We want to go to 8. Imm = -8.
    systemBus.storeWord(16, 0xfe051ce3);     // bne x10, x0, -8

    // --- LOOP END ---

    // 6. Final Result: Load the total into x10 to show off
    systemBus.storeWord(20, 0x00058513);     // addi x10, x11, 0


    systemCpu.tick(); 
    std::cout << "RESULT X10: ";
    systemRegister.dumpReg(10); 

    systemCpu.tick(); 
    std::cout << "RESULT X10: ";
    systemRegister.dumpReg(10); 
    systemCpu.tick(); 
    std::cout << "RESULT X10: ";
    systemRegister.dumpReg(10); 
    systemCpu.tick(); 
    std::cout << "RESULT X10: ";
    systemRegister.dumpReg(10); 
    systemCpu.tick(); 
    std::cout << "RESULT X10: ";
    systemRegister.dumpReg(10); 
    systemCpu.tick(); 
    std::cout << "RESULT X10: ";
    systemRegister.dumpReg(10); 
    systemCpu.tick(); 
    std::cout << "RESULT X10: ";
    systemRegister.dumpReg(10); 
    systemCpu.tick(); 
    std::cout << "RESULT X10: ";
    systemRegister.dumpReg(10); 
    systemCpu.tick(); 
    std::cout << "RESULT X10: ";
    systemRegister.dumpReg(10); 
    systemCpu.tick(); 
    std::cout << "RESULT X10: ";
    systemRegister.dumpReg(10); 

    systemBus.dumpMemory();

    // 8. Verify the result
    std::cout << "RESULT X11: ";
    systemRegister.dumpReg(11); 
    

    return 0;
}

