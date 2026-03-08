#include <QSettings>
#include <QPushButton>

#include "networktables/NetworkTableInstance.h"

#include "StatusBar.h"
#include "NTPopup.h"

void StatusBar::ConnectionListenerCallback(nt::Event event) {
    if (event.Is(nt::EventFlags::kConnected)) {
        connectionState = true;
    } else if (event.Is(nt::EventFlags::kDisconnected)) {
        connectionState = false;
    }
    updateStatus();
}

void StatusBar::updateStatus() {
    if (connectionState) {
        connectionText = "Connected";
        connectionColour = "#36e450";
    } else {
        connectionText = "Disconnected";
        connectionColour = "#FF3664";
    }
    auto connectionType = QSettings().value("ntType").toString();
    if (connectionType == "Address") connectionAddress = QSettings().value("ntAddress").toString().toStdString();
    if (connectionType == "Team Number") connectionAddress = QSettings().value("ntTeam").toString().toStdString();
    connectionStatus->setText(QString::fromStdString(fmt::format(
        "NetworkTables: <span style='color: {};'>{}</span> <span style='color: #99FFFFFF;'>({})</span>",
        connectionColour, connectionText, connectionAddress
    )));

    // updateLatency();
}

void StatusBar::openPopup() {
    NTPopup* popup = new NTPopup(this, this);
}

// void StatusBar::updateLatency() {
//     auto timeOffset = nt::GetServerTimeOffset(nt::GetDefaultInstance());
//     std::string latencyColour;
//     if (timeOffset.has_value()) {
//         latencyColour = "#FFFFFF";
//         connectionLatency = 0;
//     } else {
//         latencyColour = "#99FFFFFF";
//         connectionLatency = 0;
//     }
//     uint latencyBig = connectionLatency;
//     uint latencySmall = fmod(connectionLatency, 1)*10;
//     latencyStatus->setText(QString::fromStdString(fmt::format(
//         "<span style='color: {};'>{}.{}ms</span>",
//         "#FFFFFF", latencyBig, latencySmall
//     )));
// }

StatusBar::StatusBar(QWidget* parent):QStatusBar(parent) {
    // NTPopup* popup = new NTPopup(this, this);
    setObjectName("statusBar");
    setFixedHeight(40);
    setContentsMargins(5,10,5,0);
    QFont b612("B612", 16);
    connectionStatus->setFont(b612);
    
    updateStatus();
    
    QPushButton* editButton = new QPushButton();
    editButton->setFlat(true);
    editButton->setFixedSize(16,16);
    editButton->setIcon(QIcon(":/images/auto/edit"));
    editButton->setObjectName("genericButton");
    connect(editButton, &QPushButton::clicked, this, &StatusBar::openPopup);

    // QLabel* latencyHeader = new QLabel("Latency:");
    // latencyHeader->setFont(b612);
    // latencyStatus->setFont(QFont("B612 Mono", 16));

    addWidget(editButton);
    addWidget(connectionStatus);
    // addPermanentWidget(latencyHeader);
    // addPermanentWidget(latencyStatus);

    auto inst = nt::NetworkTableInstance::GetDefault();
    NT_Listener listener = inst.AddConnectionListener(
        true,
        [this](const nt::Event& event) {
            this->StatusBar::ConnectionListenerCallback(event);
        }
    );

    // refreshTimer.setParent(this);
    // refreshTimer.setTimerType(Qt::PreciseTimer);
    // connect(&refreshTimer, &QTimer::timeout, this, &StatusBar::updateLatency);
    // refreshTimer.start(500);

    // nt::GetServerTimeOffset(nt::GetDefaultInstance());
}