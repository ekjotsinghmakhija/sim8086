#include "register_view.h"
#include <QMenu>
#include <QApplication>
#include <QClipboard>
#include <QStringBuilder>

RegisterView::RegisterView(QWidget *parent) : QTableWidget(parent) {
    setColumnCount(4);
    setHorizontalHeaderLabels({"Reg", "H", "L", "Val"});
    verticalHeader()->setVisible(false);

    setRowCount(12);
    QStringList names = {"AX", "BX", "CX", "DX", "CS", "IP", "SS", "SP", "BP", "SI", "DI", "DS"};
    for(int i=0; i<names.size(); i++) {
        setItem(i, 0, new QTableWidgetItem(names[i]));
        setItem(i, 1, new QTableWidgetItem("00"));
        setItem(i, 2, new QTableWidgetItem("00"));
        setItem(i, 3, new QTableWidgetItem("0000"));
    }

    setColumnWidth(0, 40);
    setColumnWidth(1, 30);
    setColumnWidth(2, 30);
    horizontalHeader()->setStretchLastSection(true);
}

// --- NEW: Context Menu Event ---
void RegisterView::contextMenuEvent(QContextMenuEvent *event) {
    QMenu menu(this);
    QAction *copyAction = menu.addAction("Copy Table to Clipboard");
    connect(copyAction, &QAction::triggered, this, &RegisterView::copyToClipboard);
    menu.exec(event->globalPos());
}

// --- NEW: Copy Logic ---
void RegisterView::copyToClipboard() {
    QString text;

    // 1. Add Headers
    text += "Reg\tH\tL\tVal\n";
    text += "---\t--\t--\t----\n";

    // 2. Iterate Rows
    for (int i = 0; i < rowCount(); ++i) {
        QString reg = item(i, 0)->text();
        QString h   = item(i, 1)->text();
        QString l   = item(i, 2)->text();
        QString val = item(i, 3)->text();

        // Format with Tabs for easy pasting into Excel/Notepad
        text += reg + "\t" + (h.isEmpty() ? "  " : h) + "\t" + (l.isEmpty() ? "  " : l) + "\t" + val + "\n";
    }

    // 3. Send to System Clipboard
    QApplication::clipboard()->setText(text);
}

void RegisterView::updateState(const CPUState &state) {
    updateState(state, state);
}

void RegisterView::updateState(const CPUState &state, const CPUState &oldState) {
    auto setReg = [&](int row, uint16_t val, uint16_t oldVal, bool split) {
        QTableWidgetItem *itemVal = item(row, 3);
        itemVal->setText(QString("%1").arg(val, 4, 16, QChar('0')).toUpper());

        if (val != oldVal) {
            itemVal->setForeground(QColor("#FF4444"));
            itemVal->setFont(QFont("Consolas", 10, QFont::Bold));
        } else {
            itemVal->setForeground(QColor("#CCCCCC"));
            itemVal->setFont(QFont("Consolas", 10, QFont::Normal));
        }

        if(split) {
            uint8_t h = (val >> 8) & 0xFF;
            uint8_t l = val & 0xFF;
            item(row, 1)->setText(QString("%1").arg(h, 2, 16, QChar('0')).toUpper());
            item(row, 2)->setText(QString("%1").arg(l, 2, 16, QChar('0')).toUpper());
        } else {
             item(row, 1)->setText("");
             item(row, 2)->setText("");
        }
    };

    setReg(0, state.ax.x, oldState.ax.x, true);
    setReg(1, state.bx.x, oldState.bx.x, true);
    setReg(2, state.cx.x, oldState.cx.x, true);
    setReg(3, state.dx.x, oldState.dx.x, true);

    setReg(4, state.cs, oldState.cs, false);
    setReg(5, state.ip, oldState.ip, false);
    setReg(6, state.ss, oldState.ss, false);
    setReg(7, state.sp, oldState.sp, false);
    setReg(8, state.bp, oldState.bp, false);
    setReg(9, state.si, oldState.si, false);
    setReg(10, state.di, oldState.di, false);
    setReg(11, state.ds, oldState.ds, false);
}
