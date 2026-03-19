#include "ControlModeWidget.h"
#include "Constants.h"
#include "networktables/NetworkTableInstance.h"
#include "ntcore_c.h"
#include "ntcore_cpp.h"
#include <QtGui/qpainter.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qformlayout.h>
#include <QtWidgets/qgridlayout.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qwidget.h>

void ControlModeWidget::updateText() {
    bool driverAssistedMode = nt::GetBoolean(driverAssistedModeSub, true);
    bool operatorAssistedMode = nt::GetBoolean(operatorAssistedModeSub, true);
    std::string driverAssistedText;
    std::string operatorAssistedText;
    std::string driverColour;
    std::string operatorColour;
    if (nt::GetTopicExists(driverAssistedModeSub)) {
        driverAssistedText = (driverAssistedMode)? "Assisted": "Manual";
        driverColour = (((driverAssistedMode)? Constants::AT_TARGET.name(): Constants::IN_PROGRESS.name())).toStdString();
    } else {
        driverAssistedText = "—";
        driverColour = "#55FFFFFF";
    }
    if (nt::GetTopicExists(operatorAssistedModeSub)) {
        operatorAssistedText = (operatorAssistedMode)? "Field": "Robot";
        operatorColour = (((operatorAssistedMode)? Constants::AT_TARGET.name(): Constants::IN_PROGRESS.name())).toStdString();
    } else {
        operatorAssistedText = "—";
        operatorColour = "#55FFFFFF";
    }
    
    float pointSize = std::min((double)driverLabel->width()/2, (double)driverLabel->height())/2;
    driverLabel->setText(QString::fromStdString(fmt::format(
            "<span style='font-size: {}px; font-weight: 100;'>Driver</span><br>"
            "<span style='font-size: {}px; color: {};'>{}</span>",
            pointSize/2, pointSize, driverColour, driverAssistedText
    )));

    pointSize = std::min((double)operatorLabel->width()/2, (double)operatorLabel->height())/2;
    operatorLabel->setText(QString::fromStdString(fmt::format(
            "<span style='font-size: {}px; font-weight: 100;'>Drive Centric</span><br>"
            "<span style='font-size: {}px; color: {};'>{}</span>",
            pointSize/2, pointSize, operatorColour, operatorAssistedText
    )));
}

void ControlModeWidget::resizeEvent(QResizeEvent* event) {
    double sizeRatio = (double)width()/height();
    layout->setDirection((sizeRatio > 2)? QBoxLayout::LeftToRight: QBoxLayout::TopToBottom);
    updateText();
}

ControlModeWidget::ControlModeWidget(QWidget* parent):QWidget(parent) {
    setWindowTitle("Control Mode");

    setLayout(layout);
    layout->setSpacing(10);
    layout->addWidget(driverLabel);
    layout->addWidget(operatorLabel);

    driverLabel->setFont(QFont("B612", 16, 900));
    operatorLabel->setFont(QFont("B612", 16, 900));

    inst = nt::GetDefaultInstance();

    driverAssistedModeSub = nt::Subscribe(nt::GetTopic(
        inst, "/SmartDashboard/driverAssistedMode"), NT_BOOLEAN, "boolean"
    );
    operatorAssistedModeSub = nt::Subscribe(nt::GetTopic(
        inst, "/SmartDashboard/isFieldCentric"), NT_BOOLEAN, "boolean"
    );

    // pain();

    nt::NetworkTableInstance listenerInst = nt::NetworkTableInstance::GetDefault();

    NT_Listener driverUpdateListener = listenerInst.AddListener(
        listenerInst.GetTopic("/SmartDashboard/driverAssistedMode"),
        nt::EventFlags::kValueAll,
        [this](const nt::Event&) {
            QMetaObject::invokeMethod(
                this,
                &ControlModeWidget::updateText,
                Qt::QueuedConnection
            );
        }
    );
    NT_Listener driverDisconnectListener = listenerInst.AddListener(
        listenerInst.GetTopic("/SmartDashboard/driverAssistedMode"),
        nt::EventFlags::kUnpublish,
        [this](const nt::Event&) {
            QMetaObject::invokeMethod(
                this,
                &ControlModeWidget::updateText,
                Qt::QueuedConnection
            );
        }
    );
    NT_Listener operatorUpdateListener = listenerInst.AddListener(
        listenerInst.GetTopic("/SmartDashboard/isFieldCentric"),
        nt::EventFlags::kValueAll,
        [this](const nt::Event&) {
            QMetaObject::invokeMethod(
                this,
                &ControlModeWidget::updateText,
                Qt::QueuedConnection
            );
        }
    );
    NT_Listener operatorDisconnectListener = listenerInst.AddListener(
        listenerInst.GetTopic("/SmartDashboard/isFieldCentric"),
        nt::EventFlags::kUnpublish,
        [this](const nt::Event&) {
            QMetaObject::invokeMethod(
                this,
                &ControlModeWidget::updateText,
                Qt::QueuedConnection
            );
        }
    );
}