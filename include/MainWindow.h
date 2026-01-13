#pragma once

#include <QMainWindow>
#include <QtWidgets/qwidget.h>
// #include <memory>

class MainWindow : public QMainWindow {
    Q_OBJECT

    // std::unique_ptr<QWidget> w;
public:

    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() {}
    void restoreApplicationState();
    void createPopup();
    QMenuBar* createMenuBar();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
};