#pragma once

#include "StatusBar.h"
#include <QInputDialog>
#include <QSettings>
#include <QComboBox>

class NTPopup : public QDialog {
    Q_OBJECT

public:

    explicit NTPopup(QWidget* parent = nullptr, StatusBar* statusBar = nullptr);
    ~NTPopup() {}
    void cancelAction();
    void resetAction();
    void applyAction();
    void typeChange();
    void typeUpdate();
    QLineEdit* addressInput;
    QLineEdit* portInput;
    QSettings settings;
    QComboBox* addressTypeChooser;

    StatusBar* reportTo;
};