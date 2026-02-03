#include "emulator.h"
#include <cstring>
#include <sstream>
#include <iostream>
#include <algorithm>

Emulator::Emulator() { reset(); }

void Emulator::reset() {
    std::memset(memory, 0, sizeof(memory));
    state.ax.x = 0; state.bx.x = 0; state.cx.x = 0; state.dx.x = 0;
    state.cs = 0; state.ds = 0; state.es = 0; state.ss = 0;
    state.ip = 0; state.flags = 0;
    state.sp = 0xFFFE; // Stack pointer usually starts at top
}

uint32_t Emulator::getPhysicalAddress(uint16_t segment, uint16_t offset) {
    // The core 20-bit logic: Segment * 16 + Offset
    return ((uint32_t)segment << 4) + offset;
}

// Helper to map strings to our internal logic
// 0=AX, 1=CX, 2=DX, 3=BX (Standard 8086 machine code order)
int Emulator::getRegIndex(std::string regName) {
    if (regName == "AX") return 0;
    if (regName == "CX") return 1;
    if (regName == "DX") return 2;
    if (regName == "BX") return 3;
    return -1;
}

void Emulator::compile(const std::string& sourceCode) {
    reset();
    std::stringstream ss(sourceCode);
    std::string line;
    uint32_t writeAddr = 0;

    // Note: We are writing to CS:0000 for simplicity now
    while (std::getline(ss, line)) {
        if(line.empty()) continue;
        std::transform(line.begin(), line.end(), line.begin(), ::toupper);
        std::stringstream ls(line);
        std::string opcode, arg1, arg2;
        ls >> opcode;

        if (opcode == "MOV") {
            ls >> arg1 >> arg2;
            arg1.erase(std::remove(arg1.begin(), arg1.end(), ','), arg1.end());
            int reg = getRegIndex(arg1);
            int val = std::stoi(arg2, nullptr, 16);

            // B8 + reg = MOV reg, imm16
            memory[writeAddr++] = 0xB8 + reg;
            memory[writeAddr++] = val & 0xFF;        // Low byte
            memory[writeAddr++] = (val >> 8) & 0xFF; // High byte
        }
        else if (opcode == "HLT") {
            memory[writeAddr++] = 0xF4;
        }
    }
}

void Emulator::step() {
    // 20-bit Fetch
    uint32_t pc = getPhysicalAddress(state.cs, state.ip);
    uint8_t opcode = memory[pc];

    switch(opcode) {
        case 0xB8: state.ax.x = memory[pc+1] | (memory[pc+2] << 8); state.ip += 3; break;
        case 0xB9: state.cx.x = memory[pc+1] | (memory[pc+2] << 8); state.ip += 3; break;
        case 0xBA: state.dx.x = memory[pc+1] | (memory[pc+2] << 8); state.ip += 3; break;
        case 0xBB: state.bx.x = memory[pc+1] | (memory[pc+2] << 8); state.ip += 3; break;
        case 0xF4: std::cout << "HALT" << std::endl; break;
        default: state.ip++; break;
    }
}
