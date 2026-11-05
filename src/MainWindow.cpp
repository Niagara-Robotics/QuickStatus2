#include <QSettings>
#include <QEvent>
#include <QTimer>

#include "MainWindow.h"
#include "AutoWidget.h"
#include "ControlModeWidget.h"
#include "ShiftWidget.h"
#include "ShooterWidget.h"
#include "SpacerWidget.h"
#include "StatusBar.h"
#include "SwerveWidget.h"
#include "CalibrationWidget.h"

QDockWidget* MainWindow::createNewWidget(QWidget* content) {
    QDockWidget* dockContainer = new QDockWidget(content->windowTitle(), this);
    dockContainer->setObjectName("widget" + std::to_string(widgetCount));
    dockContainer->setMinimumSize(150,150);
    content->setMinimumSize(150,150);
    dockContainer->setAllowedAreas(Qt::DockWidgetArea::AllDockWidgetAreas);
    dockContainer->setDockLocation(Qt::DockWidgetArea::TopDockWidgetArea);
    dockContainer->setContentsMargins(0,0,0,0);
    
    content->setObjectName("dockContent");
    content->setAttribute(Qt::WA_StyledBackground, true);
    dockContainer->setAttribute(Qt::WA_StyledBackground, true);
    dockContainer->setWidget(content);

    widgetCount += 1;
    return dockContainer;
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event) {
    // Check if the event belongs to one of our QDockWidgets
    QDockWidget* dockWidget = qobject_cast<QDockWidget*>(watched);
    QString identifier = "";
    bool validEvent = false;
    bool opened = false;
    if (dockWidget) {
        identifier = dockWidget->windowTitle();
        
        if (event->type() == QEvent::Close) {
            validEvent = true;
            
        } else if (event->type() == QEvent::Show) {
            opened = true;
            validEvent = true;
        }
    }
    if (validEvent) {
        for (QPushButton* button : statusBar->buttonList) {
            if (button->accessibleName() == identifier) button->setChecked(opened);
        }
    }

    // pass the event to the base class so qt can handle it normally
    return QMainWindow::eventFilter(watched, event);
}

MainWindow::MainWindow(QWidget* parent):QMainWindow(parent) {
    setMinimumSize(480,480);
    setDockNestingEnabled(true);
    setContentsMargins(10,10,10,10);
    setAutoFillBackground(true);
    setBackgroundRole(QPalette::ColorRole::Mid);

    statusBar = new StatusBar(this);
    setStatusBar(statusBar);

    QTimer::singleShot(0, this, &MainWindow::restoreApplicationState);

    ShiftWidget* shiftWidget = new ShiftWidget(this);
    SwerveWidget* swerveWidget = new SwerveWidget(this);
    ShooterWidget* shooterWidget = new ShooterWidget(this);

    QDockWidget* shiftDock = createNewWidget(shiftWidget);
    QDockWidget* autoDock = createNewWidget(new AutoWidget(this));
    QDockWidget* swerveDock = createNewWidget(swerveWidget);
    QDockWidget* shooterDock = createNewWidget(shooterWidget);
    QDockWidget* controlModeDock = createNewWidget(new ControlModeWidget(this));
    QDockWidget* spacerDock = createNewWidget(new SpacerWidget(this));
    QDockWidget* calibrationDock = createNewWidget(new CalibrationWidget(this));
    QList<QDockWidget*> dockWidgets = {
        shiftDock, 
        autoDock, 
        swerveDock, 
        shooterDock, 
        controlModeDock, 
        spacerDock, 
        calibrationDock
    };

    nt::NetworkTableInstance inst = nt::NetworkTableInstance::GetDefault();
    static bool isConnected = false;
    
    QTimer* refreshTimer = new QTimer(this);
    refreshTimer->setTimerType(Qt::CoarseTimer);
    connect(refreshTimer, &QTimer::timeout, this, [this, shiftWidget, shiftDock, swerveWidget, swerveDock, shooterWidget, shooterDock]() {
        if (!isConnected) return;
        if (shiftDock->isVisible()) shiftWidget->update();
        if (swerveDock->isVisible()) swerveWidget->update();
        if (shooterDock->isVisible()) shooterWidget->update();
    });
    refreshTimer->start(33);

    QTimer* lessImportantTimer = new QTimer(this);
    lessImportantTimer->setTimerType(Qt::CoarseTimer);
    connect(lessImportantTimer, &QTimer::timeout, this, [this, inst]() {
        isConnected = inst.IsConnected();
    });
    lessImportantTimer->start(500);
    
    for (QDockWidget* dockWidget : dockWidgets) dockWidget->installEventFilter(this);
    
    QTimer::singleShot(0, this, [this, dockWidgets](){
        for (QPushButton* button : statusBar->buttonList) {
            for (QDockWidget* dockWidget : dockWidgets) {
                if (dockWidget->windowTitle() == button->accessibleName()) button->setChecked(dockWidget->isVisible());
            }
            statusBar->connect(button, &QPushButton::toggled, statusBar, [this, button, dockWidgets]() {
                // qDebug()<<button->isChecked();
                for (QDockWidget* dockWidget : dockWidgets) {
                    if (dockWidget->windowTitle() == button->accessibleName()) {
                        dockWidget->setVisible(button->isChecked()); // Make window visible/invisible
                    };
                }
            });
        }
    });
}

void MainWindow::restoreApplicationState() {
    QSettings settings;
    settings.beginGroup("MainWindow");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray()); // Restore toolbar/dock state
    settings.endGroup();
}

void MainWindow::closeEvent(QCloseEvent* event) {
    QSettings settings;
    settings.beginGroup("MainWindow");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.endGroup();
}