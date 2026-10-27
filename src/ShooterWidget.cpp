#include "ShooterWidget.h"
#include "Constants.h"
#include <QPainter>
#include "networktables/NetworkTableInstance.h"

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

    outlineOffset = -outlineRect.width()*0.4;

    rpsFont = QFont("B612", textSize, 900);
    targetFont = QFont("B612", textSize * 0.32, 900);
    shooterFont = QFont("B612 Mono", textSize * 0.4, 100);
    feederFont = QFont("B612 Mono", textSize * 0.3, 100);
    hoodFont = QFont("B612", textSize * 0.2, 100);

    targetRect = QFontMetrics(rpsFont).boundingRect("40 RPS");
    targetRect.moveCenter(rpsRect.center());
    targetRect.adjust(0, -textSize * 0.3, 0, 0);

    cachedOutline = createPixmapFromSvg(outline, "#FFFFFF", outlineSize.toSize() * 4);
}

void ShooterWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::LosslessImageRendering);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    int wideShooterStatus = wideShooterStatusSub.Get();
    int wideFeederStatus = wideFeederStatusSub.Get();
    
    double targetRPS = targetRPSSub.Get();
    double wideShooterRPS = wideShooterRPSSub.Get();
    double manualShooterRPS = manualShooterRPSSub.Get();

    double hoodAngle = hoodAngleSub.Get();
    double targetAngle = hoodTargetSub.Get();

    double wideFeederRPS = wideFeederRPSSub.Get();

    bool driverAssistedMode = driverAssistedModeSub.Get();

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

    painter.setFont(targetFont);
    painter.drawText(targetRect, Qt::AlignTop, "Target");
    
    painter.setOpacity(1);
    painter.setFont(shooterFont);

    // ## draw shooter ##
    painter.translate(outlineOffset,0);
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
    painter.translate(outlineOffset,0);
    
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
    painter.translate(outlineOffset,0);

    painter.setFont(feederFont);
    painter.drawText(feederRect, Qt::AlignCenter, (wideFeederRPS == -1)? "—": QString::number(round(wideFeederRPS)));

    // draw hood visual

    painter.translate(textSize*0.6, 0);

    QPen pen = QPen("#676767");
    pen.setWidth(textSize/10);
    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);
    painter.drawLine(outlineRect.topRight(),outlineRect.bottomRight());

    // notches
    painter.setFont(hoodFont);
    QLineF notchLine = QLineF(
        QPointF(outlineRect.topRight().x()-(textSize/10.0), outlineRect.topRight().y()), 
        QPointF(outlineRect.topRight().x(), outlineRect.topRight().y()));
    for (int i=0; i <= 4; i++) {
        QLineF translatedLine = notchLine.translated(0,outlineRect.height()*(i/4.0));
        painter.drawLine(translatedLine);
        QString text = QString::number((i/4.0)*100) + "%";
        int textWidth = painter.fontMetrics().horizontalAdvance(text);
        int textHeight = painter.fontMetrics().height();
        
        painter.drawText(translatedLine.translated(-textWidth-(textSize*0.1), textHeight/4.0).p1(), text);
        
        if (i == 0) painter.drawText(translatedLine.translated((textSize*0.29), textHeight/4.0).p1(), "High");
        if (i == 4) painter.drawText(translatedLine.translated((textSize*0.29), textHeight/4.0).p1(), "Low");
    }
    notchLine.setP2(QPointF(outlineRect.topRight().x()+(textSize/10.0), outlineRect.topRight().y()));
    double hoodAngleWrapped = (hoodAngle > 0.85)
        ? (0.85 - ((hoodAngle-0.85)*(0.85/0.15)))
        : hoodAngle;
    double hoodDiff = abs(hoodAngle - targetAngle);
    hoodDiff = std::min(hoodDiff, 1.0 - hoodDiff);
    if (wideShooterStatus == 0) pen.setColor(Constants::NO_STATUS);
    else pen.setColor((hoodDiff <= 0.02)
        ? Constants::AT_TARGET
        : Constants::IN_PROGRESS);
    painter.setPen(pen);

    if (hoodAngle != -1) painter.drawLine(notchLine.translated(0, outlineRect.height() - outlineRect.height()*(1-hoodAngleWrapped/0.85)));

    // Formula for angles over 0.85: ### (0.85 - ((angle-0.85)*(0.85/0.15))) / 0.85
    // Formula for angles under/equal 0.85: ### angle / 0.85
}

ShooterWidget::ShooterWidget(QWidget* parent):QWidget(parent) {
    setWindowTitle("Shooter");

    nt::NetworkTableInstance inst = nt::NetworkTableInstance::GetDefault();
    std::shared_ptr<nt::NetworkTable> smartDashboard = inst.GetTable("SmartDashboard");
    wideShooterStatusSub = smartDashboard->GetIntegerTopic("Shooters/shooterStatus").Subscribe(-1);
    wideFeederStatusSub = smartDashboard->GetIntegerTopic("Shooters/feederStatus").Subscribe(-1);
    
    targetRPSSub = smartDashboard->GetDoubleTopic("Shooters/Target RPS").Subscribe(-1);
    manualShooterRPSSub = smartDashboard->GetDoubleTopic("manualShooterRPS").Subscribe(-1);
    wideShooterRPSSub = smartDashboard->GetDoubleTopic("Shooters/Shooter RPS").Subscribe(-1);
    wideFeederRPSSub = smartDashboard->GetDoubleTopic("feederCurrentSpeed").Subscribe(-1);

    hoodAngleSub = smartDashboard->GetDoubleTopic("Shooters/Current Angle").Subscribe(-1);
    hoodTargetSub = smartDashboard->GetDoubleTopic("Shooters/Targer Angle").Subscribe(-1);

    driverAssistedModeSub = smartDashboard->GetBooleanTopic("driverAssistedMode").Subscribe(-1);
}