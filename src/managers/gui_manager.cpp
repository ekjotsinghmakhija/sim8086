#include "gui_manager.h"
#include <QHBoxLayout>
#include <QSplitter>
#include <QStyle>
#include <QApplication>
#include <QFileDialog>
#include <QTextStream>
#include <QDialog>
#include <QListWidget>
#include <QDialogButtonBox>
#include <QCompleter>

UIManager::UIManager(QMainWindow *window) : QObject(window), parentWindow(window) {}

void UIManager::setupLayout(QWidget *central, CodeEditor *editor, RegisterView *reg, MemoryView *mem, QVBoxLayout *&hwLayout) {
    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    // Right Side (Debug Panel)
    QWidget *rightContainer = new QWidget();
    hwLayout = new QVBoxLayout(rightContainer);
    hwLayout->setContentsMargins(0, 0, 0, 0);

    hwLayout->addWidget(new QLabel("<b>Registers</b>"));
    hwLayout->addWidget(reg, 1);
    hwLayout->addWidget(new QLabel("<b>Memory</b>"));
    hwLayout->addWidget(mem, 2);

    // Splitter
    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(editor);
    splitter->addWidget(rightContainer);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 1);

    mainLayout->addWidget(splitter);
}

void UIManager::setupToolbar(QToolBar *toolbar) {
    // Toolbar actions are managed by MainWindow via public QAction pointers in this class
    // or by binding them directly. Since MainWindow calls this, we just ensure it exists.
    // (In this simplified refactor, MainWindow adds the actions directly using UIManager methods)
}

void UIManager::onNew(CodeEditor *editor) {
    QDialog dlg(parentWindow);
    dlg.setWindowTitle("New Template");
    dlg.resize(400, 300);
    QVBoxLayout *l = new QVBoxLayout(&dlg);

    QListWidget *list = new QListWidget();
    list->addItem("1. COM Template (Simple .COM file)");
    list->addItem("2. EXE Template (Advanced .EXE with Stack)");
    list->addItem("3. Empty Workspace");
    list->addItem("4. Hello World (Standard)");
    list->setCurrentRow(0); // Default to first option
    l->addWidget(list);

    QDialogButtonBox *btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    l->addWidget(btns);

    if (dlg.exec() == QDialog::Accepted) {
        int choice = list->currentRow();
        editor->clear();

        if (choice == 0) { // COM Template
            editor->setPlainText(
                "; COM Template\n"
                "ORG 100h\n\n"
                "start:\n"
                "    MOV AX, 0\n"
                "    ; Your code here\n"
                "    RET\n"
                "END start\n"
            );
        }
        else if (choice == 1) { // EXE Template
            editor->setPlainText(
                "; EXE Template\n"
                ".model small\n"
                ".stack 100h\n"
                ".data\n"
                "    msg db 'Hello', 0\n"
                ".code\n"
                "start:\n"
                "    MOV AX, @data\n"
                "    MOV DS, AX\n"
                "    ; Your code here\n"
                "    MOV AX, 4C00h\n"
                "    INT 21h\n"
                "END start\n"
            );
        }
        else if (choice == 2) { // Empty
            editor->setPlainText("");
        }
        else if (choice == 3) { // Hello World
            editor->setPlainText(
                "; Hello World\n"
                "ORG 100h\n"
                "MOV DX, OFFSET msg\n"
                "MOV AH, 09h\n"
                "INT 21h\n"
                "RET\n"
                "msg DB 'Hello World!$'\n"
            );
        }
    }
}

void UIManager::onOpen(CodeEditor *editor) {
    QString fileName = QFileDialog::getOpenFileName(parentWindow, "Open", "", "ASM (*.asm)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)) editor->setPlainText(file.readAll());
    }
}

void UIManager::onSave(CodeEditor *editor) {
    QString fileName = QFileDialog::getSaveFileName(parentWindow, "Save", "", "ASM (*.asm)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)) {
            QTextStream out(&file);
            out << editor->toPlainText();
        }
    }
}

void UIManager::setupAutocomplete(CodeEditor *editor) {
    QStringList words = {
        "MOV", "ADD", "SUB", "INC", "DEC", "JMP", "JNZ", "JE",
        "CMP", "HLT", "INT", "ORG", "RET", "PUSH", "POP", "CALL",
        "AX", "BX", "CX", "DX", "SI", "DI", "BP", "SP",
        "CS", "DS", "ES", "SS", "AL", "AH", "BL", "BH",
        "CL", "CH", "DL", "DH", "OUT", "IN"
    };
    QCompleter *completer = new QCompleter(words, parentWindow);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::PopupCompletion);

    // Style the popup to match Dark Mode
    QAbstractItemView *popup = completer->popup();
    if(popup) {
        popup->setStyleSheet("background-color: #252526; color: #CCCCCC; border: 1px solid #3F3F46;");
    }

    editor->setCompleter(completer);
}
