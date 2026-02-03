#ifndef EMULATOR_H
#define EMULATOR_H

#include "cpu_state.h"
#include <vector>
#include <string>

class Emulator {
public:
    Emulator();
    void reset();
    void compile(const std::string& sourceCode);
    void step();

    // 20-Bit Address Calculation: (Segment << 4) + Offset
    uint32_t getPhysicalAddress(uint16_t segment, uint16_t offset);

    CPUState getState() const { return state; }
    uint8_t getMemoryAt(uint32_t addr) const {
        if(addr < 1024*1024) return memory[addr];
        return 0;
    }

private:
    uint8_t memory[1024 * 1024]; // 1MB RAM
    CPUState state;

    int getRegIndex(std::string regName);
};

#endif // EMULATOR_H
