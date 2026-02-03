#ifndef MEMORY_VIEW_H
#define MEMORY_VIEW_H

#include <QTableWidget>
#include <QHeaderView>
#include "../cpu8086.h"

class MemoryView : public QTableWidget {
    Q_OBJECT
public:
    explicit MemoryView(QWidget *parent = nullptr);
    void updateMemory(const CPU8086 &cpu);

private:
    void setupTable();
};

#endif // MEMORY_VIEW_H
