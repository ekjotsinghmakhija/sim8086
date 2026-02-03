#include "cpu8086.h"
#include <cstring>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <map>

CPU8086::CPU8086() {
    memory.resize(1024 * 1024, 0);
    reset();
}

void CPU8086::reset() {
    std::lock_guard<std::mutex> lock(stateMutex);
    std::fill(memory.begin(), memory.end(), 0);

    state.ax.x = 0; state.bx.x = 0; state.cx.x = 0; state.dx.x = 0;
    state.cs = 0; state.ds = 0; state.es = 0; state.ss = 0;
    state.ip = 0;
    state.flags = 0;
    state.sp = 0xFFFE;
    state.cycles = 0;
    halted = false; // Reset halted state

    historyBuffer.clear();
    debugLineMap.clear();
}

bool CPU8086::isHalted() const { return halted; }

void CPU8086::saveSnapshot(uint32_t modifiedAddr) {
    Snapshot snap;
    snap.state = state;
    snap.memory = memory;
    snap.lastModifiedAddr = modifiedAddr;
    if (historyBuffer.size() >= 200) historyBuffer.erase(historyBuffer.begin());
    historyBuffer.push_back(snap);
}

CPUState CPU8086::getState() const {
    std::lock_guard<std::mutex> lock(stateMutex);
    return state;
}

uint8_t CPU8086::getMemoryAt(uint32_t addr) const {
    std::lock_guard<std::mutex> lock(stateMutex);
    if (addr >= memory.size()) return 0;
    return memory[addr];
}

int CPU8086::getLineForAddress(uint32_t addr) const {
    if (debugLineMap.empty()) return 0;
    auto it = debugLineMap.upper_bound(addr);
    if (it == debugLineMap.begin()) return 0;
    return (--it)->second;
}

uint32_t CPU8086::getPhysicalAddress(uint16_t segment, uint16_t offset) {
    return (uint32_t(segment) << 4) + offset;
}

int CPU8086::getRegIndex(std::string regName) {
    if (regName == "AX") return 0;
    if (regName == "CX") return 1;
    if (regName == "DX") return 2;
    if (regName == "BX") return 3;
    return -1;
}

void CPU8086::compile(const std::string& sourceCode) {
    reset();
    std::lock_guard<std::mutex> lock(stateMutex);

    std::map<std::string, uint32_t> labelMap;

    // --- TWO-PASS COMPILER ---
    // Pass 1: Find Labels
    // Pass 2: Generate Code & Resolve Jumps
    for (int pass = 1; pass <= 2; pass++) {
        std::stringstream ss(sourceCode);
        std::string line;
        uint32_t writeAddr = 0;
        int lineNumber = 0;

        while (std::getline(ss, line)) {
            // 1. Cleanup: Strip Comments
            std::string cleanLine = line;
            size_t commentPos = cleanLine.find(';');
            if (commentPos != std::string::npos) cleanLine = cleanLine.substr(0, commentPos);

            // 2. Normalize: Replace commas/tabs with spaces
            std::replace(cleanLine.begin(), cleanLine.end(), ',', ' ');
            std::replace(cleanLine.begin(), cleanLine.end(), '\t', ' ');

            // 3. Trim Whitespace
            cleanLine.erase(0, cleanLine.find_first_not_of(" \r\n"));
            if (cleanLine.find_last_not_of(" \r\n") != std::string::npos)
                cleanLine.erase(cleanLine.find_last_not_of(" \r\n") + 1);

            if (cleanLine.empty()) { lineNumber++; continue; }

            // 4. Map Line Number (Pass 2 only)
            if (pass == 2) debugLineMap[writeAddr] = lineNumber;

            // 5. Prepare Tokens for DB/Label checks
            std::string upperLine = cleanLine;
            std::transform(upperLine.begin(), upperLine.end(), upperLine.begin(), ::toupper);
            std::stringstream ls_check(upperLine);
            std::string t1, t2;
            ls_check >> t1 >> t2;

            // --- SPECIAL HANDLING FOR "DB" (Data Byte) ---
            if (t1 == "DB" || t2 == "DB") {
                if (t2 == "DB") {
                    // It's a variable! (e.g. msg_start DB)
                    if (pass == 1) labelMap[t1] = writeAddr;
                }

                // Parse the string from original 'line' to preserve case
                size_t quoteStart = line.find("'");
                size_t quoteEnd = line.rfind("'");
                if (quoteStart != std::string::npos && quoteEnd > quoteStart) {
                    std::string content = line.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
                    for (char c : content) {
                        if (pass == 2) memory[writeAddr] = (uint8_t)c;
                        writeAddr++;
                    }
                }
                lineNumber++;
                continue; // Done with this line
            }

            // 6. Label Detection (Ends with :)
            if (cleanLine.back() == ':') {
                std::string labelName = cleanLine.substr(0, cleanLine.length() - 1);
                // Trim again in case of "Label :"
                if (labelName.find_last_not_of(" ") != std::string::npos)
                    labelName.erase(labelName.find_last_not_of(" ") + 1);

                std::transform(labelName.begin(), labelName.end(), labelName.begin(), ::toupper);

                if (pass == 1) labelMap[labelName] = writeAddr;
                lineNumber++;
                continue;
            }

            // 7. Tokenize Instruction
            std::stringstream ls(upperLine);
            std::string opcode, arg1, arg2;
            ls >> opcode;

            if (opcode == "ORG") {
                ls >> arg1;
                if (!arg1.empty() && arg1.back() == 'H') arg1.pop_back();
                try {
                    uint32_t orgVal = std::stoi(arg1, nullptr, 16);
                    writeAddr = orgVal;
                    if (pass == 2) state.ip = (uint16_t)orgVal;
                } catch(...) {}
            }
            else if (opcode == "MOV") {
                ls >> arg1 >> arg2;

                if (arg2 == "OFFSET") { // MOV DX OFFSET msg
                    std::string targetLabel; ls >> targetLabel;
                    if (pass == 2) {
                        uint16_t addr = labelMap[targetLabel]; // Look up address
                        int reg = getRegIndex(arg1);
                        memory[writeAddr] = 0xB8 + reg;
                        memory[writeAddr+1] = addr & 0xFF;
                        memory[writeAddr+2] = (addr >> 8) & 0xFF;
                    }
                    writeAddr += 3;
                }
                else if (arg1 == "AH" && arg2 == "09H") {
                    if (pass == 2) { memory[writeAddr] = 0xB4; memory[writeAddr+1] = 0x09; }
                    writeAddr += 2;
                }
                else {
                    int reg = getRegIndex(arg1);
                    int val = 0;
                    try {
                        if (!arg2.empty() && arg2.back() == 'H') arg2.pop_back();
                        val = std::stoi(arg2, nullptr, 16);
                    } catch(...) {}

                    if (pass == 2) {
                        memory[writeAddr] = 0xB8 + reg;
                        memory[writeAddr+1] = val & 0xFF;
                        memory[writeAddr+2] = (val >> 8) & 0xFF;
                    }
                    writeAddr += 3;
                }
            }
            else if (opcode == "ADD") {
                ls >> arg1 >> arg2;
                int val = 0;
                try { val = std::stoi(arg2, nullptr, 16); } catch(...) {}
                if (pass == 2) { memory[writeAddr] = 0x05; memory[writeAddr+1] = val & 0xFF; memory[writeAddr+2] = (val >> 8) & 0xFF; }
                writeAddr += 3;
            }
            else if (opcode == "SUB") {
                ls >> arg1 >> arg2;
                int val = 0;
                try { val = std::stoi(arg2, nullptr, 16); } catch(...) {}
                if (pass == 2) { memory[writeAddr] = 0x2D; memory[writeAddr+1] = val & 0xFF; memory[writeAddr+2] = 0x00; }
                writeAddr += 3;
            }
            else if (opcode == "MUL") {
                ls >> arg1;
                int reg = getRegIndex(arg1);
                if (pass == 2) { memory[writeAddr] = 0xF7; memory[writeAddr+1] = 0xE0 + reg; }
                writeAddr += 2;
            }
            else if (opcode == "DIV") {
                ls >> arg1;
                int reg = getRegIndex(arg1);
                if (pass == 2) { memory[writeAddr] = 0xF7; memory[writeAddr+1] = 0xF0 + reg; }
                writeAddr += 2;
            }
            else if (opcode == "INT") {
                if (pass == 2) { memory[writeAddr] = 0xCD; memory[writeAddr+1] = 0x21; }
                writeAddr += 2;
            }
            else if (opcode == "OUT") {
                ls >> arg1 >> arg2;
                if (pass == 2) { memory[writeAddr] = 0xE6; memory[writeAddr+1] = 199; }
                writeAddr += 2;
            }
            else if (opcode == "PUSH") {
                ls >> arg1;
                if (pass == 2) {
                    if(arg1 == "AX") memory[writeAddr] = 0x50;
                    else if(arg1 == "CX") memory[writeAddr] = 0x51;
                    else if(arg1 == "DX") memory[writeAddr] = 0x52;
                    else if(arg1 == "BX") memory[writeAddr] = 0x53;
                }
                writeAddr += 1;
            }
            else if (opcode == "POP") {
                ls >> arg1;
                if (pass == 2) {
                    if(arg1 == "AX") memory[writeAddr] = 0x58;
                    else if(arg1 == "CX") memory[writeAddr] = 0x59;
                    else if(arg1 == "DX") memory[writeAddr] = 0x5A;
                    else if(arg1 == "BX") memory[writeAddr] = 0x5B;
                }
                writeAddr += 1;
            }
            else if (opcode == "CALL") {
                ls >> arg1; // Target Label
                if (pass == 2) {
                    uint32_t target = labelMap[arg1];
                    // E8 rel16: Offset = Target - (NextInstructionAddr)
                    int relative = target - (writeAddr + 3);
                    memory[writeAddr] = 0xE8;
                    memory[writeAddr+1] = relative & 0xFF;
                    memory[writeAddr+2] = (relative >> 8) & 0xFF;
                }
                writeAddr += 3;
            }
            else if (opcode == "RET") {
                if (pass == 2) memory[writeAddr] = 0xC3;
                writeAddr += 1;
            }
            else if (opcode == "HLT") {
                if (pass == 2) memory[writeAddr] = 0xF4;
                writeAddr += 1;
            }

            lineNumber++;
        }
    }
}
