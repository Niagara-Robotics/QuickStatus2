#pragma once

#include "networktables/BooleanTopic.h"
#include "networktables/DoubleTopic.h"
#include "networktables/StringTopic.h"
#include <QLabel>

class ShiftWidget : public QWidget {
    Q_OBJECT

public:

    explicit ShiftWidget(QWidget* parent = nullptr);
    ~ShiftWidget() {}

    static void shouldUpdate();

    enum class Shift {
        NONE,
        RED,
        BLUE,
        ALL
    };

    double getMatchTimeLeft();
    double getShiftTime();
    double getShiftTimeMax();
    Shift getCurrentAlliance();
    std::string getCurrentShiftString();
    std::string getAutoWinnerString();
    Shift getActiveAlliance();

    nt::DoubleSubscriber matchTimeSub;
    nt::StringSubscriber gameMessageSub;
    nt::BooleanSubscriber isRedSub;
    nt::StringSubscriber robotStateSub;

    QLabel* timerLabel = new QLabel();

private:
    double matchTimeLeft = -1;
    double lastMatchTimeLeft = -2;
    double shiftTime = -1;
    double shiftTimeMax = -1;
    Shift activeAlliance = Shift::NONE;
    Shift currentAlliance = Shift::NONE;
    std::string robotState = "";
    std::string autoWinnerString = "";

    double lastUpdateTime;
    
protected:
    void paintEvent(QPaintEvent* event) override;
};