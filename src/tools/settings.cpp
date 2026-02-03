#include "settings.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QSettings>
#include <QApplication>

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Preferences");
    resize(300, 200);

    auto *layout = new QVBoxLayout(this);
    auto *form = new QFormLayout();

    // Font Size
    fontSizeSpin = new QSpinBox();
    fontSizeSpin->setRange(10, 32);
    fontSizeSpin->setValue(getFontSize());
    form->addRow("Editor Font Size:", fontSizeSpin);

    // Theme
    themeCombo = new QComboBox();
    themeCombo->addItem("Dark Mode");
    themeCombo->addItem("Light Mode");
    themeCombo->setCurrentIndex(isDarkMode() ? 0 : 1);
    form->addRow("UI Theme:", themeCombo);

    layout->addLayout(form);

    // Buttons
    auto *btnApply = new QPushButton("Apply & Save");
    connect(btnApply, &QPushButton::clicked, this, &SettingsDialog::onApply);
    layout->addWidget(btnApply);
}

void SettingsDialog::onApply() {
    QSettings settings("Sim8086", "Config");
    settings.setValue("fontSize", fontSizeSpin->value());
    settings.setValue("darkMode", themeCombo->currentIndex() == 0);

    emit settingsChanged(); // Notify Main Window
    close();
}

int SettingsDialog::getFontSize() {
    QSettings settings("Sim8086", "Config");
    return settings.value("fontSize", 14).toInt();
}

bool SettingsDialog::isDarkMode() {
    QSettings settings("Sim8086", "Config");
    return settings.value("darkMode", true).toBool();
}
