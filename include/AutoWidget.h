#pragma once

#include <QVBoxLayout>
#include <QLabel>
#include <ntcore.h>
#include <QButtonGroup>

class AutoWidget : public QWidget {
    Q_OBJECT

public:

    explicit AutoWidget(QWidget* parent = nullptr);
    ~AutoWidget() {}

    void setupNT();
    void buttonClicked(std::string value);
    void updateButtons();
    void removeButtons();
    NT_Inst inst;
    NT_Subscriber optionsSub;
    NT_Subscriber activeSub;
    NT_Subscriber defaultSub;
    NT_Publisher selectionPub;
    QButtonGroup buttons;
    QVBoxLayout* layout;
    QLabel* noAutos;
};