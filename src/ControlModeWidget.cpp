#include "ControlModeWidget.h"
#include "Constants.h"
#include <QTimer>
#include "networktables/NetworkTableInstance.h"

void ControlModeWidget::updateText() {
    bool driverAssistedMode = driverAssistedModeSub.Get();
    bool isFieldCentric = fieldCentricSub.Get();
    std::string driverAssistedText;
    std::string fieldCentricText;
    std::string driverColour;
    std::string operatorColour;
    if (driverAssistedModeSub.Exists()) {
        driverAssistedText = (driverAssistedMode)? "Assisted": "Manual";
        driverColour = (((driverAssistedMode)? Constants::AT_TARGET.name(): Constants::IN_PROGRESS.name())).toStdString();
    } else {
        driverAssistedText = "—";
        driverColour = "#55FFFFFF";
    }
    if (fieldCentricSub.Exists()) {
        fieldCentricText = (isFieldCentric)? "Field": "Robot";
        operatorColour = (((isFieldCentric)? Constants::AT_TARGET.name(): Constants::IN_PROGRESS.name())).toStdString();
    } else {
        fieldCentricText = "—";
        operatorColour = "#55FFFFFF";
    }
    
    float pointSize = std::min(static_cast<double>(driverLabel->width())/2, static_cast<double>(driverLabel->height()))/2;
    driverLabel->setText(QString::fromStdString(fmt::format(
            "<span style='font-size: {}px; font-weight: 100;'>Driver</span><br>"
            "<span style='font-size: {}px; color: {};'>{}</span>",
            pointSize/2, pointSize, driverColour, driverAssistedText
    )));

    pointSize = std::min(static_cast<double>(operatorLabel->width())/2, static_cast<double>(operatorLabel->height()))/2;
    operatorLabel->setText(QString::fromStdString(fmt::format(
            "<span style='font-size: {}px; font-weight: 100;'>Drive Centric</span><br>"
            "<span style='font-size: {}px; color: {};'>{}</span>",
            pointSize/2, pointSize, operatorColour, fieldCentricText
    )));
}

void ControlModeWidget::resized() {
    double sizeRatio = static_cast<double>(width())/height();
    layout->setDirection((sizeRatio > 2)? QBoxLayout::LeftToRight: QBoxLayout::TopToBottom);
    updateText();
}

void ControlModeWidget::resizeEvent(QResizeEvent* event) {
    resized();
}

ControlModeWidget::ControlModeWidget(QWidget* parent):QWidget(parent) {
    setWindowTitle("Control Mode");

    setLayout(layout);
    layout->setSpacing(10);
    layout->addWidget(driverLabel);
    layout->addWidget(operatorLabel);

    driverLabel->setFont(QFont("B612", 16, 900));
    operatorLabel->setFont(QFont("B612", 16, 900));

    nt::NetworkTableInstance inst = nt::NetworkTableInstance::GetDefault();

    driverAssistedModeSub = inst.GetBooleanTopic("/SmartDashboard/driverAssistedMode").Subscribe(true);
    fieldCentricSub = inst.GetBooleanTopic("/SmartDashboard/isFieldCentric").Subscribe(true);

    // pain();

    NT_Listener driverUpdateListener = inst.AddListener(
        inst.GetTopic("/SmartDashboard/driverAssistedMode"),
        nt::EventFlags::kValueAll,
        [this](const nt::Event&) {
            QMetaObject::invokeMethod(
                this,
                &ControlModeWidget::updateText,
                Qt::QueuedConnection
            );
        }
    );
    NT_Listener driverDisconnectListener = inst.AddListener(
        inst.GetTopic("/SmartDashboard/driverAssistedMode"),
        nt::EventFlags::kUnpublish,
        [this](const nt::Event&) {
            QMetaObject::invokeMethod(
                this,
                &ControlModeWidget::updateText,
                Qt::QueuedConnection
            );
        }
    );
    NT_Listener operatorUpdateListener = inst.AddListener(
        inst.GetTopic("/SmartDashboard/isFieldCentric"),
        nt::EventFlags::kValueAll,
        [this](const nt::Event&) {
            QMetaObject::invokeMethod(
                this,
                &ControlModeWidget::updateText,
                Qt::QueuedConnection
            );
        }
    );
    NT_Listener operatorDisconnectListener = inst.AddListener(
        inst.GetTopic("/SmartDashboard/isFieldCentric"),
        nt::EventFlags::kUnpublish,
        [this](const nt::Event&) {
            QMetaObject::invokeMethod(
                this,
                &ControlModeWidget::updateText,
                Qt::QueuedConnection
            );
        }
    );
    QTimer::singleShot(0, this, [this](){resized();});
}
