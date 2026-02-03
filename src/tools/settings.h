#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = nullptr);

    // Static helpers to get current values
    static int getFontSize();
    static bool isDarkMode();

signals:
    void settingsChanged();

private slots:
    void onApply();

private:
    QSpinBox *fontSizeSpin;
    QComboBox *themeCombo;
};

#endif // SETTINGS_H
