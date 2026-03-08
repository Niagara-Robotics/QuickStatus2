#include "ShooterWidget.h"
#include "Constants.h"
#include "ntcore_c.h"
#include "ntcore_cpp_types.h"
#include <QPainter>
#include <QtCore/qnamespace.h>
#include <QtGui/qcolor.h>
#include <QtSvg/qsvgrenderer.h>
#include <QtWidgets/qlayoutitem.h>
#include <string>

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
    
    int leftShooterStatus = nt::GetInteger(leftShooterStatusSub, -1);
    int rightShooterStatus = nt::GetInteger(rightShooterStatusSub, -1);
    int leftFeederStatus = nt::GetInteger(leftFeederStatusSub, -1);
    int rightFeederStatus = nt::GetInteger(rightFeederStatusSub, -1);
    
    double targetRPS = nt::GetDouble(targetRPSSub, -1);
    double leftShooterRPS = nt::GetDouble(leftShooterRPSSub, 40);
    double rightShooterRPS = nt::GetDouble(rightShooterRPSSub, -1);
    double manualShooterRPS = nt::GetDouble(manualShooterRPSSub, -1);

    bool driverAssistedMode = nt::GetBoolean(driverAssistedModeSub, true);

    // draw RPS text

    double textSize = rect().height()*0.2;
    QRect rpsRect = rect();
    rpsRect.adjust(0, -textSize*2.5, 0, 0);
    
    QString rpsText;
    if (driverAssistedMode) rpsText = ((targetRPS != -1)? QString::number(targetRPS): "—")+" RPS";
    else rpsText = ((manualShooterRPS != -1)? QString::number(manualShooterRPS): "—")+" RPS";

    painter.setPen(QPen("#FFFFFF"));
    if (!driverAssistedMode) painter.setOpacity(1);
    else painter.setOpacity(0.5);
    painter.setFont(QFont("B612", textSize, 900));
    painter.drawText(rpsRect, Qt::AlignCenter, rpsText);

    QRect targetRect = QFontMetrics(painter.font()).boundingRect(rpsText);
    targetRect.moveCenter(rpsRect.center());
    targetRect.adjust(0, -textSize*0.3, 0, 0);

    painter.setFont(QFont("B612", textSize*0.32, 900));
    painter.drawText(targetRect, Qt::AlignTop, "Target");

    painter.setOpacity(1);

    double shooterDist = textSize*1;
    double feederDist = textSize*1.1;
    QRect shooterRect = rect();
    double shooterSizeVal = textSize;
    QSize shooterSize = QSize(shooterSizeVal,shooterSizeVal);
    shooterRect.setSize(shooterSize);
    shooterRect.moveCenter(rect().center());
    shooterRect.adjust(-shooterDist*1.2, 0,-shooterDist*1.2,0);

    QRect outlineRect = shooterRect;
    QSize outlineSize = shooterSize*2.5;
    outlineRect.setSize(outlineSize);
    outlineRect.adjust(-shooterSizeVal*0.5,0,-shooterSizeVal*0.5,0);

    painter.setFont(QFont("B612 Mono", textSize*0.4, 100));

    for (int i=0; i<2; i++) {
        int shooterStatus = ((i==0)? leftShooterStatus: rightShooterStatus);
        int shooterRPS = ((i==0)? leftShooterRPS: rightShooterRPS);
        QColor shooterColour = getStatusColour(shooterStatus);
        shooterColour.setAlpha((shooterStatus == -1)? 150: 255);
        createIconFromSvg(outline, shooterColour, outlineSize*2).paint(&painter, outlineRect);

        // render shooter
        createIconFromSvg(wheel_10, shooterColour, shooterSize*2).paint(&painter, shooterRect);
        painter.drawText(shooterRect, Qt::AlignCenter, (shooterRPS == -1)? "—": QString::number(shooterRPS));
        
        // render feeder
        int feederStatus = ((i==0)? leftFeederStatus: rightFeederStatus);
        QColor feederColour = getStatusColour(shooterStatus);
        feederColour.setAlpha((feederStatus == -1)? 150: 255);

        QRect feederRect = shooterRect.adjusted(0,feederDist,0,feederDist);
        QSize feederSize = shooterSize*0.7;
        QPoint feederCenter = feederRect.center();
        feederRect.setSize(feederSize);
        feederRect.moveCenter(feederCenter);
        
        QColor().setAlpha((((i==0)? leftFeederStatus: rightFeederStatus) == -1)? 150: 255);
        createIconFromSvg(wheel_8, feederColour, shooterSize*2).paint(&painter, feederRect);
        
        shooterRect.adjust(shooterDist*2,0,shooterDist*2,0);
        outlineRect.adjust(shooterDist*2,0,shooterDist*2,0);
    }
}

ShooterWidget::ShooterWidget(QWidget* parent):QWidget(parent) {
    setWindowTitle("Shooter");

    inst = nt::GetDefaultInstance();

    leftShooterStatusSub = nt::Subscribe(nt::GetTopic(
        inst, "/SmartDashboard/Shooters/leftShooterStatus"), NT_INTEGER, "int"
    );
    rightShooterStatusSub = nt::Subscribe(nt::GetTopic(
        inst, "/SmartDashboard/Shooters/rightShooterStatus"), NT_INTEGER, "int"
    );

    leftFeederStatusSub = nt::Subscribe(nt::GetTopic(
        inst, "/SmartDashboard/leftFeederStatus"), NT_INTEGER, "int"
    );
    rightFeederStatusSub = nt::Subscribe(nt::GetTopic(
        inst, "/SmartDashboard/rightFeederStatus"), NT_INTEGER, "int"
    );

    leftShooterRPSSub = nt::Subscribe(nt::GetTopic(
        inst, "/SmartDashboard/Shooters/Left RPS"), NT_DOUBLE, "double"
    );
    rightShooterRPSSub = nt::Subscribe(nt::GetTopic(
        inst, "/SmartDashboard/Shooters/Right RPS"), NT_DOUBLE, "double"
    );
    targetRPSSub = nt::Subscribe(nt::GetTopic(
        inst, "/SmartDashboard/Shooters/Target RPS"), NT_DOUBLE, "double"
    );
    manualShooterRPSSub = nt::Subscribe(nt::GetTopic(
        inst, "/SmartDashboard/manualShooterRPS"), NT_DOUBLE, "double"
    );
    driverAssistedModeSub = nt::Subscribe(nt::GetTopic(
        inst, "/SmartDashboard/driverAssistedMode"), NT_BOOLEAN, "bool"
    );

    refreshTimer.setParent(this);
    refreshTimer.setTimerType(Qt::PreciseTimer);
    connect(&refreshTimer, &QTimer::timeout, this, QOverload<>::of(&QWidget::update));
    refreshTimer.start(1000.0/60.0);
}