#pragma once

#include "AutoWidget.h"
#include "networktables/NetworkTableInstance.h"
#include <QInputDialog>

class AutoPopup : public QDialog {
    Q_OBJECT

public:

    explicit AutoPopup(QWidget* parent = nullptr, AutoWidget* autoWidget = nullptr);
    ~AutoPopup() {}
    void cancelAction();
    void applyAction();
    void checkTables();
    QStringList validTables;
    QLineEdit* searchInput;
    nt::NetworkTableInstance inst = nt::NetworkTableInstance::GetDefault();
    NT_Listener topicListener;

    AutoWidget* reportTo;

};