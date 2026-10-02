#include <QLabel>
#include <QGridLayout>
#include <QPainter>

#include <cmath>
#include <cstddef>
#include <frc/Timer.h>

#include "ShiftWidget.h"

ShiftWidget::Shift ShiftWidget::GetCurrentAlliance() {
    if (! nt::GetTopicExists(isRedSub)) return Shift::NONE;
    bool isRedAlliance = nt::GetBoolean(isRedSub, false);
    Shift alliance = (isRedAlliance == true)? Shift::RED: Shift::BLUE;
    return alliance;
}

ShiftWidget::Shift ShiftWidget::GetActiveAlliance() {
    if (matchTimeLeft <= 20 && robotState == "auto") return Shift::ALL;
    if (matchTimeLeft == -1 || matchTimeLeft > 140 || autoWinnerString == "") return Shift::NONE;
    if (matchTimeLeft > 130 || matchTimeLeft <= 30) return Shift::ALL; // all active
    else if ((matchTimeLeft > 30 && matchTimeLeft <= 55) || 
            (matchTimeLeft > 80 && matchTimeLeft <= 105)
    ) return (autoWinnerString == "R")? Shift::RED: Shift::BLUE; // return winner
    else return (autoWinnerString == "R")? Shift::BLUE: Shift::RED; // return loser
}

std::string ShiftWidget::GetCurrentShiftString() {
    if (matchTimeLeft == -1) return "";
    if (matchTimeLeft <= 20 && robotState == "auto") return "Auto";
    if (matchTimeLeft > 130) return "Transition";
    else if (matchTimeLeft > 30) return "Shift " + std::to_string(int(ceil((131-matchTimeLeft) / 25.0)));
    else return "End Game";
}

double ShiftWidget::GetShiftTime() {
    if (matchTimeLeft == -1) return -1;
    if (matchTimeLeft > 130) return matchTimeLeft - 130;
    else if (matchTimeLeft > 30) return - fmod(6-matchTimeLeft, 25)+1;
    else return matchTimeLeft;
}

double ShiftWidget::GetShiftTimeMax() {
    if (matchTimeLeft == -1) return -1;
    if (matchTimeLeft <= 20 && robotState == "auto") return 20;
    if (matchTimeLeft > 130) return 10;
    else if (matchTimeLeft > 30) return 25;
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

double ShiftWidget::GetMatchTimeLeft() {
    double matchTime = nt::GetDouble(matchTimeSub, -1);
    return (matchTime<0)? -1: ceil(matchTime);
}

void ShiftWidget::paintEvent(QPaintEvent* event) {  
    robotState = nt::GetString(robotStateSub, "");
    matchTimeLeft = GetMatchTimeLeft();
    double blinkClock = frc::GetTime().value();
    double blinkSpeed = 3;
    double minSize = fmin(timerLabel->width(), timerLabel->height())*1.1;
    bool isBlinkVisible = (fmod(blinkClock*blinkSpeed, 1) > 0.5);
    if (matchTimeLeft != lastMatchTimeLeft) {
        lastUpdateTime = frc::GetTime().value();
        activeAlliance = GetActiveAlliance();
        currentAlliance = GetCurrentAlliance();
        autoWinnerString = nt::GetString(gameMessageSub, "");
    } else if (matchTimeLeft == -1) {
        shiftTime = -1;
    }
    shiftTime = GetShiftTime();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::LosslessImageRendering);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
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
    if (activeAlliance == Shift::ALL || (activeAlliance == currentAlliance && activeAlliance != Shift::NONE)) painter.drawText(activeRect, Qt::AlignCenter, "ACTIVE");
    activeRect.setTop(pointSize*2);
    painter.drawText(activeRect, Qt::AlignCenter, QString::fromStdString(GetCurrentShiftString()));

    int arcSize = minSize * 0.8;

    //centered rect
    QRectF boundingRect(center.x() - arcSize / 2.0, 
        center.y() - arcSize / 2.0, 
        arcSize, arcSize
    );

    pen.setWidth(minSize*0.1);
    pen.setColor(QColor("#22FFFFFF"));
    painter.setPen(pen);
    painter.drawArc(boundingRect, 0, 5760);

    std::string allianceColour;
    switch (activeAlliance) {
        case Shift::ALL: {
            allianceColour = "#FFFFFF";
            break;
        }
        case Shift::RED: {
            allianceColour = "#e22e43";
            break;
        }
        case Shift::BLUE: {
            allianceColour = "#3bb1ff";
            break;
        }
        default:
            allianceColour = "#77FFFFFF";
    }
    
    if (matchTimeLeft != lastMatchTimeLeft) shiftTimeMax = GetShiftTimeMax();
    int startAngle = 90;
    double interpolateMod = blinkClock - lastUpdateTime;
    int spanAngle = (shiftTime-interpolateMod)/shiftTimeMax * 360 * 16;
    if (shiftTime != -1) {
        if (isBlinkVisible && activeAlliance == currentAlliance && activeAlliance != Shift::NONE) {
            pen.setColor("#FFFFFF");
            painter.setPen(pen);
            painter.drawArc(boundingRect, startAngle * 16, spanAngle);
        }
    
        pen.setWidth(minSize*0.08);
        pen.setColor(QString::fromStdString(allianceColour));
        painter.setPen(pen);
        painter.drawArc(boundingRect, startAngle * 16, spanAngle);
    }
    lastMatchTimeLeft = matchTimeLeft;
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