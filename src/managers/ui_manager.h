#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <QMainWindow>
#include <QToolBar>
#include <QLabel>
#include <QVBoxLayout>
#include "../components/code_editor.h"
#include "../components/syntax_highlighter.h"
#include "../components/register_view.h"
#include "../components/memory_view.h"

class UIManager : public QObject {
    Q_OBJECT
public:
    explicit UIManager(QMainWindow *window);

    // Setup
    void setupLayout(QWidget *central, CodeEditor *editor, RegisterView *reg, MemoryView *mem, QVBoxLayout *&hwLayout);
    void setupToolbar(QToolBar *toolbar);
    void setupAutocomplete(CodeEditor *editor);

    // File Operations
    void onNew(CodeEditor *editor);
    void onOpen(CodeEditor *editor);
    void onSave(CodeEditor *editor);

    // Getters for Actions (to connect signals)
    QAction *actRun;
    QAction *actStep;
    QAction *actBack;
    QAction *actCompile;
    QAction *actCalc;
    QAction *actConv;
    QAction *actSettings;
    QAction *actHelp;

private:
    QMainWindow *parentWindow;
};

#endif // UI_MANAGER_H
