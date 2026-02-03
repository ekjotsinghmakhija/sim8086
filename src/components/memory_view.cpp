#include "memory_view.h"
#include <QStringList>

MemoryView::MemoryView(QWidget *parent) : QTableWidget(parent) {
    setupTable();
}

void MemoryView::setupTable() {
    setColumnCount(3);
    setHorizontalHeaderLabels({"Address", "Bin", "Assembly"});

    // Styling to match SIM8086 look
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    horizontalHeader()->setStretchLastSection(true);

    verticalHeader()->setVisible(false);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setStyleSheet("font-family: Consolas; font-size: 13px;");
    setRowCount(20); // Show next 20 instructions
}

void MemoryView::updateMemory(const CPU8086 &cpu) {
    CPUState state = cpu.getState();
    uint32_t pc = (state.cs << 4) + state.ip;

    // Simple Disassembly Loop (Visual only)
    for(int i=0; i<20; i++) {
        uint32_t currentAddr = pc + (i * 3); // Approx stepping
        uint8_t op = cpu.getMemoryAt(currentAddr);
        uint8_t arg1 = cpu.getMemoryAt(currentAddr+1);
        uint8_t arg2 = cpu.getMemoryAt(currentAddr+2);

        // 1. Address Column (CS:IP format)
        QString addrStr = QString("%1:%2")
            .arg(state.cs, 4, 16, QChar('0'))
            .arg(state.ip + (i*3), 4, 16, QChar('0')).toUpper();

        // 2. Binary Column (B8 34 12)
        QString binStr = QString("%1 %2 %3")
            .arg(op, 2, 16, QChar('0'))
            .arg(arg1, 2, 16, QChar('0'))
            .arg(arg2, 2, 16, QChar('0')).toUpper();

        // 3. Assembly Mnemonic (Rudimentary decoding for display)
        QString asmStr = "NOP";
        if(op >= 0xB8 && op <= 0xBF) asmStr = "MOV R, imm";
        if(op == 0x05) asmStr = "ADD AX, imm";
        if(op == 0xF4) asmStr = "HLT";

        setItem(i, 0, new QTableWidgetItem(addrStr));
        setItem(i, 1, new QTableWidgetItem(binStr));
        setItem(i, 2, new QTableWidgetItem(asmStr));

        // Highlight the first row (Current Instruction)
        if (i == 0) {
            item(i, 0)->setBackground(QColor("#000080")); // Dark Blue
            item(i, 0)->setForeground(Qt::white);
            item(i, 1)->setBackground(QColor("#000080"));
            item(i, 1)->setForeground(Qt::white);
            item(i, 2)->setBackground(QColor("#000080"));
            item(i, 2)->setForeground(Qt::white);
        }
    }
}
