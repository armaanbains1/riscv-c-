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
// 1. x1 = -2 (0xFFFFFFFE)
systemBus.storeWord(0, 0x00a00513);      // addi x10, x0, 10    (Base value: 10)
systemBus.storeWord(4, 0x00200593);      // addi x11, x0, 2     (Shift/Compare value: 2)

// 1. SLLI (Immediate): x12 = x10 << 2
systemBus.storeWord(8, 0x00251613);      // slli x12, x10, 2    -> Expect 40 (0x28)

// 2. SRLI (Immediate): x13 = x10 >> 2
systemBus.storeWord(12, 0x00255693);     // srli x13, x10, 2    -> Expect 2

// 3. SLTIU (Immediate): x14 = (x10 < 2)
systemBus.storeWord(16, 0x00253713);     // sltiu x14, x10, 2   -> Expect 0

// 4. SLL (Register): x15 = x10 << x11
systemBus.storeWord(20, 0x00b517b3);     // sll x15, x10, x11   -> Expect 40 (0x28)

// 5. SRL (Register): x16 = x10 >> x11
systemBus.storeWord(24, 0x00b55833);     // srl x16, x10, x11   -> Expect 2

// 6. SLTU (Register): x17 = (x11 < x10)
systemBus.storeWord(28, 0x00a5b8b3);     // sltu x17, x11, x10  -> Expect 1

// Run 8 ticks to clear setup and tests
for(int i=0; i<8; i++) systemCpu.tick();

std::cout << "--- LOGICAL GAUNTLET RESULTS ---" << std::endl;
std::cout << "X12 (SLLI): " << std::hex; systemRegister.dumpReg(12);
std::cout << "X13 (SRLI): "; systemRegister.dumpReg(13);
std::cout << "X14 (SLTIU):"; systemRegister.dumpReg(14);
std::cout << "X15 (SLL):  "; systemRegister.dumpReg(15);
std::cout << "X16 (SRL):  "; systemRegister.dumpReg(16);
std::cout << "X17 (SLTU): "; systemRegister.dumpReg(17);
    systemBus.dumpMemory();

    // 8. Verify the result
    std::cout << "RESULT X11: ";
    systemRegister.dumpReg(11); 
    

    return 0;
}

