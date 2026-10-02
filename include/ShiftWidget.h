#pragma once

#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <ntcore.h>

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

    double GetMatchTimeLeft();
    double GetShiftTime();
    double GetShiftTimeMax();
    Shift GetCurrentAlliance();
    std::string GetCurrentShiftString();
    std::string GetAutoWinnerString();
    Shift GetActiveAlliance();
    void SetupNT();
    void doThing(); //testing
    NT_Inst inst;
    NT_Subscriber matchTimeSub;
    NT_Subscriber gameMessageSub;
    NT_Subscriber isRedSub;
    NT_Subscriber robotStateSub;
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