#ifndef EMULATOR_WINDOW_H
#define EMULATOR_WINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTimer>
#include "cpu8086.h"
#include "components/register_view.h"
#include "components/memory_view.h"

class EmulatorWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit EmulatorWindow(CPU8086 *cpuRef, QWidget *parent = nullptr);
    void refresh(); // Force UI update

private slots:
    void onStep();
    void onRun();
    void onStop();
    void onStepBack();
    void onTimerTick();

private:
    CPU8086 *cpu; // Pointer to the shared CPU instance
    QTimer *timer;

    // UI Components
    RegisterView *regView;
    MemoryView *memView;

    QPushButton *btnStep;
    QPushButton *btnRun;
    QPushButton *btnBack;
};

#endif
