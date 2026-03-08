#pragma once

#include "frc/kinematics/SwerveModuleState.h"
#include <frc/kinematics/SwerveModulePosition.h>
#include <networktables/StructArrayTopic.h>
#include <QWidget>
#include <QTimer>
#include <QSvgRenderer>

class ShooterWidget : public QWidget {
    Q_OBJECT

public:

    explicit ShooterWidget(QWidget* parent = nullptr);
    ~ShooterWidget() {}
    NT_Inst inst;
    nt::NetworkTableInstance altInst;
    NT_Subscriber leftShooterStatusSub;
    NT_Subscriber rightShooterStatusSub;
    NT_Subscriber leftFeederStatusSub;
    NT_Subscriber rightFeederStatusSub;
    NT_Subscriber leftShooterRPSSub;
    NT_Subscriber rightShooterRPSSub;
    NT_Subscriber targetRPSSub;
    NT_Subscriber manualShooterRPSSub;
    NT_Subscriber driverAssistedModeSub;
    QSvgRenderer wheel_10 = QSvgRenderer(QString::fromStdString(":/images/shooter/wheel_10"));
    QSvgRenderer wheel_8 = QSvgRenderer(QString::fromStdString(":/images/shooter/wheel_8"));
    QSvgRenderer outline = QSvgRenderer(QString::fromStdString(":/images/shooter/outline"));

private:
    QTimer refreshTimer;
    QIcon createIconFromSvg(QSvgRenderer& renderer, const QColor& color, QSize size);

protected:
    void paintEvent(QPaintEvent *event);
};