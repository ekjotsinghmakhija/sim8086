#ifndef HARDWARE_CONTROLLER_H
#define HARDWARE_CONTROLLER_H

#include <QObject>
#include <QVBoxLayout>
#include "../cpu8086.h"
#include "../components/dos_terminal.h"
#include "../components/traffic_light.h"

class HardwareController : public QObject {
    Q_OBJECT
public:
    explicit HardwareController(QWidget *parent);
    ~HardwareController();

    void setupLayout(QVBoxLayout *layout);
    void connectToCpu(CPU8086 &cpu);

private:
    QWidget *parentWidget;
    DosTerminal *dosTerm;
    TrafficLight *trafficLight;
};

#endif
