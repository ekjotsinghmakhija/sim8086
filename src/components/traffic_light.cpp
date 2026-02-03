#include "traffic_light.h"
#include <QPainter>

TrafficLight::TrafficLight(QWidget *parent) : QWidget(parent) {
    setFixedSize(60, 160);
    redOn = false; yellowOn = false; greenOn = false;
}

void TrafficLight::setState(uint8_t val) {
    // Bit 0 = Green, Bit 1 = Yellow, Bit 2 = Red
    greenOn  = val & 1;
    yellowOn = val & 2;
    redOn    = val & 4;
    update(); // Trigger repaint
}

void TrafficLight::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Draw Casing
    p.setBrush(QColor("#333"));
    p.setPen(Qt::NoPen);
    p.drawRect(0, 0, width(), height());

    // Draw Lights (Dim if off, Bright if on)
    p.setBrush(redOn ? Qt::red : QColor("#500"));
    p.drawEllipse(10, 10, 40, 40);

    p.setBrush(yellowOn ? Qt::yellow : QColor("#550"));
    p.drawEllipse(10, 60, 40, 40);

    p.setBrush(greenOn ? Qt::green : QColor("#050"));
    p.drawEllipse(10, 110, 40, 40);
}
