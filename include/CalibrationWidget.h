#pragma once

#include <QWidget>
#include <ntcore.h>

class CalibrationWidget : public QWidget {
    Q_OBJECT

public:

    explicit CalibrationWidget(QWidget* parent = nullptr);
    ~CalibrationWidget() {}
    double widgetSize;
    NT_Inst inst;
    NT_Subscriber tagCountSub;
    NT_Subscriber tagDistSub;
    NT_Subscriber gyroCalibratedSub;
    NT_Subscriber manualCalibratedSub;

private:
protected:
    void check();
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
};