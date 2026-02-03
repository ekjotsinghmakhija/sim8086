#include "calculator.h"
#include <QRegularExpression> // <--- THIS WAS MISSING

CalculatorDialog::CalculatorDialog(QWidget *parent) : QDialog(parent), storedVal(0.0), waitingForOperand(true) {
    setWindowTitle("Mini Calc");
    resize(250, 300);

    auto *layout = new QGridLayout(this);
    display = new QLineEdit("0");
    display->setAlignment(Qt::AlignRight);
    display->setReadOnly(true);
    layout->addWidget(display, 0, 0, 1, 4);

    const QString buttons[4][4] = {
        {"7", "8", "9", "/"},
        {"4", "5", "6", "*"},
        {"1", "2", "3", "-"},
        {"0", "C", "=", "+"}
    };

    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            auto *btn = new QPushButton(buttons[r][c]);
            layout->addWidget(btn, r + 1, c);

            // This line caused the error because QRegularExpression wasn't included
            if (buttons[r][c].contains(QRegularExpression("[0-9]"))) {
                connect(btn, &QPushButton::clicked, this, &CalculatorDialog::onDigitClicked);
            } else if (buttons[r][c] == "C") {
                connect(btn, &QPushButton::clicked, this, &CalculatorDialog::onClearClicked);
            } else if (buttons[r][c] == "=") {
                connect(btn, &QPushButton::clicked, this, &CalculatorDialog::onEqualClicked);
            } else {
                connect(btn, &QPushButton::clicked, this, &CalculatorDialog::onOpClicked);
            }
        }
    }
}

void CalculatorDialog::onDigitClicked() {
    auto *btn = qobject_cast<QPushButton*>(sender());
    if (waitingForOperand) {
        display->setText(btn->text());
        waitingForOperand = false;
    } else {
        display->setText(display->text() + btn->text());
    }
}

void CalculatorDialog::onOpClicked() {
    auto *btn = qobject_cast<QPushButton*>(sender());
    storedVal = display->text().toDouble();
    currentOp = btn->text();
    waitingForOperand = true;
}

void CalculatorDialog::onEqualClicked() {
    double operand = display->text().toDouble();
    double result = 0.0;

    if (currentOp == "+") result = storedVal + operand;
    else if (currentOp == "-") result = storedVal - operand;
    else if (currentOp == "*") result = storedVal * operand;
    else if (currentOp == "/") result = storedVal / operand;

    display->setText(QString::number(result));
    waitingForOperand = true;
}

void CalculatorDialog::onClearClicked() {
    display->setText("0");
    storedVal = 0.0;
    waitingForOperand = true;
}
