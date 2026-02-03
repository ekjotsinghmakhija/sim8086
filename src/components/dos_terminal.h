#ifndef DOS_TERMINAL_H
#define DOS_TERMINAL_H

#include <QDialog>
#include <QTextEdit>

class DosTerminal : public QDialog {
    Q_OBJECT
public:
    explicit DosTerminal(QWidget *parent = nullptr);
    void writeChar(char c);
    void writeString(const QString &str);
    void clearScreen();

private:
    QTextEdit *screen;
};

#endif // DOS_TERMINAL_H
