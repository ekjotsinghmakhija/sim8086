#include "hardware_controller.h"
#include <QLabel>

HardwareController::HardwareController(QWidget *parent) : QObject(parent), parentWidget(parent) {
    // Create devices but don't show them yet
    // Note: We pass 'parent' (MainWindow) to them, so MainWindow owns them.
    // This means MainWindow will automatically delete them when it closes.
    dosTerm = new DosTerminal(parent);
    trafficLight = new TrafficLight(parent);

    // Explicitly hide traffic light (it remains in memory, just not visible)
    trafficLight->hide();
}

HardwareController::~HardwareController() {
    // FIX: Do NOT delete dosTerm or trafficLight here.
    // They are children of 'parentWidget', so Qt handles their deletion automatically.
    // Deleting them here causes a "Double Free" crash on exit.
}

void HardwareController::setupLayout(QVBoxLayout *layout) {
    // We keep the hardware hidden from the main layout for a cleaner UI
    layout->addStretch();
}

void HardwareController::connectToCpu(CPU8086 &cpu) {
    // 1. DOS Terminal
    // Connects CPU output interrupt to the terminal window
    cpu.outputCallback = [this](char c) {
        // "Why does it open?" -> Because we force it to show here!
        if (!dosTerm->isVisible()) dosTerm->show();
        dosTerm->writeChar(c);
    };

    // 2. Traffic Light
    cpu.port199Callback = [this](uint8_t val) {
        trafficLight->setState(val);
    };
}
