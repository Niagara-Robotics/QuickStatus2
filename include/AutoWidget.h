#pragma once

#include <QVBoxLayout>
#include <QLabel>
#include <QSettings>
#include <QPushButton>
#include <QButtonGroup>
#include "networktables/IntegerTopic.h"
#include "networktables/StringArrayTopic.h"
#include "networktables/StringTopic.h"

class AutoWidget : public QWidget {
    Q_OBJECT

public:

    explicit AutoWidget(QWidget* parent = nullptr);
    ~AutoWidget() {}

    void setupNT();
    void buttonClicked(std::string value);
    void categoryChanged(QString value);
    void updateButtons();
    void removeButtons();
    void openPopup();
    void updateMatch();

    nt::StringArraySubscriber optionsSub;
    nt::StringSubscriber activeSub;
    nt::StringSubscriber defaultSub;
    nt::IntegerSubscriber matchNumberSub;
    nt::StringPublisher selectionPub;

    QButtonGroup buttons;
    QButtonGroup categories;
    QVBoxLayout* layout;
    QLabel* noAutos;
    QSettings settings;
    QPushButton editButton;
    QLabel currentLabel;
    NT_Listener updateListener;
    NT_Listener disconnectListener;
protected:
    void resizeEvent(QResizeEvent* event);
};