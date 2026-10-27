#pragma once

#include "networktables/DoubleArrayTopic.h"
#include "networktables/IntegerTopic.h"
#include "networktables/StructArrayTopic.h"
#include "frc/kinematics/SwerveModuleState.h"
#include "frc/kinematics/SwerveModulePosition.h"
#include <QWidget>
#include <QSvgRenderer>

class SwerveWidget : public QWidget {
    Q_OBJECT

public:

    explicit SwerveWidget(QWidget* parent = nullptr);
    ~SwerveWidget() {}
    nt::DoubleArraySubscriber baseRotSub;
    nt::StructArraySubscriber<frc::SwerveModulePosition> wheelPosStructSub;
    nt::StructArraySubscriber<frc::SwerveModuleState> wheelVelocityStructSub;
    nt::StructArraySubscriber<frc::SwerveModuleState> wheelPowerStructSub;
    nt::IntegerSubscriber alignStatusSub;
    double wheelRots[4];
    double lastBaseRot;
    QSvgRenderer baseRender = QSvgRenderer(QString::fromStdString(":/images/swerve/base"));
    QSvgRenderer wheelRender = QSvgRenderer(QString::fromStdString(":/images/swerve/wheel"));
    QSvgRenderer velocityRender = QSvgRenderer(QString::fromStdString(":/images/swerve/velocity"));
    QSvgRenderer powerRender = QSvgRenderer(QString::fromStdString(":/images/swerve/power"));
private:
    QPixmap createPixmapFromSvg(QSvgRenderer& renderer, QSize size);
    QPixmap cachedWheel;
    QPixmap cachedBase;
protected:
    void paintEvent(QPaintEvent *event);
};