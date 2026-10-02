#include "ShooterWidget.h"
#include "Constants.h"
#include <QPainter>
#include <QtCore/qnamespace.h>
#include <QtCore/qsize.h>
#include <QtGui/qcolor.h>
#include <QtGui/qicon.h>
#include <QtGui/qtransform.h>
#include <QtSvg/qsvgrenderer.h>
#include <QtWidgets/qlayoutitem.h>
#include <QtWidgets/qwidget.h>

QPixmap ShooterWidget::createPixmapFromSvg(QSvgRenderer& renderer, const QColor& color, QSize size) {
    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    renderer.render(&painter);

    painter.setCompositionMode(QPainter::CompositionMode_SourceIn); 
    painter.fillRect(pixmap.rect(), QBrush(color)); 

    painter.end();
    return pixmap;
}

void ShooterWidget::resizeEvent(QResizeEvent *event) {
    QRect widgetRect = rect();
    textSize = fmin(width(), height())*0.21;
    rpsRect = widgetRect.adjusted(0, -textSize*2.5, 0, 0);

    feederDist = textSize*1.1;
    shooterRect = widgetRect;
    shooterSize = QSize(textSize,textSize);
    shooterRect.setSize(shooterSize);
    shooterRect.moveCenter(widgetRect.center());

    outlineSize = shooterSize*2.5;
    outlineRect = shooterRect;
    outlineRect.setSize(outlineSize);
    outlineRect.adjust(-textSize*0.5,0,-textSize*0.5,0);

    rpsFont = QFont("B612", textSize, 900);
    targetFont = QFont("B612", textSize * 0.32, 900);
    shooterFont = QFont("B612 Mono", textSize * 0.4, 100);
    feederFont = QFont("B612 Mono", textSize * 0.3, 100);

    // Compute the bounding box once here
    targetRect = QFontMetrics(targetFont).boundingRect("40 RPS");
    targetRect.moveCenter(rpsRect.center());
    targetRect.adjust(0, -textSize * 0.3, 0, 0);
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
    
    QString rpsText;
    if (driverAssistedMode) rpsText = ((targetRPS != -1)? QString::number(round(targetRPS)): "—")+" RPS";
    else rpsText = ((manualShooterRPS != -1)? QString::number(round(manualShooterRPS)): "—")+" RPS";

    painter.setPen(QPen("#FFFFFF"));
    if (!driverAssistedMode) painter.setOpacity(1);
    else painter.setOpacity(0.5);
    painter.setFont(rpsFont);
    painter.drawText(rpsRect, Qt::AlignCenter, rpsText);

    QRect targetRect = QFontMetrics(painter.font()).boundingRect("40 RPS");
    targetRect.moveCenter(rpsRect.center());
    targetRect.adjust(0, -textSize*0.3, 0, 0);

    painter.setFont(targetFont);
    painter.drawText(targetRect, Qt::AlignTop, "Target");
    
    painter.setOpacity(1);
    painter.setFont(shooterFont);

    // ## draw shooter ##

    if (cachedOutline.isNull()) {
        cachedOutline = createPixmapFromSvg(outline, "#FFFFFF", outlineSize.toSize() * 4);
    }
    painter.drawPixmap(outlineRect.toRect(), cachedOutline);
    
    QColor shooterColour = getStatusColour(wideShooterStatus);
    shooterColour.setAlpha((wideShooterStatus == -1)? 150: 255);
    
    // render shooter
    if (cachedWheel10.isNull() || wideShooterStatus != lastShooterStatus) {
        cachedWheel10 = createPixmapFromSvg(wheel_10, shooterColour, shooterSize.toSize() * 4);
        lastShooterStatus = wideShooterStatus;
    }

    if (wideShooterRPS != -1) {
        painter.translate(shooterRect.center());
        painter.rotate(-totalWideShooterRPS*0.1);
        painter.translate(-shooterRect.center());
    }

    painter.drawPixmap(shooterRect.toRect(), cachedWheel10);
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

    if (cachedWheel8.isNull() || wideFeederStatus != lastFeederStatus) {
        cachedWheel8 = createPixmapFromSvg(wheel_8, feederColour, feederSize.toSize() * 4);
        lastFeederStatus = wideFeederStatus;
    }
    
    if (wideFeederRPS != -1) {
        painter.translate(feederRect.center());
        painter.rotate(-totalWideFeederRPS*0.1);
        painter.translate(-feederRect.center());
    }
    
    painter.drawPixmap(feederRect.toRect(), cachedWheel8);

    // createIconFromSvg(wheel_8, feederColour, shooterSize.toSize()*2).paint(&painter, feederRect.toRect());
    painter.resetTransform();

    painter.setFont(feederFont);
    painter.drawText(feederRect, Qt::AlignCenter, (wideFeederRPS == -1)? "—": QString::number(round(wideFeederRPS)));
}

ShooterWidget::ShooterWidget(QWidget* parent):QWidget(parent) {
    setWindowTitle("Shooter");

    inst = nt::GetDefaultInstance();

    wideShooterStatusSub = nt::Subscribe(nt::GetTopic(
        inst, "/SmartDashboard/Shooters/shooterStatus"), NT_INTEGER, "int"
    );

    wideFeederStatusSub = nt::Subscribe(nt::GetTopic(
        inst, "/SmartDashboard/feederStatus"), NT_INTEGER, "int"
    );

    wideShooterRPSSub = nt::Subscribe(nt::GetTopic(
        inst, "/SmartDashboard/Shooters/Shooter RPS"), NT_DOUBLE, "double"
    );

    wideFeederRPSSub = nt::Subscribe(nt::GetTopic(
        inst, "/SmartDashboard/feederCurrentSpeed"), NT_DOUBLE, "double"
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