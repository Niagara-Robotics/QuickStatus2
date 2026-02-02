#include <QFormLayout>
#include <QPushButton>

#include "StatusBar.h"
#include "NTPopup.h"
#include "NTManager.h"

void NTPopup::cancelAction() {
    close();
}
void NTPopup::typeChange() {
    auto type = addressTypeChooser->currentText();
    settings.setValue("ntType", type);
    typeUpdate();
}
void NTPopup::typeUpdate() {
    auto type = addressTypeChooser->currentText();
    if (type == "Address") {
        addressInput->setText(settings.value("ntAddress").toString());
        addressInput->setValidator(NULL);
    }
    if (type == "Team Number") {
        addressInput->setText(settings.value("ntTeam").toString());
        addressInput->setValidator(new QIntValidator(0, 99999, this));
    }
}
void NTPopup::resetAction() {
    settings.remove("ntAddress");
    settings.remove("ntTeam");
    settings.remove("ntPort");
    settings.remove("ntType");
    NTManager::checkSettings();
    addressInput->setText(settings.value("ntAddress").toString());
    addressInput->setText(settings.value("ntAddress").toString());
    portInput->setText(settings.value("ntPort").toString());
    addressTypeChooser->setCurrentText(settings.value("ntType").toString());
}
void NTPopup::applyAction() {
    NTManager::UpdateSettings(addressInput->text(), portInput->text().toUInt());
    reportTo->updateStatus();
    close();
}

NTPopup::NTPopup(QWidget* parent, StatusBar* statusBar):QDialog(parent) {
    reportTo = statusBar;
    setWindowFlag(Qt::WindowType::WindowStaysOnTopHint);
    setWindowModality(Qt::WindowModality::ApplicationModal);
    // setFixedSize(300,150);
    setFixedSize(300,160);

    QFormLayout* layout = new QFormLayout(this);
    setWindowTitle("Configure NetworkTable Client");

    // std::vector<nt::ConnectionInfo> connections = nt::GetConnections(nt::GetDefaultInstance());
    QString initialAddress = settings.value("ntAddress").toString();
    QString initalTeam = settings.value("ntTeam").toString();
    QString initialPort = settings.value("ntPort").toString();
    QString initialType = settings.value("ntType").toString();
    
    addressInput = new QLineEdit(initialAddress, this);
    addressInput->setMaxLength(15);
    portInput = new QLineEdit(initialPort, this);
    QIntValidator *portValidator = new QIntValidator(0, 99999, this);
    portInput->setValidator(portValidator);

    addressTypeChooser = new QComboBox(this);
    addressTypeChooser->addItem("Address");
    addressTypeChooser->addItem("Team Number");
    addressTypeChooser->setCurrentText(initialType);
    connect(addressTypeChooser, &QComboBox::currentIndexChanged, this, &NTPopup::typeChange);

    typeUpdate();

    layout->addRow(addressTypeChooser, addressInput);
    layout->addRow("Port:", portInput);

    QPushButton* applyButton = new QPushButton("Apply", this);
    connect(applyButton, &QPushButton::clicked, this, &NTPopup::applyAction);

    QPushButton* cancelButton = new QPushButton("Cancel", this);
    connect(cancelButton, &QPushButton::clicked, this, &NTPopup::cancelAction);

    QPushButton* resetButton = new QPushButton("Reset", this);
    connect(resetButton, &QPushButton::clicked, this, &NTPopup::resetAction);
    layout->addWidget(resetButton);
    layout->addRow(cancelButton, applyButton);
    
    setLayout(layout);

    show();

    // exec();
}