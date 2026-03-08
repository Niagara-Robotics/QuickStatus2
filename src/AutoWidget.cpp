#include <QRadioButton>
#include <QTimer>
#include "AutoWidget.h"
#include "AutoPopup.h"

void AutoWidget::setupNT() {
    QString autoName = (settings.value("autoChooser").toString());
    currentLabel.setText(QString::fromStdString("Current Auto Group: ") + autoName);
    currentLabel.adjustSize();

    nt::Unsubscribe(optionsSub);
    nt::Unsubscribe(activeSub);
    nt::Unsubscribe(defaultSub);
    nt::Unpublish(selectionPub);

    inst = nt::GetDefaultInstance();
    std::string autoPath = "/SmartDashboard/" + autoName.toStdString() + "/";
    optionsSub = nt::Subscribe(
        nt::GetTopic(inst,  autoPath + "options"), 
        NT_STRING_ARRAY, "string[]");
    activeSub = nt::Subscribe(
        nt::GetTopic(inst, autoPath + "active"), 
        NT_STRING, "string");
    defaultSub = nt::Subscribe(
        nt::GetTopic(inst, autoPath + "default"), 
        NT_STRING, "string");
    selectionPub = nt::Publish(
        nt::GetTopic(inst, autoPath + "selected"),
        NT_STRING, "string");
    
    auto listenerInst = nt::NetworkTableInstance::GetDefault();
    listenerInst.RemoveListener(updateListener);
    listenerInst.RemoveListener(disconnectListener);

    updateListener = listenerInst.AddListener(
        listenerInst.GetTopic(autoPath + "active"),
        nt::EventFlags::kValueAll,
        [this](const nt::Event&) {
            QMetaObject::invokeMethod(
                this,
                &AutoWidget::updateButtons,
                Qt::QueuedConnection
            );
        }
    );

    disconnectListener = listenerInst.AddListener(
        listenerInst.GetTopic(autoPath + "options"),
        nt::EventFlags::kUnpublish,
        [this](const nt::Event&) {
            QMetaObject::invokeMethod(
                this,
                &AutoWidget::updateButtons,
                Qt::QueuedConnection
            );
        }
    );

    updateButtons();
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
    noAutos->setVisible(options.empty());
}

void AutoWidget::openPopup() {
    AutoPopup* popup = new AutoPopup(this, this);
}

AutoWidget::AutoWidget(QWidget* parent):QWidget(parent) {
    setWindowTitle("Auto Chooser");
    layout = new QVBoxLayout(this);
    this->setLayout(layout);
    layout->setContentsMargins(10,20,10,10);

    //ensure value exists by giving it a default
    if (!settings.value("autoChooser").isValid()) {
        settings.setValue("autoChooser", "Auto Chooser");
    }
    currentLabel.setObjectName("currentLabel");
    editButton.setIcon(QIcon(":/images/auto/edit"));
    editButton.setFlat(true);
    editButton.setObjectName("autoEditButton");

    connect(&editButton, &QPushButton::clicked, this, &AutoWidget::openPopup);
    QWidget* editContainer = new QWidget(this);
    editContainer->setFixedHeight(30);
    currentLabel.setParent(editContainer);
    editButton.setParent(editContainer);
    editContainer->adjustSize();

    buttons.setParent(this);
    noAutos = new QLabel("No autos detected", this);
    noAutos->setAlignment(Qt::AlignCenter);
    noAutos->setFont(QFont("B612", 40, 900));
    layout->addWidget(noAutos);

    // updateButtons();

    QTimer::singleShot(0, this, &AutoWidget::setupNT);
}