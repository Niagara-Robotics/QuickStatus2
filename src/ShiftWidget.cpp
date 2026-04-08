#include <QLabel>
#include <QGridLayout>
#include <QPainter>

#include <cmath>
#include <frc/Timer.h>

#include "ShiftWidget.h"
#include "ntcore_c.h"
#include "ntcore_cpp_types.h"

void ShiftWidget::doThing() {
    timerLabel->setText(QString::number(GetShiftTime())+" "+QString::fromStdString(GetActiveAlliance()));
}

std::string ShiftWidget::GetCurrentAlliance() {
    if (! nt::GetTopicExists(isRedSub)) return "";
    bool isRedAlliance = nt::GetBoolean(isRedSub, false);
    std::string alliance = (isRedAlliance == true)? "R": "B";
    return alliance;
}

std::string ShiftWidget::GetActiveAlliance() {
    double timeLeft = GetTimeLeft();
    std::string autoWinner = GetAutoWinner();
    std::string autoLoser = (autoWinner == "R")? "B": "R";
    std::string robotState = nt::GetString(robotStateSub, "");
    if (timeLeft <= 20 && robotState == "auto") return "A";
    if (timeLeft == -1 || timeLeft > 140 || autoWinner == "") return "";
    if (timeLeft > 130 || timeLeft <= 30) return "A"; // all active
    else if ((timeLeft > 30 && timeLeft <= 55) || 
            (timeLeft > 80 && timeLeft <= 105)
    ) return autoWinner;
    else return autoLoser;
}

std::string ShiftWidget::GetCurrentShift() {
    double timeLeft = GetTimeLeft();
    std::string robotState = nt::GetString(robotStateSub, "");
    if (timeLeft == -1) return "";
    if (timeLeft <= 20 && robotState == "auto") return "Auto";
    if (timeLeft > 130) return "Transition";
    else if (timeLeft > 30) return "Shift " + std::to_string(int(ceil((131-timeLeft) / 25.0)));
    else return "End Game";
}

double ShiftWidget::GetShiftTime() {
    double timeLeft = GetTimeLeft();
    if (timeLeft == -1) return -1;
    if (timeLeft > 130) return timeLeft - 130;
    else if (timeLeft > 30) return - fmod(6-timeLeft, 25)+1;
    else return timeLeft;
}

double ShiftWidget::GetShiftTimeMax() {
    double timeLeft = GetTimeLeft();
    std::string robotState = nt::GetString(robotStateSub, "");
    if (timeLeft == -1) return -1;
    if (timeLeft <= 20 && robotState == "auto") return 20;
    if (timeLeft > 130) return 10;
    else if (timeLeft > 30) return 25;
    else return 30;
}

void ShiftWidget::SetupNT() {
    inst = nt::GetDefaultInstance();
    matchTimeSub = nt::Subscribe(nt::GetTopic(
        inst, "/SmartDashboard/matchTime"), NT_DOUBLE, "double"
    );
    gameMessageSub = nt::Subscribe(nt::GetTopic(
        inst, "/FMSInfo/GameSpecificMessage"), NT_STRING, "string"
    );
    isRedSub = nt::Subscribe(nt::GetTopic(
        inst, "/FMSInfo/IsRedAlliance"), NT_BOOLEAN, "boolean"
    );
    robotStateSub = nt::Subscribe(nt::GetTopic(
        inst, "/SmartDashboard/robotState"), NT_STRING, "string"
    );
}

std::string ShiftWidget::GetAutoWinner() {
    std::string gabeMessage = nt::GetString(gameMessageSub, "");
    return gabeMessage; //gabe has taken over FRC and is infecting my variable names
}

double ShiftWidget::GetTimeLeft() {
    auto matchTime = nt::GetDouble(matchTimeSub, -1);
    return (matchTime<0)? -1: ceil(matchTime);
}

void ShiftWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::LosslessImageRendering);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    
    double shiftTime = GetShiftTime();
    double blinkClock = frc::GetTime().value();
    double blinkSpeed = 3;
    double minSize = fmin(timerLabel->width(), timerLabel->height())*1.1;
    bool isBlinkVisible = (fmod(blinkClock*blinkSpeed, 1) > 0.5);
    std::string activeAlliance = GetActiveAlliance();
    std::string currentAlliance = GetCurrentAlliance();

    //timer display
    float pointSize = minSize * 0.2;
    if (shiftTime == -1) { //invalid data
        timerLabel->setText(QString::fromStdString(fmt::format(
            "<span style='font-size: {}px;'>—</span>",
            pointSize*1.2
        )));
    } else { //yippie display the timer!!
        timerLabel->setText(QString::fromStdString(fmt::format(
            "<span style='font-size: {}px;'>{}</span>",
            pointSize*1.2, round(shiftTime)
        )));
    }

    QPen pen(QColor("#FFFFFF"), 32, Qt::SolidLine, Qt::RoundCap);
    painter.setPen(pen);
    QRect activeRect = rect();
    activeRect.setTop(-pointSize*2);
    QPoint center = rect().center();
    painter.setFont(QFont("B612", pointSize*0.4, 900));
    if (activeAlliance == "A" || (activeAlliance == currentAlliance && activeAlliance != "")) painter.drawText(activeRect, Qt::AlignCenter, "ACTIVE");
    activeRect.setTop(pointSize*2);
    painter.drawText(activeRect, Qt::AlignCenter, QString::fromStdString(GetCurrentShift()));

    int arcWidth = minSize * 0.8;
    int arcHeight = arcWidth;

    //centered rect
    QRectF boundingRect(center.x() - arcWidth / 2.0, 
        center.y() - arcHeight / 2.0, 
        arcWidth, arcHeight
    );

    pen.setWidth(minSize*0.1);
    pen.setColor(QColor("#22FFFFFF"));
    painter.setPen(pen);
    painter.drawArc(boundingRect, 0, 5760);

    std::string allianceColour = (activeAlliance == "R")? "#e22e43": "#3bb1ff";
    allianceColour = (activeAlliance == "")? "#77FFFFFF": allianceColour;
    if (activeAlliance == "A") allianceColour = "#FFFFFF";

    double shiftTimeMax = GetShiftTimeMax();
    int startAngle = 90;
    int spanAngle = shiftTime/shiftTimeMax * 360 * 16;
    if (shiftTime != -1) {
        if (isBlinkVisible && activeAlliance == currentAlliance && activeAlliance != "") {
            pen.setColor("#FFFFFF");
            painter.setPen(pen);
            painter.drawArc(boundingRect, startAngle * 16, spanAngle);
        }
    
        pen.setWidth(minSize*0.08);
        pen.setColor(QString::fromStdString(allianceColour));
        painter.setPen(pen);
        painter.drawArc(boundingRect, startAngle * 16, spanAngle);
    }
}

ShiftWidget::ShiftWidget(QWidget* parent):QWidget(parent) {
    setWindowTitle("Alliance Shifts");

    SetupNT();

    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(timerLabel);
    setLayout(layout);

    timerLabel->setFont(QFont("B612 Mono"));
    timerLabel->setAlignment(Qt::AlignCenter);
}