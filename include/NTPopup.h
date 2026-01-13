#pragma once

#include <QInputDialog>

class NTPopup : public QDialog {
    Q_OBJECT

public:

    explicit NTPopup(QWidget* parent = nullptr);
    ~NTPopup() {}
    void cancelAction();
    void applyAction();
    QLineEdit* addressInput;
    QLineEdit* portInput;
};