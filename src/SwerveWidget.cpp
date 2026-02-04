#include "SwerveWidget.h"
#include <QSvgRenderer>
#include <QPainter>
#include <ntcore.h>

#include <frc/kinematics/SwerveModulePosition.h>
#include <frc/kinematics/SwerveModuleState.h>
#include <networktables/StructArrayTopic.h>

float NormalizeAngle(float angle) {
    angle = std::fmod(angle, 360.0f);
    if (angle < 0) angle += 360.0f;
    return angle;
}

float LerpAngleDegrees(float a, float b, float t) {
    a = NormalizeAngle(a);
    b = NormalizeAngle(b);
    
    float diff = b - a;
    
    // Handle wrap-around
    if (std::abs(diff) > 180.0f) {
        if (diff > 0) diff -= 360.0f;
        else diff += 360.0f;
    }
    
    return NormalizeAngle(a + diff * t);
}

void flipCanvas(QPainter &painter, QPointF center) {
    painter.translate(center);
    painter.scale(1, -1); //flip vertically
    painter.translate(-center);
}

void SwerveWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    auto rotArray = nt::GetDoubleArray(baseRotSub, std::span<double>());
    double currentBaseRot = (!rotArray.empty())? rotArray[2]: 0;
    currentBaseRot = LerpAngleDegrees(lastBaseRot, currentBaseRot, 0.4);// maybe makes it look less like garbage
    lastBaseRot = currentBaseRot;

    auto wheelPosStruct = wheelPosStructSub.Get();
    auto wheelVelocityStruct = wheelVelocityStructSub.Get();
    auto wheelPowerStruct = wheelPowerStructSub.Get();

    int baseSize = height()*0.5;
    QRectF baseRect = rect();
    baseRect.setSize(QSize(baseSize,baseSize));
    baseRect.moveCenter(rect().center());
    
    int wheelSize = baseSize*0.5;
    QRectF wheelRect = rect();
    wheelRect.setSize(QSize(wheelSize,wheelSize));

    if (baseRender.isValid()) {
        painter.translate(baseRect.center());
        painter.rotate(-currentBaseRot+180);
        painter.translate(-baseRect.center());

        if (rotArray.empty()) painter.setOpacity(0.5);
        else painter.setOpacity(1);
        baseRender.render(&painter, baseRect);

        if (wheelPosStruct.empty()) painter.setOpacity(0.5);
        else painter.setOpacity(1);

        for (int i=0; i < 4; i++) { //draw 4 wheels
            painter.save();

            wheelRect.moveCenter(QPointF(
                baseRect.topLeft().x() + baseSize * (i % 2), // every other wheel, switch l/r
                baseRect.topLeft().y() + baseSize * (i/2 % 2) // every 2 wheels, switch u/d
            ));

            painter.translate(wheelRect.center());

            double wheelRot = (!wheelPosStruct.empty())? wheelPosStruct[i].angle.Degrees().value(): 0;
            wheelRot = LerpAngleDegrees(wheelRots[i], wheelRot, 0.4); // maybe makes it look less like garbage
            wheelRots[i] = wheelRot;
            painter.rotate(-wheelRot);

            painter.translate(-wheelRect.center());
            //draw velocity/power
            //velocity and target are both sent in m/s, so convert to a % (robot max is 5.41 here)
            QRectF velocityRect = wheelRect;
            double velocity = (!wheelVelocityStruct.empty())? wheelVelocityStruct[i].speed.value() / 5.41: 0;
            if (velocity != 0) {
                if (velocity < 0) flipCanvas(painter, wheelRect.center());
                velocityRect.setSize(QSizeF(velocityRect.size().width(), velocityRect.size().height()*abs(velocity)));
                velocityRect.moveCenter(wheelRect.center());
                velocityRender.render(&painter, velocityRect);
                if (velocity < 0) flipCanvas(painter, wheelRect.center());
            }

            QRectF powerRect = wheelRect;
            double power = (!wheelPowerStruct.empty())? wheelPowerStruct[i].speed.value() / 5.41: 0;
            if (power != 0) {
                if (power < 0) flipCanvas(painter, wheelRect.center());
                powerRect.setSize(QSizeF(powerRect.size().width(), powerRect.size().height()*abs(power)));
                powerRect.moveCenter(wheelRect.center());
                powerRender.render(&painter, powerRect);
                if (power < 0) flipCanvas(painter, wheelRect.center());
            }
            // if (velocity != 0 || power != 0) qDebug()<<velocity<<power;

            //draw wheel
            wheelRender.render(&painter, wheelRect);
            painter.restore();
        }
        painter.resetTransform();
    }
}

SwerveWidget::SwerveWidget(QWidget* parent):QWidget(parent) {
    setWindowTitle("Swerve");

    inst = nt::GetDefaultInstance();
    
    baseRotSub = nt::Subscribe(nt::GetTopic(
        inst, "/Pose/robotPose"), NT_DOUBLE_ARRAY, "double[]"
    );
    altInst = nt::NetworkTableInstance::GetDefault();
    wheelPosStructSub = altInst.GetStructArrayTopic<frc::SwerveModulePosition>(
        "/DriveState/ModulePositions"
    ).Subscribe({});
    wheelVelocityStructSub = altInst.GetStructArrayTopic<frc::SwerveModuleState>(
        "/DriveState/ModuleStates"
    ).Subscribe({});
    wheelPowerStructSub = altInst.GetStructArrayTopic<frc::SwerveModuleState>(
        "/DriveState/ModuleTargets"
    ).Subscribe({});

    refreshTimer.setParent(this);
    refreshTimer.setTimerType(Qt::PreciseTimer);
    connect(&refreshTimer, &QTimer::timeout, this, QOverload<>::of(&QWidget::update));
    refreshTimer.start(1000.0/60.0);
}