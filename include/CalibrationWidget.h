#pragma once

#include "networktables/BooleanTopic.h"
#include "networktables/DoubleTopic.h"
#include "networktables/IntegerTopic.h"
#include <QWidget>

class CalibrationWidget : public QWidget {
    Q_OBJECT

public:

    explicit CalibrationWidget(QWidget* parent = nullptr);
    ~CalibrationWidget() {}
    double widgetSize;

    nt::IntegerSubscriber tagCountSub;
    nt::DoubleSubscriber tagDistSub;
    nt::BooleanSubscriber gyroCalibratedSub;
    nt::BooleanSubscriber manualCalibratedSub;

private:
protected:
    void check();
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
};