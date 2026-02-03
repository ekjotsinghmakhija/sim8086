#include "converter.h"

ConverterDialog::ConverterDialog(QWidget *parent) : QDialog(parent), isUpdating(false) {
    setWindowTitle("Base Converter");
    resize(300, 150);

    auto *layout = new QFormLayout(this);

    hexEdit = new QLineEdit();
    decEdit = new QLineEdit();
    binEdit = new QLineEdit();

    layout->addRow("Hexadecimal (16):", hexEdit);
    layout->addRow("Decimal (10):", decEdit);
    layout->addRow("Binary (2):", binEdit);

    connect(hexEdit, &QLineEdit::textChanged, this, &ConverterDialog::onHexChanged);
    connect(decEdit, &QLineEdit::textChanged, this, &ConverterDialog::onDecChanged);
    connect(binEdit, &QLineEdit::textChanged, this, &ConverterDialog::onBinChanged);
}

void ConverterDialog::onHexChanged(const QString &text) {
    if (isUpdating) return;
    isUpdating = true;
    bool ok;
    long long val = text.toLongLong(&ok, 16);
    if (ok) {
        decEdit->setText(QString::number(val, 10));
        binEdit->setText(QString::number(val, 2));
    }
    isUpdating = false;
}

void ConverterDialog::onDecChanged(const QString &text) {
    if (isUpdating) return;
    isUpdating = true;
    bool ok;
    long long val = text.toLongLong(&ok, 10);
    if (ok) {
        hexEdit->setText(QString::number(val, 16).toUpper());
        binEdit->setText(QString::number(val, 2));
    }
    isUpdating = false;
}

void ConverterDialog::onBinChanged(const QString &text) {
    if (isUpdating) return;
    isUpdating = true;
    bool ok;
    long long val = text.toLongLong(&ok, 2);
    if (ok) {
        hexEdit->setText(QString::number(val, 16).toUpper());
        decEdit->setText(QString::number(val, 10));
    }
    isUpdating = false;
}
