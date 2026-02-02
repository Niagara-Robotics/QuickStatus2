#pragma once

#include "frc/kinematics/SwerveModuleState.h"
#include <frc/kinematics/SwerveModulePosition.h>
#include <networktables/StructArrayTopic.h>
#include <QWidget>
#include <QTimer>
#include <QSvgRenderer>

class SwerveWidget : public QWidget {
    Q_OBJECT

public:

    explicit SwerveWidget(QWidget* parent = nullptr);
    ~SwerveWidget() {}
    NT_Inst inst;
    nt::NetworkTableInstance altInst;
    NT_Subscriber baseRotSub;
    nt::StructArraySubscriber<frc::SwerveModulePosition> wheelPosStructSub;
    nt::StructArraySubscriber<frc::SwerveModuleState> wheelVelocityStructSub;
    nt::StructArraySubscriber<frc::SwerveModuleState> wheelPowerStructSub;
    double wheelRots[4];
    double lastBaseRot;
    QSvgRenderer baseRender = QSvgRenderer(QString::fromStdString(":/images/swerve/base"));
    QSvgRenderer wheelRender = QSvgRenderer(QString::fromStdString(":/images/swerve/wheel"));
    QSvgRenderer velocityRender = QSvgRenderer(QString::fromStdString(":/images/swerve/velocity"));
    QSvgRenderer powerRender = QSvgRenderer(QString::fromStdString(":/images/swerve/power"));
private:
    QTimer refreshTimer;
protected:
    void paintEvent(QPaintEvent *event);
};