#pragma once

#include <QBoxLayout>
#include <QLabel>
#include "networktables/BooleanTopic.h"

class ControlModeWidget : public QWidget {
    Q_OBJECT

public:

    explicit ControlModeWidget(QWidget* parent = nullptr);
    ~ControlModeWidget() {}

    nt::BooleanSubscriber driverAssistedModeSub;
    nt::BooleanSubscriber fieldCentricSub;

    QLabel* driverLabel = new QLabel();
    QLabel* operatorLabel = new QLabel();
    QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight, this);

protected:
    void resizeEvent(QResizeEvent* event);
    void updateText();
    void resized();
};