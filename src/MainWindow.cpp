#include <QGridLayout>
#include <QSettings>
#include <QtCore/qnamespace.h>

#include "MainWindow.h"
#include "AutoWidget.h"
#include "ControlModeWidget.h"
#include "FuelWidget.h"
#include "ShiftWidget.h"
#include "ShooterWidget.h"
#include "SpacerWidget.h"
#include "StatusBar.h"
#include "SwerveWidget.h"

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

    StatusBar* statusBar = new StatusBar(this);
    setStatusBar(statusBar);

    QDockWidget* shiftWidget = createNewWidget(new ShiftWidget(this));
    QDockWidget* autoWidget = createNewWidget(new AutoWidget(this));
    QDockWidget* fuelWidget = createNewWidget(new FuelWidget(this));
    QDockWidget* swerveWidget = createNewWidget(new SwerveWidget(this));
    QDockWidget* shooterWidget = createNewWidget(new ShooterWidget(this));
    QDockWidget* controlModeWidget = createNewWidget(new ControlModeWidget(this));
    QDockWidget* driveSpacer = createNewWidget(new SpacerWidget(this));

    QTimer::singleShot(0, this, &MainWindow::restoreApplicationState);
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