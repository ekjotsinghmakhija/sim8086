#include "cpu8086.h"

void CPU8086::stepForward() {
    std::lock_guard<std::mutex> lock(stateMutex);
    saveSnapshot();

    uint32_t pc = getPhysicalAddress(state.cs, state.ip);
    if (pc >= memory.size()) pc = 0;
    uint8_t opcode = memory[pc];

    if ((opcode & 0xF0) == 0xB0) opMOV(opcode, pc);
    else if (opcode == 0x05 || opcode == 0x2D || opcode == 0x3C ||
             (opcode >= 0x20 && opcode <= 0x25) ||
             (opcode >= 0x08 && opcode <= 0x0D)) opALU(opcode, pc);

    // FIX: Expanded range to catch ALL PUSH/POP instructions (0x50 - 0x5F)
    else if ((opcode >= 0x50 && opcode <= 0x5F) ||
             opcode == 0xE8 || opcode == 0xC3 ||
             opcode == 0xEB || opcode == 0x74 || opcode == 0x75) opStack(opcode, pc);

    else if (opcode == 0xF7) opMath(opcode, pc);
    else if (opcode == 0xE6) {
        uint8_t port = memory[pc + 1];
        state.ip += 2;
        if (port == 199 && port199Callback) port199Callback(state.ax.byte.l);
    }
    else if (opcode == 0xCD) {
        if (memory[pc+1] == 0x21 && outputCallback) {
            if (state.ax.byte.h == 0x02) outputCallback((char)state.dx.byte.l);
            else if (state.ax.byte.h == 0x09) {
                // Print String ($ terminated)
                uint32_t addr = (state.ds << 4) + state.dx.x;
                int safetyLimit = 0;
                while(memory[addr] != '$' && safetyLimit < 2000) {
                    outputCallback((char)memory[addr++]);
                    safetyLimit++;
                }
            }
        }
        state.ip += 2;
    }
    else if (opcode == 0x40) { state.ax.x++; state.ip++; }
    else if (opcode == 0x48) { state.ax.x--; state.ip++; }
    else if (opcode == 0xF4) { halted = true; }
    else { state.ip++; }

    state.cycles++;
}

void CPU8086::stepBackward() {
    std::lock_guard<std::mutex> lock(stateMutex);
    if (historyBuffer.empty()) return;
    Snapshot last = historyBuffer.back();
    historyBuffer.pop_back();
    state = last.state;
    memory = last.memory;
    halted = false;
}

bool CPU8086::canStepBack() {
    std::lock_guard<std::mutex> lock(stateMutex);
    return !historyBuffer.empty();
}
