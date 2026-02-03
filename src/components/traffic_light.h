#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H

#include <QWidget>

class TrafficLight : public QWidget {
    Q_OBJECT
public:
    explicit TrafficLight(QWidget *parent = nullptr);
    void setState(uint8_t val); // 1=Red, 2=Yellow, 4=Green

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    bool redOn, yellowOn, greenOn;
};

#endif
