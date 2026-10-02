#include <QGridLayout>
#include <QSettings>
#include <QEvent>
#include <QtWidgets/qdockwidget.h>
#include <QtWidgets/qpushbutton.h>

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
    
    QLayout* dockLayout = new QGridLayout();
    QWidget* dockContainerWidget = new QWidget();
    dockContainerWidget->setObjectName("containerTest");
    dockContainerWidget->setMaximumSize(0,0);
    dockContainerWidget->setLayout(dockLayout);
    dockLayout->setContentsMargins(0,0,0,0);
    // setCentralWidget(dockContainerWidget);
    setDockNestingEnabled(true);
    setContentsMargins(10,10,10,10);
    setAutoFillBackground(true);
    setBackgroundRole(QPalette::ColorRole::Mid);

    statusBar = new StatusBar(this);
    setStatusBar(statusBar);

    QTimer::singleShot(0, this, &MainWindow::restoreApplicationState);

    QDockWidget* shiftWidget = createNewWidget(new ShiftWidget(this));
    QDockWidget* autoWidget = createNewWidget(new AutoWidget(this));
    QDockWidget* swerveWidget = createNewWidget(new SwerveWidget(this));
    QDockWidget* shooterWidget = createNewWidget(new ShooterWidget(this));
    QDockWidget* controlModeWidget = createNewWidget(new ControlModeWidget(this));
    QDockWidget* driveSpacer = createNewWidget(new SpacerWidget(this));
    QDockWidget* calibrationWidget = createNewWidget(new CalibrationWidget(this));
    QList<QDockWidget*> dockWidgets = {
        shiftWidget, 
        autoWidget, 
        swerveWidget, 
        shooterWidget, 
        controlModeWidget, 
        driveSpacer, 
        calibrationWidget
    };

    QWidget* shiftContent = shiftWidget->widget();
    QWidget* swerveContent = swerveWidget->widget();
    QWidget* shooterContent = shooterWidget->widget();
    QWidget* calibrationContent = calibrationWidget->widget();
    NT_Inst inst = nt::GetDefaultInstance();
    static bool isConnected = false;

    QTimer* refreshTimer = new QTimer(this);
    refreshTimer->setTimerType(Qt::CoarseTimer);
    connect(refreshTimer, &QTimer::timeout, this, [this, shiftWidget, shiftContent, swerveWidget, swerveContent, shooterWidget, shooterContent]() {
        if (!isConnected) return;
        if (shiftWidget->isVisible()) shiftContent->update();
        if (swerveWidget->isVisible()) swerveContent->update();
        if (shooterWidget->isVisible()) shooterContent->update();
    });
    refreshTimer->start(33);

    QTimer* lessImportantTimer = new QTimer(this);
    lessImportantTimer->setTimerType(Qt::CoarseTimer);
    
    connect(lessImportantTimer, &QTimer::timeout, this, [this, inst, calibrationWidget, calibrationContent]() {
        isConnected = nt::IsConnected(inst);
        if (calibrationWidget->isVisible()) calibrationContent->update();
    });
    lessImportantTimer->start(500);

    
    for (QDockWidget* dockWidget : dockWidgets) {
        dockWidget->installEventFilter(this);
    }
    QTimer::singleShot(0, this, [this, dockWidgets](){
        for (QPushButton* button : statusBar->buttonList) {
            for (QDockWidget* dockWidget : dockWidgets) {
                if (dockWidget->windowTitle() == button->accessibleName()) button->setChecked(dockWidget->isVisible());
            }
            statusBar->connect(button, &QPushButton::toggled, statusBar, [this, button, dockWidgets]() {
                // qDebug()<<button->isChecked();
                for (QDockWidget* dockWidget : dockWidgets) {
                    if (dockWidget->windowTitle() == button->accessibleName()) {
                        dockWidget->setVisible(button->isChecked());
                    };
                }
                // Make window visible/invisible
            });
        }
    });
}

void MainWindow::restoreApplicationState()
{
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