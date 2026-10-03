#include <QSettings>
#include <QPushButton>
#include <QtCore/qnamespace.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qdialog.h>
#include <QtWidgets/qmessagebox.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qwidget.h>
#include <QEvent>
#include <QMessageBox>
#include <QTextBrowser>

#include "networktables/NetworkTableInstance.h"

#include "StatusBar.h"
#include "NTPopup.h"

void StatusBar::ConnectionListenerCallback(nt::Event event) {
    if (event.Is(nt::EventFlags::kConnected)) {
        connectionState = true;
    } else if (event.Is(nt::EventFlags::kDisconnected)) {
        connectionState = false;
        this->parentWidget()->update();
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

QLabel* createDivider() {
    // 1. Create the divider label
    QLabel *divider = new QLabel();
    QPixmap pixmap(":/images/status_bar/divider");
    pixmap.setDevicePixelRatio(2);

    // 2. Scale the divider appropriately
    divider->setPixmap(pixmap.scaledToHeight(40, Qt::SmoothTransformation));
    divider->setAlignment(Qt::AlignCenter);
    divider->setFixedSize(5,20);
    divider->setObjectName("statusDivider");

    return divider;
}

QPushButton* StatusBar::createToggleButton(QString name, QString displayName) {
    QPushButton* toggleButton = new QPushButton();
    toggleButton->setFlat(true);
    toggleButton->setFixedSize(22,22);
    toggleButton->setCheckable(true);
    toggleButton->setObjectName(name);
    toggleButton->setAccessibleName(displayName);

    buttonList.append(toggleButton);
    
    addPermanentWidget(toggleButton);
    if (name != "visionButton") addPermanentWidget(createDivider());

    toggleButton->installEventFilter(this);

    // QPoint test = mapToGlobal(toggleButton->rect().topLeft());
    // tooltip->move(test.x(), test.y()); // puts in top right??

    return toggleButton;
}

bool StatusBar::eventFilter(QObject* watched, QEvent* event) {
    // Check if the event is coming from your button
    QPushButton* button = qobject_cast<QPushButton*>(watched);

    if (button && tooltip) {
        if (event->type() == QEvent::Enter) {
            tooltip->setText(button->accessibleName());
            tooltip->adjustSize();
            // Position the tooltip *above* the button
            QPoint btnPos = button->mapToGlobal(QPoint(0, 0));
            int x = btnPos.x() + (button->width() / 2) - (tooltip->width() / 2);
            if (button->objectName() == "visionButton") x -= 15;
            int y = btnPos.y() - tooltip->height() - 5; // 5px padding above

            tooltip->move(x, y);
            tooltip->show();
            return true;
        } 
        else if (event->type() == QEvent::Leave) {
            tooltip->hide();
            return true;
        }
    }
    return QStatusBar::eventFilter(watched, event);
}

StatusBar::StatusBar(QWidget* parent):QStatusBar(parent) {
    // NTPopup* popup = new NTPopup(this, this);
    setObjectName("statusBar");
    setFixedHeight(40);
    setContentsMargins(5,10,5,0);
    QFont b612("B612", 12);
    connectionStatus->setFont(b612);
    
    updateStatus();
    
    QPushButton* editButton = new QPushButton();
    editButton->setFlat(true);
    editButton->setFixedSize(16,16);
    // editButton->setIcon(QIcon(":/images/auto/edit"));
    editButton->setObjectName("genericButton");
    connect(editButton, &QPushButton::clicked, this, &StatusBar::openPopup);

    // QLabel* latencyHeader = new QLabel("Latency:");
    // latencyHeader->setFont(b612);
    // latencyStatus->setFont(QFont("B612 Mono", 16));

    shiftButton = createToggleButton("shiftButton", "Alliance Shifts");
    autoButton = createToggleButton("autoButton", "Auto Chooser");
    swerveButton = createToggleButton("swerveButton", "Swerve");
    shooterButton = createToggleButton("shooterButton", "Shooter");
    controlModeButton = createToggleButton("controlModeButton", "Control Mode");
    spacerButton = createToggleButton("spacerButton", "Spacer");
    visionButton = createToggleButton("visionButton", "Calibration");

    // connect(swerveButton, &QPushButton::clicked, this, &StatusBar::openPopup);

    addWidget(editButton);
    addWidget(connectionStatus);

    tooltip = new RoundedTooltip("", nullptr); 
    tooltip->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
    tooltip->setAttribute(Qt::WA_ShowWithoutActivating);
    tooltip->setAttribute(Qt::WA_TranslucentBackground);
    tooltip->setObjectName("tooltip");

    QWidget* container = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(container);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);

    QPushButton* helpButton = new QPushButton("I\'m confused!"); 
    helpButton->setFont(b612);
    helpButton->setFlat(true);
    helpButton->setObjectName("helpButton");

    QDialog* popup = new QDialog(this);
    popup->setWindowTitle("QuickStatus Help");
    popup->setWindowFlags(
        Qt::Dialog |
        Qt::CustomizeWindowHint |
        Qt::WindowTitleHint |
        Qt::WindowCloseButtonHint
    );
    
    popup->setWindowModality(Qt::WindowModality::ApplicationModal);
    popup->setMinimumSize(360,280);
    
    QVBoxLayout *popupLayout = new QVBoxLayout(popup);
    QTextBrowser *textBrowser = new QTextBrowser(popup);
    textBrowser->setObjectName("helpText");
    
    QString markdownText = 
        "# QuickStatus 2.1.7\n"
        // "You can use **bold text**, *italics*, or `inline code` easily.\n\n"
        "Some <span style='color: #50ce63;'>quick</span> tips:\n"
        "- Each widget can be enabled/disabled by clicking on its icon in the bottom right corner\n"
        "- By default, each widget is docked inside the main window, but they can be popped out by dragging its titlebar or clicking the icon to the right of the X\n"
        "- Widgets can be resized while docked by dragging in the space between them\n"
        "- While dragging a widget, you can hover over another widget to create a tab group, allowing you to switch between widgets\n\n"
        "this menu lowkey useless but i wanted to add more random stuff\n";

    textBrowser->setMarkdown(markdownText);
    popupLayout->addWidget(textBrowser);

    connect(helpButton, &QPushButton::clicked, this, [popup](){
        popup->setWindowState(Qt::WindowNoState);
        popup->resize(360,280);
        popup->show();

    });

    layout->addStretch();
    layout->addWidget(helpButton);
    layout->addStretch();

    addWidget(container,1);
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