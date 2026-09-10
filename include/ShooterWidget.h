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

private:
    QIcon createIconFromSvg(QSvgRenderer& renderer, const QColor& color, QSize size);

protected:
    void paintEvent(QPaintEvent *event);
};