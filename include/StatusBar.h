#pragma once

#include <QStatusBar>
#include <QTimer>
#include <QLabel>
#include <ntcore.h>

class StatusBar : public QStatusBar {
    Q_OBJECT

public:

    explicit StatusBar(QWidget* parent = nullptr);
    ~StatusBar() {}

    void updateStatus();
    void updateLatency();
    
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
};