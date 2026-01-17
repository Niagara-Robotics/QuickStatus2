#pragma once

#include "StatusBar.h"
#include <QInputDialog>

class NTPopup : public QDialog {
    Q_OBJECT

public:

    explicit NTPopup(QWidget* parent = nullptr, StatusBar* statusBar = nullptr);
    ~NTPopup() {}
    void cancelAction();
    void applyAction();
    QLineEdit* addressInput;
    QLineEdit* portInput;

    StatusBar* reportTo;
};