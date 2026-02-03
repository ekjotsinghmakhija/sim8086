#include "execution_manager.h"
#include <QTextBlock>

ExecutionManager::ExecutionManager(CPU8086 *c, CodeEditor *e, RegisterView *r, MemoryView *m, QLabel *s)
    : cpu(c), editor(e), regView(r), memView(m), statusLabel(s), isRunning(false) {
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &ExecutionManager::onTick);
    prevState = cpu->getState();
}

void ExecutionManager::compile() {
    stop();
    cpu->compile(editor->toPlainText().toStdString());
    prevState = cpu->getState();
    updateUI();
    statusLabel->setText("Status: Compiled");
}

void ExecutionManager::step() {
    if (cpu->isHalted()) return;
    prevState = cpu->getState();
    cpu->stepForward();
    updateUI();
    statusLabel->setText("Status: Stepped");
}

void ExecutionManager::run() {
    isRunning = true;
    timer->start(16);
    statusLabel->setText("Status: Running");
}

void ExecutionManager::stop() {
    isRunning = false;
    timer->stop();
    statusLabel->setText("Status: Stopped");
}

void ExecutionManager::stepBack() {
    if (cpu->canStepBack()) {
        cpu->stepBackward();
        prevState = cpu->getState();
        updateUI();
        statusLabel->setText("Status: Rewind");
    }
}

void ExecutionManager::onTick() {
    for(int i=0; i<10; i++) {
        if (cpu->isHalted()) {
            stop(); // STOP TIMER AUTOMATICALLY
            statusLabel->setText("Status: Halted");
            updateUI();
            return;
        }
        cpu->stepForward();
    }
    updateUI();
}

void ExecutionManager::updateUI() {
    CPUState curr = cpu->getState();
    regView->updateState(curr, prevState);
    memView->updateMemory(*cpu);
    highlightLine(curr.cs, curr.ip);
}

void ExecutionManager::highlightLine(uint16_t cs, uint16_t ip) {
    uint32_t pc = (uint32_t(cs) << 4) + ip;
    int line = cpu->getLineForAddress(pc);

    QTextCursor c(editor->document()->findBlockByLineNumber(line));
    QTextEdit::ExtraSelection sel;
    sel.format.setBackground(QColor("#264F78"));
    sel.format.setProperty(QTextFormat::FullWidthSelection, true);
    sel.cursor = c;
    editor->setExtraSelections({sel});
    editor->setTextCursor(c); // Scroll to line
}
