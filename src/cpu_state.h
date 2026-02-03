#ifndef CPU_STATE_H
#define CPU_STATE_H

#include <cstdint>

// The Magic of Unions:
// Changing 'x' changes 'l' and 'h' automatically.
// Little Endian: 'l' is the first byte, 'h' is the second.
union Register16 {
    uint16_t x;             // 16-bit access (e.g., AX)
    struct {
        uint8_t l;          // Low byte (e.g., AL)
        uint8_t h;          // High byte (e.g., AH)
    } byte;
};

struct CPUState {
    // General Purpose Registers (AX, BX, CX, DX)
    Register16 ax, bx, cx, dx;

    // Index & Pointer Registers (16-bit only)
    uint16_t sp, bp, si, di;

    // Segment Registers (For 20-bit addressing)
    uint16_t cs, ds, es, ss;

    // Instruction Pointer
    uint16_t ip;

    // Flags Register
    uint16_t flags;

    // Helper to check flags
    bool getFlag(int bit) const { return (flags >> bit) & 1; }
    void setFlag(int bit, bool val) {
        if (val) flags |= (1 << bit);
        else     flags &= ~(1 << bit);
    }
};

#endif // CPU_STATE_H
