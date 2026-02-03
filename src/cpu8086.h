#ifndef CPU8086_H
#define CPU8086_H

#include <cstdint>
#include <vector>
#include <string>
#include <atomic>
#include <mutex>
#include <map>
#include <functional>

union Register16 {
    uint16_t x;
    struct { uint8_t l; uint8_t h; } byte;
};

struct CPUState {
    Register16 ax, bx, cx, dx;
    uint16_t cs, ds, es, ss;
    uint16_t sp, bp, si, di;
    uint16_t ip;
    uint16_t flags; // CF=0, ZF=6, SF=7, OF=11
    uint64_t cycles;
};

struct Snapshot {
    CPUState state;
    uint32_t lastModifiedAddr;
    std::vector<uint8_t> memory;
};

class CPU8086 {
public:
    CPU8086();

    // --- Lifecycle (cpu8086.cpp) ---
    void reset();
    void compile(const std::string& sourceCode);
    CPUState getState() const;
    uint8_t getMemoryAt(uint32_t addr) const;
    int getLineForAddress(uint32_t addr) const;
    bool isHalted() const; // NEW

    // --- Execution (cpu_exec.cpp) ---
    void stepForward();
    void stepBackward();
    bool canStepBack();

    // --- I/O Ports ---
    std::function<void(char)> outputCallback;
    std::function<void(uint8_t)> port199Callback;

private:
    std::vector<uint8_t> memory;
    CPUState state;
    std::vector<Snapshot> historyBuffer;
    std::map<uint32_t, int> debugLineMap;
    mutable std::mutex stateMutex;

    bool halted; // NEW

    // --- Helpers ---
    void saveSnapshot(uint32_t modifiedAddr = 0xFFFFFFFF);
    uint32_t getPhysicalAddress(uint16_t segment, uint16_t offset);
    int getRegIndex(std::string regName);

    // --- Operations (cpu_ops.cpp) ---
    void push(uint16_t val);
    uint16_t pop();
    void opMOV(uint8_t opcode, uint32_t pc);
    void opALU(uint8_t opcode, uint32_t pc);
    void opMath(uint8_t opcode, uint32_t pc);
    void opStack(uint8_t opcode, uint32_t pc);
    void opJMP(uint8_t opcode, uint32_t pc);
    void setFlags(uint32_t result, int bits);
};

#endif // CPU8086_H
