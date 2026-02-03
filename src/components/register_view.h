#ifndef REGISTER_VIEW_H
#define REGISTER_VIEW_H

#include <QTableWidget>
#include <QHeaderView>
#include <QContextMenuEvent> // NEW
#include <QAction>           // NEW
#include "../cpu8086.h"

class RegisterView : public QTableWidget {
    Q_OBJECT
public:
    explicit RegisterView(QWidget *parent = nullptr);
    void updateState(const CPUState &state, const CPUState &oldState);
    void updateState(const CPUState &state);

protected:
    // NEW: Detect Right-Click
    void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
    // NEW: Copy Logic
    void copyToClipboard();

private:
    void setupRow(int row, QString name);
};

#endif // REGISTER_VIEW_H
