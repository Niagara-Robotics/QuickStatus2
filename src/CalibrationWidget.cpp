#include "CalibrationWidget.h"
#include "Constants.h"
#include "ntcore_cpp_types.h"
#include <QtCore/qnamespace.h>
#include <QtGui/qcolor.h>
#include <QtGui/qpainter.h>
#include <QtGui/qtextoption.h>
#include <QtGui/qwindowdefs.h>

void CalibrationWidget::resizeEvent(QResizeEvent* event) {
    widgetSize = fmin(width(), height()) * 0.3;
}

void CalibrationWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::LosslessImageRendering);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    int tagCount = nt::GetInteger(tagCountSub, -1);
    double tagDist = nt::GetInteger(tagDistSub, -1);
    tagDist = round(tagDist*100)/100;
    bool isGyroCalibrated = nt::GetBoolean(gyroCalibratedSub, false);
    bool manualCalibrated = nt::GetBoolean(manualCalibratedSub, false);
    
    painter.setPen(QPen(QColor("#FFFFFF"), widgetSize*0.1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    painter.setBrush(QColor("#000000"));

    QRect tagRect = QRect(0,0,widgetSize,widgetSize);
    QPoint tagCenter = QPoint(rect().center().x() - widgetSize*0.8, rect().center().y() - widgetSize*0.8);
    tagRect.moveCenter(tagCenter);

    painter.drawRect(tagRect);
    // painter.drawRect(tagRect);
    painter.setFont(QFont("B612", widgetSize*0.75, 900));
    painter.setPen(QColor("#FFFFFF"));

    QTextOption textOption = QTextOption();
    textOption.setAlignment(Qt::AlignCenter);

    painter.drawText(tagRect, QString::number(tagCount), textOption);
    painter.setFont(QFont("B612", widgetSize*0.5, 900));

    textOption.setAlignment(Qt::AlignLeft);
    painter.drawText(QRect(rect().center().x(), tagCenter.y()-widgetSize*0.1, 1000, widgetSize), QString::number(tagDist)+"m");
    painter.setPen(QColor("#66FFFFFF"));
    painter.setFont(QFont("B612", widgetSize*0.3, 900));
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
    painter.setFont(QFont("B612", widgetSize*0.5, 900));

    painter.drawText(QRect(0,rect().center().y(),width(),height()*0.4), currentMode, textOption);
}

CalibrationWidget::CalibrationWidget(QWidget* parent):QWidget(parent) {
    setWindowTitle("Calibration");
    
    inst = nt::GetDefaultInstance();
    tagCountSub = nt::Subscribe(nt::GetTopic(
        inst, "/SmartDashboard/calibrationTags"), NT_INTEGER, "int"
    );
    tagDistSub = nt::Subscribe(nt::GetTopic(
        inst, "/SmartDashboard/calibrationDist"), NT_DOUBLE, "double"
    );
    gyroCalibratedSub = nt::Subscribe(nt::GetTopic(
        inst, "/SmartDashboard/isGyroCalibrated"), NT_BOOLEAN, "boolean"
    );
    manualCalibratedSub = nt::Subscribe(nt::GetTopic(
        inst, "/SmartDashboard/manualCalibratoin"), NT_BOOLEAN, "boolean"
    );
}