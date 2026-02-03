#ifndef CONVERTER_H
#define CONVERTER_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QFormLayout>

class ConverterDialog : public QDialog {
    Q_OBJECT
public:
    explicit ConverterDialog(QWidget *parent = nullptr);

private slots:
    void onHexChanged(const QString &text);
    void onDecChanged(const QString &text);
    void onBinChanged(const QString &text);

private:
    QLineEdit *hexEdit;
    QLineEdit *decEdit;
    QLineEdit *binEdit;
    bool isUpdating; // Prevent recursive loops
};

#endif // CONVERTER_H
