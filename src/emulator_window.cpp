#include "emulator_window.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolBar>

EmulatorWindow::EmulatorWindow(CPU8086 *cpuRef, QWidget *parent)
    : QMainWindow(parent), cpu(cpuRef)
{
    setWindowTitle("Emulator - [0100:0000]");
    resize(800, 500);

    // --- Toolbar (Top Controls like Image 2) ---
    QToolBar *toolbar = addToolBar("Debug");
    toolbar->setMovable(false);

    btnBack = new QPushButton("Step Back");
    btnStep = new QPushButton("Single Step");
    btnRun = new QPushButton("Run");
    auto *btnReload = new QPushButton("Reload"); // Reset

    toolbar->addWidget(new QPushButton("Load")); // Dummy
    toolbar->addWidget(btnReload);
    toolbar->addSeparator();
    toolbar->addWidget(btnBack);
    toolbar->addWidget(btnStep);
    toolbar->addWidget(btnRun);

    // --- Main Layout ---
    QWidget *central = new QWidget;
    setCentralWidget(central);
    QHBoxLayout *mainLayout = new QHBoxLayout(central);

    // Left Side: Registers
    regView = new RegisterView();
    regView->setFixedWidth(240); // Matches Image 2 Sidebar width

    // Right Side: Memory View
    memView = new MemoryView();

    mainLayout->addWidget(regView);
    mainLayout->addWidget(memView);

    // --- Logic ---
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &EmulatorWindow::onTimerTick);

    connect(btnStep, &QPushButton::clicked, this, &EmulatorWindow::onStep);
    connect(btnRun, &QPushButton::clicked, this, &EmulatorWindow::onRun);
    connect(btnBack, &QPushButton::clicked, this, &EmulatorWindow::onStepBack);

    refresh();
}

void EmulatorWindow::refresh() {
    regView->updateState(cpu->getState());
    memView->updateMemory(*cpu);
}

void EmulatorWindow::onStep() {
    cpu->stepForward();
    refresh();
}

void EmulatorWindow::onRun() {
    timer->start(50); // Fast execution
}

void EmulatorWindow::onTimerTick() {
    cpu->stepForward();
    refresh();
}

void EmulatorWindow::onStop() {
    timer->stop();
}

void EmulatorWindow::onStepBack() {
    if(cpu->canStepBack()) {
        cpu->stepBackward();
        refresh();
    }
}
