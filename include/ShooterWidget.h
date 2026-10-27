#pragma once

#include "networktables/BooleanTopic.h"
#include "networktables/DoubleTopic.h"
#include "networktables/IntegerTopic.h"
#include <QWidget>
#include <QSvgRenderer>

class ShooterWidget : public QWidget {
    Q_OBJECT

public:

    explicit ShooterWidget(QWidget* parent = nullptr);
    ~ShooterWidget() {}

    nt::IntegerSubscriber wideShooterStatusSub;
    nt::IntegerSubscriber wideFeederStatusSub;

    nt::DoubleSubscriber targetRPSSub;
    nt::DoubleSubscriber manualShooterRPSSub;
    nt::DoubleSubscriber wideShooterRPSSub;
    nt::DoubleSubscriber wideFeederRPSSub;

    nt::DoubleSubscriber hoodAngleSub;
    nt::DoubleSubscriber hoodTargetSub;
    
    nt::BooleanSubscriber driverAssistedModeSub;

    QSvgRenderer wheel_10 = QSvgRenderer(QString::fromStdString(":/images/shooter/wheel_10"));
    QSvgRenderer wheel_8 = QSvgRenderer(QString::fromStdString(":/images/shooter/wheel_8"));
    QSvgRenderer outline = QSvgRenderer(QString::fromStdString(":/images/shooter/outline"));

    double totalWideShooterRPS;
    double totalWideFeederRPS;

    double textSize;
    QRect rpsRect;

    double feederDist;
    static inline QRectF shooterRect = QRectF(0,0,100,100);
    QSizeF shooterSize;

    QRectF outlineRect;
    QSizeF outlineSize;

private:
    QPixmap createPixmapFromSvg(QSvgRenderer& renderer, const QColor& color, QSize size);
    QPixmap cachedWheel10;
    QPixmap cachedWheel8;
    QPixmap cachedOutline;
    int lastShooterStatus = -2; // track changes to regenerate color
    int lastFeederStatus = -2;
    double outlineOffset;

    QFont rpsFont;
    QFont targetFont;
    QFont shooterFont;
    QFont feederFont;
    QFont hoodFont;
    QRect targetRect; // Cache the bounding box too!

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
};