#include <QLabel>
#include <QRadioButton>
#include "AutoWidget.h"
#include "networktables/NetworkTableInstance.h"

void AutoWidget::setupNT() {
    inst = nt::GetDefaultInstance();
    optionsSub = nt::Subscribe(
        nt::GetTopic(inst, "/SmartDashboard/Auto/options"), 
        NT_STRING_ARRAY, "string[]");
    activeSub = nt::Subscribe(
        nt::GetTopic(inst, "/SmartDashboard/Auto/active"), 
        NT_STRING, "string");
    defaultSub = nt::Subscribe(
        nt::GetTopic(inst, "/SmartDashboard/Auto/default"), 
        NT_STRING, "string");
    selectionPub = nt::Publish(
        nt::GetTopic(inst, "/SmartDashboard/Auto/selected"),
        NT_STRING, "string");
}

void AutoWidget::buttonClicked(std::string value) {
    nt::SetString(selectionPub, value);
}

void AutoWidget::removeButtons() {
    for (QAbstractButton* button : buttons.buttons()) {
        buttons.removeButton(button);
        button->deleteLater();
    }
}

void AutoWidget::updateButtons() {
    std::string active = nt::GetString(activeSub, "");
    std::string defaultSelected = nt::GetString(defaultSub, "");
    auto options = nt::GetStringArray(optionsSub, std::span<std::string>());
    removeButtons();
    for (std::string option : options) {
        QRadioButton* button = new QRadioButton(QString::fromStdString(option), this);
        if (active == option) button->setChecked(true);
        if (defaultSelected == option) button->setText(button->text()+" •");
        button->setContentsMargins(10,10,10,10);
        buttons.addButton(button);
        layout->addWidget(button);
        connect(button, &QRadioButton::clicked, this, [this, option]() {
            buttonClicked(option);
        });
    }
    if (options.empty()) {
        noAutos->show();
    } else {
        noAutos->hide();
    }
}

AutoWidget::AutoWidget(QWidget* parent):QWidget(parent) {
    setWindowTitle("Auto Selector");
    layout = new QVBoxLayout(this);
    setupNT();
    buttons.setParent(this);
    noAutos = new QLabel("No autos detected", this);
    noAutos->setAlignment(Qt::AlignCenter);
    noAutos->setFont(QFont("B612", 40, 900));
    layout->addWidget(noAutos);
    noAutos->hide();

    updateButtons();

    auto listenerInst = nt::NetworkTableInstance::GetDefault();
    NT_Listener updateListener = listenerInst.AddListener(
        listenerInst.GetTopic("/SmartDashboard/Auto/options"),
        nt::EventFlags::kValueAll,
        [this](const nt::Event&) {
            QMetaObject::invokeMethod(
                this,
                &AutoWidget::updateButtons,
                Qt::QueuedConnection
            );
        }
    );
    NT_Listener disconnectListener = listenerInst.AddListener(
        listenerInst.GetTopic("/SmartDashboard/Auto/options"),
        nt::EventFlags::kUnpublish,
        [this](const nt::Event&) {
            QMetaObject::invokeMethod(
                this,
                &AutoWidget::updateButtons,
                Qt::QueuedConnection
            );
        }
    );
}