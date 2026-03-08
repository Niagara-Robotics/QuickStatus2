#include "FuelWidget.h"
#include "ntcore_c.h"
#include "ntcore_cpp.h"
#include "ntcore_cpp_types.h"
#include <QPainter>
#include <QtCore/qnamespace.h>
#include <QtCore/qpoint.h>
#include <QtWidgets/qgridlayout.h>
#include <cstddef>
#include <span>
#include <string>
#include <frc/Timer.h>
#include <string_view>

void FuelWidget::check() {
    detector = nt::GetString(detectorSub, "");
    auto targetData = nt::GetDoubleArray(targetDataSub, std::span<double>());
    if (!targetData.empty()) tagCount = targetData[1];
    else tagCount = -1;

    if (!targetData.empty()) {
        textLabel->setText(QString::fromStdString(std::to_string(tagCount)));
    } else {
        textLabel->setText("—");
    }
}

void FuelWidget::paintEvent(QPaintEvent * event) {
    check();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::LosslessImageRendering);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    double blinkClock = frc::GetTime().value();
    double blinkSpeed = 1.7;
    bool isBlinkVisible = (fmod(blinkClock*blinkSpeed, 1) > 0.5);
    
    float pointSize = textLabel->height() * 0.4;
    QPoint center = rect().center();
    textLabel->setFont(QFont("B612 Mono", pointSize, 700));

    double arcSize = textLabel->height() * 0.8;

    QRectF boundingRect(center.x() - arcSize / 2.0, 
        center.y() - arcSize / 2.0, 
        arcSize, arcSize
    );

    QPen pen(QColor("#fff950"), pointSize/3, Qt::SolidLine, Qt::RoundCap);
    if (tagCount == -1) pen.setColor(QColor("#33fff950"));
    painter.setPen(pen);

    painter.drawEllipse(boundingRect);
}

FuelWidget::FuelWidget(QWidget* parent):QWidget(parent) {
    setWindowTitle("Fuel Detection");

    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(textLabel);
    setLayout(layout);

    textLabel->setAlignment(Qt::AlignCenter);
    textLabel->setScaledContents(true);

    
    inst = nt::GetDefaultInstance();
    detectorSub = nt::Subscribe(nt::GetTopic(
        inst, "/limelight-fuel/tdclass"), NT_STRING, "string"
    );
    targetDataSub = nt::Subscribe(nt::GetTopic(
        inst, "/limelight-fuel/t2d"), NT_DOUBLE_ARRAY, "double[]"
    );

    refreshTimer.setParent(this);
    refreshTimer.setTimerType(Qt::PreciseTimer);
    connect(&refreshTimer, &QTimer::timeout, this, QOverload<>::of(&QWidget::update));
    refreshTimer.start(150);
}