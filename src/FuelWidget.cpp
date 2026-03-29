#include "FuelWidget.h"
#include "ntcore_c.h"
#include "ntcore_cpp.h"
#include "ntcore_cpp_types.h"
#include <QPainter>
#include <QtCore/qnamespace.h>
#include <QtCore/qpoint.h>
#include <QtGui/qwindowdefs.h>
#include <QtWidgets/qgridlayout.h>
#include <QtWidgets/qwidget.h>
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

void FuelWidget::resizeEvent(QResizeEvent* event) {
    pointSize = fmin(textLabel->width(), textLabel->height()) * 0.4;
    textLabel->setFont(QFont("B612 Mono", pointSize, 700));
    pen.setWidth(pointSize/3);

    QPoint center = rect().center();
    float arcSize = pointSize * 2;
    boundingRect = QRect(center.x() - arcSize / 2.0, 
        center.y() - arcSize / 2.0, 
        arcSize, arcSize
    );
}

void FuelWidget::paintEvent(QPaintEvent* event) {
    check();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::LosslessImageRendering);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    if (tagCount == -1) pen.setColor(QColor("#33fff950"));
    else pen.setColor("#fff950");
    painter.setPen(pen);

    painter.drawEllipse(boundingRect);
}

FuelWidget::FuelWidget(QWidget* parent):QWidget(parent) {
    setWindowTitle("Fuel Detection");

    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(textLabel);
    setLayout(layout);

    pen = QPen(QColor("#fff950"), 1, Qt::SolidLine, Qt::RoundCap);

    textLabel->setAlignment(Qt::AlignCenter);
    
    inst = nt::GetDefaultInstance();
    detectorSub = nt::Subscribe(nt::GetTopic(
        inst, "/limelight-fuel/tdclass"), NT_STRING, "string"
    );
    targetDataSub = nt::Subscribe(nt::GetTopic(
        inst, "/limelight-fuel/t2d"), NT_DOUBLE_ARRAY, "double[]"
    );
}