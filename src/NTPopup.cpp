#include <QLabel>
#include <QLineEdit>
#include <QFormLayout>
#include <QIntValidator>
#include <QSettings>
#include <QPushButton>
#include <ntcore.h>
#include "NTPopup.h"
#include "NetworkTables.h"

void NTPopup::cancelAction() {
    close();
}
void NTPopup::applyAction() {
    NetworkTables::UpdateSettings(addressInput->text(), portInput->text().toUInt());
    close();
}

NTPopup::NTPopup(QWidget* parent):QDialog(parent) {
    setWindowFlag(Qt::WindowType::WindowStaysOnTopHint);
    setWindowModality(Qt::WindowModality::ApplicationModal);
    setFixedSize(300,150);

    QFormLayout* layout = new QFormLayout(this);
    setWindowTitle("Configure NetworkTable Client");

    // std::vector<nt::ConnectionInfo> connections = nt::GetConnections(nt::GetDefaultInstance());
    QSettings settings;
    QString initialAddress = settings.value("ntAddress").toString();
    QString initialPort = settings.value("ntPort").toString();
    
    addressInput = new QLineEdit(initialAddress, this);
    portInput = new QLineEdit(initialPort, this);
    QIntValidator *portValidator = new QIntValidator(0, 99999, this);
    portInput->setValidator(portValidator);

    QLabel* connectionStatus = new QLabel("Connected");
    if (nt::IsConnected(nt::GetDefaultInstance())) {
        connectionStatus->setObjectName("connected");
        connectionStatus->setText("Connected");
    } else {
        connectionStatus->setObjectName("disconnected");
        connectionStatus->setText("Not connected");
    }
    connectionStatus->setStyleSheet("#disconnected{color: #FF3664;} #connected{color: #36e450;}");
    layout->addRow("Connection Status:", connectionStatus);
    layout->addRow("Address:", addressInput);
    layout->addRow("Port:", portInput);

    QPushButton* applyButton = new QPushButton("Apply", this);
    connect(applyButton, &QPushButton::clicked, this, &NTPopup::applyAction);

    QPushButton* cancelButton = new QPushButton("Cancel", this);
    connect(cancelButton, &QPushButton::clicked, this, &NTPopup::cancelAction);

    layout->addRow(cancelButton, applyButton);

    setLayout(layout);

    show();

    // exec();
}