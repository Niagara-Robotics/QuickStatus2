#pragma once

#include <QWidget>
#include <QTimer>
#include <QSvgRenderer>
#include <QtGui/qpixmap.h>
#include <QtWidgets/qwidget.h>
#include <ntcore.h>

class ShooterWidget : public QWidget {
    Q_OBJECT

public:

    explicit ShooterWidget(QWidget* parent = nullptr);
    ~ShooterWidget() {}
    NT_Inst inst;
    NT_Subscriber wideShooterStatusSub;
    NT_Subscriber wideFeederStatusSub;
    NT_Subscriber wideShooterRPSSub;
    NT_Subscriber wideFeederRPSSub;
    NT_Subscriber targetRPSSub;
    NT_Subscriber manualShooterRPSSub;
    NT_Subscriber driverAssistedModeSub;
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

    QFont rpsFont;
    QFont targetFont;
    QFont shooterFont;
    QFont feederFont;
    QRect targetRect; // Cache the bounding box too!

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
};