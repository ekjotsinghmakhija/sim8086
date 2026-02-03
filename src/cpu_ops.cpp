#include "cpu8086.h"

// --- Stack Helpers ---
void CPU8086::push(uint16_t val) {
    state.sp -= 2;
    uint32_t addr = getPhysicalAddress(state.ss, state.sp);
    memory[addr] = val & 0xFF;
    memory[addr+1] = (val >> 8) & 0xFF;
}

uint16_t CPU8086::pop() {
    uint32_t addr = getPhysicalAddress(state.ss, state.sp);
    uint16_t val = memory[addr] | (memory[addr+1] << 8);
    state.sp += 2;
    return val;
}

void CPU8086::setFlags(uint32_t result, int bits) {
    if (result == 0) state.flags |= (1 << 6); else state.flags &= ~(1 << 6); // ZF
    int mask = (bits == 16) ? 0x8000 : 0x80;
    if (result & mask) state.flags |= (1 << 7); else state.flags &= ~(1 << 7); // SF
}

// --- Logic Implementation ---

void CPU8086::opMOV(uint8_t opcode, uint32_t pc) {
    if (opcode >= 0xB8 && opcode <= 0xBB) {
        uint16_t val = memory[pc+1] | (memory[pc+2] << 8);
        if (opcode == 0xB8) state.ax.x = val;
        if (opcode == 0xB9) state.cx.x = val;
        if (opcode == 0xBA) state.dx.x = val;
        if (opcode == 0xBB) state.bx.x = val;
        state.ip += 3;
    }
    else if (opcode == 0xB4) {
        state.ax.byte.h = memory[pc+1];
        state.ip += 2;
    }
}

void CPU8086::opALU(uint8_t opcode, uint32_t pc) {
    uint16_t val16 = memory[pc+1] | (memory[pc+2] << 8);
    uint8_t val8 = memory[pc+1];
    uint32_t res = 0;

    if (opcode == 0x05) { // ADD AX
        res = state.ax.x + val16;
        state.ax.x = (uint16_t)res;
        setFlags(state.ax.x, 16);
        state.ip += 3;
    }
    else if (opcode == 0x2D) { // SUB AX
        res = state.ax.x - val16;
        state.ax.x = (uint16_t)res;
        setFlags(state.ax.x, 16);
        state.ip += 3;
    }
    else if (opcode == 0x3C) { // CMP AL
        res = state.ax.byte.l - val8;
        setFlags(res & 0xFF, 8);
        state.ip += 2;
    }
}

void CPU8086::opStack(uint8_t opcode, uint32_t pc) {
    // FIX: Handle PUSH for AX, CX, DX, BX
    if (opcode >= 0x50 && opcode <= 0x53) {
        uint16_t val = 0;
        if (opcode == 0x50) val = state.ax.x;
        else if (opcode == 0x51) val = state.cx.x;
        else if (opcode == 0x52) val = state.dx.x;
        else if (opcode == 0x53) val = state.bx.x;
        push(val);
        state.ip++;
    }
    // FIX: Handle POP for AX, CX, DX, BX
    else if (opcode >= 0x58 && opcode <= 0x5B) {
        uint16_t val = pop();
        if (opcode == 0x58) state.ax.x = val;
        else if (opcode == 0x59) state.cx.x = val;
        else if (opcode == 0x5A) state.dx.x = val;
        else if (opcode == 0x5B) state.bx.x = val;
        state.ip++;
    }
    // Flow Control
    else if (opcode == 0xE8) { // CALL
        uint16_t rel = memory[pc+1] | (memory[pc+2] << 8);
        push(state.ip + 3);
        state.ip += 3 + rel;
    }
    else if (opcode == 0xC3) { state.ip = pop(); } // RET
    else if (opcode == 0xEB) { state.ip += 2 + (int8_t)memory[pc+1]; } // JMP Short
    else if (opcode == 0x75) { // JNZ
        if (!(state.flags & (1 << 6))) state.ip += 2 + (int8_t)memory[pc+1];
        else state.ip += 2;
    }
}

void CPU8086::opMath(uint8_t opcode, uint32_t pc) {
    uint8_t modrm = memory[pc+1];
    int regIdx = modrm & 0x07;
    int opType = (modrm >> 3) & 0x07;

    uint16_t val = 0;
    if (regIdx == 0) val = state.ax.x;
    else if (regIdx == 1) val = state.cx.x;
    else if (regIdx == 2) val = state.dx.x;
    else if (regIdx == 3) val = state.bx.x;

    if (opType == 4) { // MUL
        uint32_t res = (uint32_t)state.ax.x * val;
        state.ax.x = res & 0xFFFF;
        state.dx.x = (res >> 16) & 0xFFFF;
        if (state.dx.x != 0) state.flags |= (1 << 11); else state.flags &= ~(1 << 11);
        state.ip += 2;
    }
    else if (opType == 6) { // DIV
        if (val == 0) return;
        uint32_t num = (uint32_t(state.dx.x) << 16) | state.ax.x;
        state.ax.x = (uint16_t)(num / val);
        state.dx.x = (uint16_t)(num % val);
        state.ip += 2;
    }
}
