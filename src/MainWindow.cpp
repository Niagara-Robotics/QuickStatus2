#include <QtGui/qaction.h>
#include <QtWidgets/qmainwindow.h>
#include <frc/DriverStation.h>
#include <ntcore.h>

#include <QDockWidget>
#include <QGridLayout>
#include <QSettings>
#include <QMenuBar>
#include <QFile>
#include <QTimer>

#include "MainWindow.h"
#include "BasicWidget.h"
#include "NTPopup.h"

QDockWidget* CreateNewWidget(std::string name, QWidget* parent) {
    QDockWidget* dockContainer = new QDockWidget(QString::fromStdString(name), parent);
    dockContainer->setObjectName(dockContainer->windowTitle());
    
    BasicWidget* content = new BasicWidget(dockContainer, "hello, i don't do anything yet!");
    dockContainer->setMinimumSize(150,150);
    dockContainer->setWidget(content);
    dockContainer->setAllowedAreas(Qt::DockWidgetArea::AllDockWidgetAreas);
    dockContainer->setDockLocation(Qt::DockWidgetArea::TopDockWidgetArea);
    // dockContainer->setContentsMargins(0,0,5,5);
    // content->setAutoFillBackground(true);
    // content->setBackgroundRole(QPalette::ColorRole::Midlight);
    content->setObjectName("dockContent");
    content->setAttribute(Qt::WA_StyledBackground, true);

    return dockContainer;
}

void MainWindow::createPopup() {
    NTPopup* popup = new NTPopup(this);
}

QMenuBar* MainWindow::createMenuBar() {
    QMenuBar* menuBar = new QMenuBar(this);
    QMenu* configMenu = menuBar->addMenu("Config");

    QAction* changeNT = new QAction("Set NT Address...", this);
    changeNT->connect(changeNT, &QAction::triggered, this, &MainWindow::createPopup);

    configMenu->addAction(changeNT);
    menuBar->setNativeMenuBar(true);
    return menuBar;
}

MainWindow::MainWindow(QWidget* parent):QMainWindow(parent) {
    setMinimumSize(480,360);
    QLayout* Layout = new QGridLayout();
    QWidget* CentralWidget = new QWidget();
    CentralWidget->setMaximumSize(0,0);
    CentralWidget->setLayout(Layout);
    setCentralWidget(CentralWidget);
    setDockNestingEnabled(true);
    setContentsMargins(10,10,10,0);
    setAutoFillBackground(true);
    setBackgroundRole(QPalette::ColorRole::Mid);

    QDockWidget* test1 = CreateNewWidget("Cool Widget 1", this);
    QDockWidget* test2 = CreateNewWidget("Awesome Widget 2", this);
    QDockWidget* test3 = CreateNewWidget("Wicked Widget 3", this);

    QMenuBar* menu = MainWindow::createMenuBar();
    setMenuBar(menu);

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