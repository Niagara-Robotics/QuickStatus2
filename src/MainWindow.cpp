#include <QDockWidget>
#include <QGridLayout>
#include <QSettings>
#include <QMenuBar>
#include <QTimer>

#include "MainWindow.h"
#include "BasicWidget.h"
#include "ShiftWidget.h"
#include "NTPopup.h"

QDockWidget* MainWindow::createNewWidget(QWidget* content) {
    QDockWidget* dockContainer = new QDockWidget(content->windowTitle(), this);
    dockContainer->setObjectName("widget" + std::to_string(widgetCount));
    dockContainer->setMinimumSize(150,150);
    dockContainer->setAllowedAreas(Qt::DockWidgetArea::AllDockWidgetAreas);
    dockContainer->setDockLocation(Qt::DockWidgetArea::TopDockWidgetArea);
    
    content->setObjectName("dockContent");
    content->setAttribute(Qt::WA_StyledBackground, true);
    dockContainer->setWidget(content);

    widgetCount += 1;
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
    
    QLayout* dockLayout = new QGridLayout();
    QWidget* dockContainerWidget = new QWidget();
    dockContainerWidget->setMaximumSize(0,0);
    dockContainerWidget->setLayout(dockLayout);
    setCentralWidget(dockContainerWidget);
    setDockNestingEnabled(true);
    setContentsMargins(10,10,10,0);
    setAutoFillBackground(true);
    setBackgroundRole(QPalette::ColorRole::Mid);

    QDockWidget* test1 = createNewWidget(new ShiftWidget());
    QDockWidget* test2 = createNewWidget(new BasicWidget("yoooo"));
    QDockWidget* test3 = createNewWidget(new BasicWidget("whats up"));

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