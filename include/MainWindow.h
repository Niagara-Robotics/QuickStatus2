#pragma once

#include "StatusBar.h"
#include <QMainWindow>
#include <QDockWidget>

// #include <memory>

class MainWindow : public QMainWindow {
    Q_OBJECT

    // std::unique_ptr<QWidget> w;
public:

    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() {}
    void restoreApplicationState();
    QDockWidget* createNewWidget(QWidget* content);

    uint widgetCount = 0;

protected:
    void closeEvent(QCloseEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    StatusBar* statusBar;
};