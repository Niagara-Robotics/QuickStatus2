#pragma once

#include <QWidget>
#include <QTimer>
#include <QSvgRenderer>
#include <ntcore.h>

class ShooterWidget : public QWidget {
    Q_OBJECT

public:

    explicit ShooterWidget(QWidget* parent = nullptr);
    ~ShooterWidget() {}
    NT_Inst inst;
    NT_Subscriber leftShooterStatusSub;
    NT_Subscriber rightShooterStatusSub;
    NT_Subscriber leftFeederStatusSub;
    NT_Subscriber rightFeederStatusSub;
    NT_Subscriber leftShooterRPSSub;
    NT_Subscriber rightShooterRPSSub;
    NT_Subscriber leftFeederRPSSub;
    NT_Subscriber rightFeederRPSSub;
    NT_Subscriber targetRPSSub;
    NT_Subscriber manualShooterRPSSub;
    NT_Subscriber driverAssistedModeSub;
    QSvgRenderer wheel_10 = QSvgRenderer(QString::fromStdString(":/images/shooter/wheel_10"));
    QSvgRenderer wheel_8 = QSvgRenderer(QString::fromStdString(":/images/shooter/wheel_8"));
    QSvgRenderer outline = QSvgRenderer(QString::fromStdString(":/images/shooter/outline"));

    double totalLeftShooterRPS;
    double totalLeftFeederRPS;
    double totalRightShooterRPS;
    double totalRightFeederRPS;

private:
    QIcon createIconFromSvg(QSvgRenderer& renderer, const QColor& color, QSize size);

protected:
    void paintEvent(QPaintEvent *event);
};