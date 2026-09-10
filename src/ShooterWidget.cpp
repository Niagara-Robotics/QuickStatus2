#include "ShooterWidget.h"
#include "Constants.h"
#include <QPainter>
#include <QtCore/qnamespace.h>
#include <QtGui/qcolor.h>
#include <QtGui/qtransform.h>
#include <QtSvg/qsvgrenderer.h>
#include <QtWidgets/qlayoutitem.h>

QIcon ShooterWidget::createIconFromSvg(QSvgRenderer& renderer, const QColor& color, QSize size) {
    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent); // Start with a transparent canvas

    QPainter painter(&pixmap);
    renderer.render(&painter); // Render the SVG (destination)

    // Apply the new color as the source, using the SVG's alpha channel
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn); 
    painter.fillRect(pixmap.rect(), QBrush(color)); // Fill with the desired color

    painter.end();
    return QIcon(pixmap);
}

void ShooterWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::LosslessImageRendering);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    
    int wideShooterStatus = nt::GetInteger(wideShooterStatusSub, -1);
    int wideFeederStatus = nt::GetInteger(wideFeederStatusSub, -1);
    
    double targetRPS = nt::GetDouble(targetRPSSub, -1);
    double wideShooterRPS = nt::GetDouble(wideShooterRPSSub, -1);
    double manualShooterRPS = nt::GetDouble(manualShooterRPSSub, -1);

    double wideFeederRPS = nt::GetDouble(wideFeederRPSSub, -1);

    bool driverAssistedMode = nt::GetBoolean(driverAssistedModeSub, true);

    totalWideFeederRPS += wideFeederRPS;
    totalWideShooterRPS += wideShooterRPS;

    // draw RPS text

    double textSize = fmin(width(), height())*0.21;
    QRect rpsRect = rect();
    rpsRect.adjust(0, -textSize*2.5, 0, 0);
    
    QString rpsText;
    if (driverAssistedMode) rpsText = ((targetRPS != -1)? QString::number(round(targetRPS)): "—")+" RPS";
    else rpsText = ((manualShooterRPS != -1)? QString::number(round(manualShooterRPS)): "—")+" RPS";

    painter.setPen(QPen("#FFFFFF"));
    if (!driverAssistedMode) painter.setOpacity(1);
    else painter.setOpacity(0.5);
    painter.setFont(QFont("B612", textSize, 900));
    painter.drawText(rpsRect, Qt::AlignCenter, rpsText);

    QRect targetRect = QFontMetrics(painter.font()).boundingRect("40 RPS");
    targetRect.moveCenter(rpsRect.center());
    targetRect.adjust(0, -textSize*0.3, 0, 0);

    painter.setFont(QFont("B612", textSize*0.32, 900));
    painter.drawText(targetRect, Qt::AlignTop, "Target");

    painter.setOpacity(1);

    double shooterDist = textSize*1;
    double feederDist = textSize*1.1;
    QRectF shooterRect = rect();
    double shooterSizeVal = textSize;
    QSizeF shooterSize = QSize(shooterSizeVal,shooterSizeVal);
    shooterRect.setSize(shooterSize);
    shooterRect.moveCenter(rect().center());

    QRectF outlineRect = shooterRect;
    QSizeF outlineSize = shooterSize*2.5;
    outlineRect.setSize(outlineSize);
    outlineRect.adjust(-shooterSizeVal*0.5,0,-shooterSizeVal*0.5,0);

    painter.setFont(QFont("B612 Mono", textSize*0.4, 100));

    // ## draw shooter ##

    createIconFromSvg(outline, "#FFFFFF", outlineSize.toSize()*2).paint(&painter, outlineRect.toRect());
    
    QColor shooterColour = getStatusColour(wideShooterStatus);
    shooterColour.setAlpha((wideShooterStatus == -1)? 150: 255);
    // render shooter

    if (wideShooterRPS != -1) {
        painter.translate(shooterRect.center());
        painter.rotate(-totalWideShooterRPS*0.1);
        painter.translate(-shooterRect.center());
    }

    createIconFromSvg(wheel_10, shooterColour, shooterSize.toSize()*2).paint(&painter, shooterRect.toRect());
    painter.resetTransform();
    
    painter.drawText(shooterRect, Qt::AlignCenter, (wideShooterRPS == -1)? "—": QString::number(round(wideShooterRPS)));
    // render feeder

    QColor feederColour = getStatusColour(wideFeederStatus);
    feederColour.setAlpha((wideFeederStatus == -1)? 150: 255);
    
    QRectF feederRect = shooterRect.adjusted(0,feederDist,0,feederDist);
    QSizeF feederSize = shooterSize*0.7;
    QPointF feederCenter = feederRect.center();
    feederRect.setSize(feederSize);
    feederRect.moveCenter(feederCenter);

    if (wideFeederRPS != -1) {
        painter.translate(feederRect.center());
        painter.rotate(-totalWideFeederRPS*0.1);
        painter.translate(-feederRect.center());
    }

    createIconFromSvg(wheel_8, feederColour, shooterSize.toSize()*2).paint(&painter, feederRect.toRect());
    painter.resetTransform();

    painter.setFont(QFont("B612 Mono", textSize*0.3, 100));
    painter.drawText(feederRect, Qt::AlignCenter, (wideFeederRPS == -1)? "—": QString::number(round(wideFeederRPS)));
}

ShooterWidget::ShooterWidget(QWidget* parent):QWidget(parent) {
    setWindowTitle("Shooter");

    inst = nt::GetDefaultInstance();

    wideShooterStatusSub = nt::Subscribe(nt::GetTopic(
        inst, "/SmartDashboard/Shooters/leftShooterStatus"), NT_INTEGER, "int"
    );

    wideFeederStatusSub = nt::Subscribe(nt::GetTopic(
        inst, "/SmartDashboard/leftFeederStatus"), NT_INTEGER, "int"
    );

    wideShooterRPSSub = nt::Subscribe(nt::GetTopic(
        inst, "/SmartDashboard/Shooters/Left RPS"), NT_DOUBLE, "double"
    );

    wideFeederRPSSub = nt::Subscribe(nt::GetTopic(
        inst, "/SmartDashboard/leftFeederCurrentSpeed"), NT_DOUBLE, "double"
    );

    targetRPSSub = nt::Subscribe(nt::GetTopic(
        inst, "/SmartDashboard/Shooters/Target RPS"), NT_DOUBLE, "double"
    );
    manualShooterRPSSub = nt::Subscribe(nt::GetTopic(
        inst, "/SmartDashboard/manualShooterRPS"), NT_DOUBLE, "double"
    );
    driverAssistedModeSub = nt::Subscribe(nt::GetTopic(
        inst, "/SmartDashboard/driverAssistedMode"), NT_BOOLEAN, "boolean"
    );
}