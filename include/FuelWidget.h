#pragma once

#include "ntcore_c.h"
#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <ntcore.h>

class FuelWidget : public QWidget {
    Q_OBJECT

public:

    explicit FuelWidget(QWidget* parent = nullptr);
    ~FuelWidget() {}
    NT_Inst inst;
    NT_Subscriber detectorSub;
    NT_Subscriber targetDataSub;
    QLabel* textLabel = new QLabel();
    std::string detector;
    int tagCount;
private:
    QTimer refreshTimer;
protected:
    void check();
    void paintEvent(QPaintEvent* event) override;
};