#include <QLabel>
#include <QGridLayout>
#include <QPainter>

#include <frc/Timer.h>

#include "ShiftWidget.h"
#include "networktables/NetworkTableValue.h"
#include "ntcore_c.h"
#include "ntcore_cpp.h"

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
    double startTime = GetStartTime();
    std::string autoWinner = GetAutoWinner();
    std::string autoLoser = (autoWinner == "R")? "B": "R";
    
    if (startTime == -1 || startTime > 140 || autoWinner == "") return "";
    if (startTime <= 10 || startTime > 110) {
        return "A"; // all active
    } else if ((startTime > 10 && startTime <= 35) || (startTime > 60 && startTime <= 85)) {
        return autoLoser;
    } else return autoWinner;
}

std::string ShiftWidget::GetCurrentShift() {
    double startTime = GetStartTime();
    if (startTime == -1) return "";
    if (startTime <= 10) {
        return "Transition";
    } else if (startTime > 10 && startTime <= 110) {
        return "Shift " + std::to_string(int(ceil((startTime-10) / 25.0)));
    } else if (startTime > 110 && startTime <= 140) {
        return "End Game";
    } else return "";
}

double ShiftWidget::GetShiftTime() {
    double startTime = GetStartTime();
    if (startTime == -1) return -1;
    if (startTime <= 10) {
        return 10 - startTime;
    } else if (startTime > 10 && startTime <= 110) {
        return 25 - fmod(startTime-10, 25);
    } else if (startTime > 110 && startTime <= 140) {
        return 140 - startTime;
    } else return -1;
}

double ShiftWidget::GetShiftTimeMax() {
    double startTime = GetStartTime();
    if (startTime == -1) return -1;
    if (startTime <= 10) {
        return 10;
    } else if (startTime > 10 && startTime <= 110) {
        return 25;
    } else if (startTime > 110 && startTime <= 140) {
        return 30;
    } else return -1;
}

void ShiftWidget::SetupNT() {
    inst = nt::GetDefaultInstance();
    robotStateSub = nt::Subscribe(nt::GetTopic(
        inst, "/SmartDashboard/robotState"), NT_STRING, "string"
    );
    gameMessageSub = nt::Subscribe(nt::GetTopic(
        inst, "/FMSInfo/GameSpecificMessage"), NT_STRING, "string"
    );
    isRedSub = nt::Subscribe(nt::GetTopic(
        inst, "/FMSInfo/IsRedAlliance"), NT_BOOLEAN, "boolean"
    );
}

std::string ShiftWidget::GetAutoWinner() {
    std::string gabeMessage = nt::GetString(gameMessageSub, "");
    return gabeMessage; //gabe has taken over FRC and is infecting my variable names
}

double ShiftWidget::GetStartTime() {
    nt::TimestampedString robotState = nt::GetAtomicString(robotStateSub, "");
    double timeSinceUpdate = double(nt::Now() - robotState.time)/1000000; //current time - send time (in seconds)
    if (timeSinceUpdate < 0) timeSinceUpdate = 0;
    timeSinceUpdate = (robotState.value != "teleop")? -1: timeSinceUpdate;
    return timeSinceUpdate;
}

void ShiftWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::LosslessImageRendering);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    
    double shiftTime = GetShiftTime();
    double blinkClock = frc::GetTime().value();
    double blinkSpeed = 3;
    bool isBlinkVisible = (fmod(blinkClock*blinkSpeed, 1) > 0.5);
    std::string activeAlliance = GetActiveAlliance();
    std::string currentAlliance = GetCurrentAlliance();

    //timer display
    float pointSize = timerLabel->height() * 0.2;
    if (shiftTime == -1) { //invalid data
        timerLabel->setText(QString::fromStdString(fmt::format(
            "<span style='font-size: {}px;'>—</span> <span style='font-size: {}px;'>.—</span>",
            pointSize, pointSize*0.6
        )));
    } else { //yippie display the timer!!
        int timeBig = shiftTime;
        int timeSmall = fmod(shiftTime, 1)*10;
        timerLabel->setText(QString::fromStdString(fmt::format(
            "<span style='font-size: {}px;'>{}</span> <span style='font-size: {}px;'>.{}</span>",
            pointSize, timeBig, pointSize*0.6, timeSmall
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
    

    int arcWidth = timerLabel->height() * 0.8;
    int arcHeight = arcWidth;

    //centered rect
    QRectF boundingRect(center.x() - arcWidth / 2.0, 
        center.y() - arcHeight / 2.0, 
        arcWidth, arcHeight
    );

    pen.setWidth(40);
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
    
        pen.setWidth(32);
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
    timerLabel->setScaledContents(true);
    refreshTimer.setParent(this);
    refreshTimer.setTimerType(Qt::PreciseTimer);
    connect(&refreshTimer, &QTimer::timeout, this, QOverload<>::of(&QWidget::update));
    refreshTimer.start(1000.0/60.0);
}