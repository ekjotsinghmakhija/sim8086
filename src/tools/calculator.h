#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QDialog>
#include <QLineEdit>
#include <QGridLayout>
#include <QPushButton>

class CalculatorDialog : public QDialog {
    Q_OBJECT
public:
    explicit CalculatorDialog(QWidget *parent = nullptr);

private slots:
    void onDigitClicked();
    void onOpClicked();
    void onEqualClicked();
    void onClearClicked();

private:
    QLineEdit *display;
    QString currentOp;
    double storedVal;
    bool waitingForOperand;
};

#endif // CALCULATOR_H
