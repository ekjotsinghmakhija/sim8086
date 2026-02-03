#ifndef EXECUTION_MANAGER_H
#define EXECUTION_MANAGER_H

#include <QObject>
#include <QTimer>
#include <QLabel>
#include "../cpu8086.h"
#include "../components/code_editor.h"
#include "../components/register_view.h"
#include "../components/memory_view.h"

class ExecutionManager : public QObject {
    Q_OBJECT
public:
    explicit ExecutionManager(CPU8086 *cpu, CodeEditor *editor, RegisterView *reg, MemoryView *mem, QLabel *status);

public slots:
    void compile();
    void step();
    void run();
    void stop();
    void stepBack();

private slots:
    void onTick();

private:
    CPU8086 *cpu;
    CodeEditor *editor;
    RegisterView *regView;
    MemoryView *memView;
    QLabel *statusLabel;

    QTimer *timer;
    CPUState prevState;
    bool isRunning;

    void updateUI();
    void highlightLine(uint16_t cs, uint16_t ip);
};

#endif // EXECUTION_MANAGER_H
