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

    double GetStartTime();
    double GetShiftTime();
    double GetShiftTimeMax();
    std::string GetCurrentAlliance();
    std::string GetCurrentShift();
    std::string GetAutoWinner();
    std::string GetActiveAlliance();
    void SetupNT();
    void doThing(); //testing
    NT_Inst inst;
    NT_Subscriber robotStateSub;
    NT_Subscriber gameMessageSub;
    NT_Subscriber isRedSub;
    QLabel* timerLabel = new QLabel();

private:
    QTimer refreshTimer;

protected:
    void paintEvent(QPaintEvent* event) override;
};