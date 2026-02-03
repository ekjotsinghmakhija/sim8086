#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QLabel>
#include "cpu8086.h"
#include "managers/gui_manager.h"
#include "managers/execution_manager.h"
#include "managers/hardware_controller.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void applySettings(); // Delegate from Settings Tool

private:
    // Core
    CPU8086 cpu;

    // Components (Owned by MainWindow)
    CodeEditor *editor;
    RegisterView *regView;
    MemoryView *memView;
    QLabel *statusLabel;
    QVBoxLayout *rightLayout;

    // Managers
    UIManager *uiManager;
    ExecutionManager *execManager;
    HardwareController *hwController;
};

#endif // MAINWINDOW_H
