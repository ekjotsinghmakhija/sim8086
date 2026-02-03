#include "dos_terminal.h"
#include <QVBoxLayout>

DosTerminal::DosTerminal(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Virtual DOS Terminal");
    resize(640, 400);
    setStyleSheet("background-color: #000000;");

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);

    screen = new QTextEdit();
    screen->setReadOnly(true);
    // Retro Style: Green text on Black, Monospace
    screen->setStyleSheet("font-family: 'Consolas', 'Courier New'; font-size: 16px; color: #00FF00; background-color: #000000; border: none;");
    screen->setFrameStyle(QFrame::NoFrame);

    layout->addWidget(screen);
}

void DosTerminal::writeChar(char c) {
    screen->moveCursor(QTextCursor::End);
    screen->insertPlainText(QString(c));
    screen->moveCursor(QTextCursor::End);
}

void DosTerminal::writeString(const QString &str) {
    screen->moveCursor(QTextCursor::End);
    screen->insertPlainText(str);
    screen->moveCursor(QTextCursor::End);
}

void DosTerminal::clearScreen() {
    screen->clear();
}
