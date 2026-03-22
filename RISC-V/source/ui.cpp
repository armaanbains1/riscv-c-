#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <iomanip>
#include <sstream>
#include <vector>
#include <string>

#include "cpu.h"
#include "bus.h"
#include "registers.h"

using namespace ftxui;
using namespace std;

int localPC = 0;

void step(cpu& systemCpu){
    systemCpu.tick();
}

void run(){
    std::cout << "Run" << std::endl;
}

int getVal(string s) {
    if (s.empty()) return 0;
    // Remove 'x', '(', ')', and ','
    string clean = "";
    for(char c : s) if(isdigit(c) || c == '-') clean += c;
    return clean.empty() ? 0 : stoi(clean);
}

uint32_t assemble(string instruction) {
    stringstream ss(instruction);
    string name, arg1, arg2, arg3;
    ss >> name >> arg1 >> arg2 >> arg3;

    uint32_t opcode = 0, f3 = 0, f7 = 0, rd = 0, rs1 = 0, rs2 = 0, imm = 0;

    // --- R-TYPE GROUP --- (rd, rs1, rs2)
    if (name == "add" || name == "sub" || name == "sll" || name == "slt" || 
        name == "sltu"|| name == "xor" || name == "srl" || name == "sra" || 
        name == "or"  || name == "and") {
        
        opcode = 0x33;
        rd = getVal(arg1); rs1 = getVal(arg2); rs2 = getVal(arg3);
        
        if (name == "add")  { f3 = 0x0; f7 = 0x00; }
        else if (name == "sub")  { f3 = 0x0; f7 = 0x20; }
        else if (name == "sll")  { f3 = 0x1; f7 = 0x00; }
        else if (name == "slt")  { f3 = 0x2; f7 = 0x00; }
        else if (name == "sltu") { f3 = 0x3; f7 = 0x00; }
        else if (name == "xor")  { f3 = 0x4; f7 = 0x00; }
        else if (name == "srl")  { f3 = 0x5; f7 = 0x00; }
        else if (name == "sra")  { f3 = 0x5; f7 = 0x20; }
        else if (name == "or")   { f3 = 0x6; f7 = 0x00; }
        else if (name == "and")  { f3 = 0x7; f7 = 0x00; }

        return (f7 << 25) | (rs2 << 20) | (rs1 << 15) | (f3 << 12) | (rd << 7) | opcode;
    }

    // --- I-TYPE GROUP --- (rd, rs1, imm) - addi, andi, ori, xori, etc.
    else if (name == "addi" || name == "andi" || name == "ori" || name == "xori" || 
             name == "slti" || name == "sltiu" || name == "jalr") {
        
        opcode = (name == "jalr") ? 0x67 : 0x13;
        rd = getVal(arg1); rs1 = getVal(arg2); imm = getVal(arg3);

        if (name == "addi") f3 = 0x0;
        else if (name == "slti") f3 = 0x2;
        else if (name == "sltiu") f3 = 0x3;
        else if (name == "xori") f3 = 0x4;
        else if (name == "ori") f3 = 0x6;
        else if (name == "andi") f3 = 0x7;

        return (imm << 20) | (rs1 << 15) | (f3 << 12) | (rd << 7) | opcode;
    }

    // --- LOAD GROUP --- (rd, imm(rs1)) - lw, lb, lhu, etc.
    else if (name == "lb" || name == "lh" || name == "lw" || name == "lbu" || name == "lhu") {
        opcode = 0x03;
        rd = getVal(arg1); imm = getVal(arg2); rs1 = getVal(arg3);
        
        if (name == "lb") f3 = 0x0;
        else if (name == "lh") f3 = 0x1;
        else if (name == "lw") f3 = 0x2;
        else if (name == "lbu") f3 = 0x4;
        else if (name == "lhu") f3 = 0x5;

        return (imm << 20) | (rs1 << 15) | (f3 << 12) | (rd << 7) | opcode;
    }

    // --- STORE GROUP --- (rs2, imm(rs1)) - sw, sb, sh
    else if (name == "sb" || name == "sh" || name == "sw") {
        opcode = 0x23;
        rs2 = getVal(arg1); imm = getVal(arg2); rs1 = getVal(arg3);
        
        if (name == "sb") f3 = 0x0;
        else if (name == "sh") f3 = 0x1;
        else if (name == "sw") f3 = 0x2;

        // S-Type Split: [imm 11:5] [rs2] [rs1] [f3] [imm 4:0] [opcode]
        return ((imm >> 5) << 25) | (rs2 << 20) | (rs1 << 15) | (f3 << 12) | ((imm & 0x1F) << 7) | opcode;
    }

    // --- B-TYPE GROUP --- (rs1, rs2, imm) - beq, bne, blt, etc.
    else if (name == "beq" || name == "bne" || name == "blt" || name == "bge") {
        opcode = 0x63;
        rs1 = getVal(arg1); rs2 = getVal(arg2); imm = getVal(arg3);
        
        if (name == "beq") f3 = 0x0;
        else if (name == "bne") f3 = 0x1;
        else if (name == "blt") f3 = 0x4;
        else if (name == "bge") f3 = 0x5;

        // B-Type is the messiest because the bits are scattered for hardware reasons
        uint32_t b12 = (imm >> 12) & 1;
        uint32_t b11 = (imm >> 11) & 1;
        uint32_t b10_5 = (imm >> 5) & 0x3F;
        uint32_t b4_1 = (imm >> 1) & 0xF;

        return (b12 << 31) | (b10_5 << 25) | (rs2 << 20) | (rs1 << 15) | (f3 << 12) | (b4_1 << 8) | (b11 << 7) | opcode;
    }

    // --- U-TYPE GROUP --- (rd, imm) - lui, auipc
    else if (name == "lui" || name == "auipc") {
        opcode = (name == "lui") ? 0x37 : 0x17;
        rd = getVal(arg1); imm = getVal(arg2);
        return (imm << 12) | (rd << 7) | opcode;
    }

    return 0; // Error / Unknown
}

int main(){
    std::vector<std::uint8_t> onchipRam(1024 * 1024, 0); 

    bus systemBus(&onchipRam);
    registers systemRegister;

    //Initialize sp to the second register

    systemRegister.writeRD(2, 1048572);
    //std::cout << "hello" << std::endl;
    int pc = 0;
    cpu systemCpu(pc, &systemBus, &systemRegister);
    
    std::vector<string> commandsLine = {};
    
    std::string command_string;
    Component command = Input(&command_string, "addi x10, x0, 5...");
    std::string labelStep = "Step";
    std::string labelRun = "Run";

    Component button_step = Button(&labelStep, [&] { step(systemCpu);}); 
    Component button_run = Button(&labelRun, run);	
    auto screen = ScreenInteractive::TerminalOutput();

    Component keyCheck = CatchEvent(command, [&](Event event) {
        if (event == Event::Character('`')) {
            screen.ExitLoopClosure()(); 
            return true;
        }
        if (event == Event::Return){
            commandsLine.push_back(command_string);
            systemBus.storeWord(localPC, assemble(command_string));
            localPC+=4;
            command_string = "";
            return true;
        }
        return false;
    });
    int selected = 0;

    Component menu = Menu({&commandsLine, &selected});


    auto components = Container::Horizontal({
        keyCheck,
        button_step,
        button_run,
        menu
    });


    auto main_renderer = Renderer(components, [&] {
    return hbox({
            // Left Column
            hbox({
                vbox({
                    text("Registers"),
                    text("x0"), text("x1"), text("x2"), text("x3"), text("x4"), 
                    text("x5"), text("x6"), text("x7"), text("x8"), text("x9"),
                    text("x10"), text("x11"), text("x12"), text("x13"), text("x14"), text("x15") 
                }) | border,

                vbox({
                    text("Values"),
                    text(systemRegister.dumpRegUI(0)),
                    text(systemRegister.dumpRegUI(1)),
                    text(systemRegister.dumpRegUI(2)),
                    text(systemRegister.dumpRegUI(3)),
                    text(systemRegister.dumpRegUI(4)),
                    text(systemRegister.dumpRegUI(5)),
                    text(systemRegister.dumpRegUI(6)),
                    text(systemRegister.dumpRegUI(7)),
                    text(systemRegister.dumpRegUI(8)),
                    text(systemRegister.dumpRegUI(9)),
                    text(systemRegister.dumpRegUI(10)),
                    text(systemRegister.dumpRegUI(11)),
                    text(systemRegister.dumpRegUI(12)),
                    text(systemRegister.dumpRegUI(13)),
                    text(systemRegister.dumpRegUI(14)),
                    text(systemRegister.dumpRegUI(15))
                }) | border,
            }),

        vbox({
            vbox({
                text("Commands") ,
                separator(),
                menu->Render() | vscroll_indicator | frame | flex,
            }) | border | flex,

            
            hbox({

                keyCheck->Render() | flex,
                button_step->Render(),
                button_run->Render()

            })
            }) | flex
        }) | border;
    });
    

    screen.Loop(main_renderer);

    return 0;
}