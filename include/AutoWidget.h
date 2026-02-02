#pragma once

#include <QVBoxLayout>
#include <QLabel>
#include <QSettings>
#include <QPushButton>
#include <QButtonGroup>
#include <ntcore.h>

class AutoWidget : public QWidget {
    Q_OBJECT

public:

    explicit AutoWidget(QWidget* parent = nullptr);
    ~AutoWidget() {}

    void setupNT();
    void buttonClicked(std::string value);
    void updateButtons();
    void removeButtons();
    void openPopup();
    NT_Inst inst;
    NT_Subscriber optionsSub;
    NT_Subscriber activeSub;
    NT_Subscriber defaultSub;
    NT_Publisher selectionPub;
    QButtonGroup buttons;
    QVBoxLayout* layout;
    QLabel* noAutos;
    QSettings settings;
    QPushButton editButton;
    QLabel currentLabel;
    NT_Listener updateListener;
    NT_Listener disconnectListener;
};