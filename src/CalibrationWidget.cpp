#include "CalibrationWidget.h"
#include "Constants.h"
#include <QPainter>
#include "networktables/NetworkTableInstance.h"

void CalibrationWidget::resizeEvent(QResizeEvent* event) {
    widgetSize = fmin(width(), height()) * 0.3;
}

void CalibrationWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::LosslessImageRendering);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    int tagCount = tagCountSub.Get();
    double tagDist = tagDistSub.Get();
    tagDist = round(tagDist*100.0)/100.0;
    bool isGyroCalibrated = gyroCalibratedSub.Get();
    bool manualCalibrated = manualCalibratedSub.Get();
    
    painter.setPen(QPen(QColor("#FFFFFF"), widgetSize*0.1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    painter.setBrush(QColor("#000000"));

    QRect tagRect = QRect(0,0,widgetSize,widgetSize);
    QPoint tagCenter = QPoint(rect().center().x() - widgetSize*0.8, rect().center().y() - widgetSize*0.8);
    tagRect.moveCenter(tagCenter);

    painter.drawRect(tagRect);
    // painter.drawRect(tagRect);
    painter.setFont(QFont("B612", widgetSize*0.6, 900));
    painter.setPen(QColor("#FFFFFF"));

    QTextOption textOption = QTextOption();
    textOption.setAlignment(Qt::AlignCenter);

    painter.drawText(tagRect, QString::number(tagCount), textOption);
    painter.setFont(QFont("B612", widgetSize*0.35, 900));

    textOption.setAlignment(Qt::AlignLeft);
    painter.drawText(QRect(rect().center().x(), tagCenter.y()-widgetSize*0.1, 1000, widgetSize), QString::number(tagDist)+"m");
    painter.setPen(QColor("#66FFFFFF"));
    painter.setFont(QFont("B612", widgetSize*0.2, 900));
    painter.drawText(QRect(rect().center().x(), tagCenter.y()-widgetSize*0.4, 1000, widgetSize), "Avg. Dist");

    QString currentMode;
    if (isGyroCalibrated) {
        currentMode = "Calibrated";
        painter.setPen(Constants::AT_TARGET);
    }
    if (manualCalibrated) {
        currentMode = "Manual";
        painter.setPen(Constants::IN_PROGRESS);
    }
    if (!isGyroCalibrated && !manualCalibrated) {
        currentMode = "Uncalibrated";
        painter.setPen(Constants::OVERRIDE);
    }
    textOption.setAlignment(Qt::AlignCenter);
    painter.setFont(QFont("B612", widgetSize*0.35, 900));

    painter.drawText(QRect(0,rect().center().y(),width(),height()*0.4), currentMode, textOption);
}

CalibrationWidget::CalibrationWidget(QWidget* parent):QWidget(parent) {
    setWindowTitle("Calibration");
    
    nt::NetworkTableInstance inst = nt::NetworkTableInstance::GetDefault();
    tagCountSub = inst.GetIntegerTopic("/SmartDashboard/calibrationTags").Subscribe(-1);
    tagDistSub = inst.GetDoubleTopic("/SmartDashboard/calibrationDist").Subscribe(-1);
    gyroCalibratedSub = inst.GetBooleanTopic("/SmartDashboard/isGyroCalibrated").Subscribe(false);
    manualCalibratedSub = inst.GetBooleanTopic("/SmartDashboard/manualCalibration").Subscribe(false);
}