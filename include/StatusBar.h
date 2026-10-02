#pragma once

#include <QStatusBar>
#include <QTimer>
#include <QLabel>
#include <QPainter>
#include <QStyleOption>
#include <QPushButton>
#include <ntcore.h>

class RoundedTooltip : public QLabel {
public:
    using QLabel::QLabel;

protected:
    void paintEvent(QPaintEvent* event) override {
        // This boilerplate code forces the custom QSS background to render 
        // properly on top of a translucent window flag.
        QPainter painter(this);
        QStyleOption opt;
        opt.initFrom(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
        
        // Let the default QLabel draw its text cleanly over our painted background
        QLabel::paintEvent(event);
    }
};

class StatusBar : public QStatusBar {
    Q_OBJECT

public:

    explicit StatusBar(QWidget* parent = nullptr);
    ~StatusBar() {}

    void updateStatus();
    void updateLatency();

    QPushButton* shooterButton;
    QPushButton* swerveButton;
    QPushButton* shiftButton;
    QPushButton* visionButton;
    QPushButton* autoButton;
    QPushButton* spacerButton;
    QPushButton* controlModeButton;
    QList<QPushButton*> buttonList;

    QPushButton* createToggleButton(QString name, QString displayName);
    QLabel* tooltip;
    
private:
    QLabel* connectionStatus = new QLabel();
    QLabel* latencyStatus = new QLabel();
    std::string connectionColour = "#FF3664";
    std::string connectionText = "Disconnected";
    std::string connectionAddress = "localhost";
    double connectionLatency = 0;
    bool connectionState = false;
    void ConnectionListenerCallback(nt::Event event);
    void openPopup();

    QTimer refreshTimer;

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
};