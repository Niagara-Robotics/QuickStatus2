#include <QFormLayout>
#include <QThread>
#include <QCompleter>

#include "AutoPopup.h"

void AutoPopup::cancelAction() {
    close();
}
void AutoPopup::applyAction() {
    QSettings settings;
    settings.setValue("autoChooser", searchInput->text());
    reportTo->setupNT();
    close();
}

void AutoPopup::checkTables() {
    if (!inst.IsConnected()) return;
    std::vector<std::string> smartSubtables = inst.GetTable("SmartDashboard")->GetSubTables();
    validTables.clear();

    for (const std::string &subtable : smartSubtables) { //for each subtable
        if (subtable == "Shooters") continue;
        std::string topicType;
        for (int i = 0; i<40; i++) { // attempt to grab entry 40 times over 1 second
            topicType = inst.GetEntry("/SmartDashboard/"+subtable+"/.type").GetString("");
            if (!topicType.empty()) break;
            QThread::msleep(25);
        }
        if (topicType == "String Chooser") {
            validTables.emplace_back(QString::fromStdString(subtable));
            // qDebug()<<"STRING CHOOSER FOUND: "<<subtable;
        }
    }
}

AutoPopup::AutoPopup(QWidget* parent, AutoWidget* autoWidget):QDialog(parent) {
    reportTo = autoWidget;
    setWindowFlag(Qt::WindowType::WindowStaysOnTopHint);
    setWindowModality(Qt::WindowModality::ApplicationModal);
    // setFixedSize(300,150);
    setFixedSize(300,100);
    QFormLayout* layout = new QFormLayout(this);
    setWindowTitle("Set Chooser Table");
    checkTables();

    QCompleter* completer = new QCompleter(validTables);
    completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);

    QSettings settings;
    QString initialSearch = settings.value("autoChooser").toString();
    
    searchInput = new QLineEdit(initialSearch, this);
    searchInput->setCompleter(completer);

    layout->addRow("/SmartDashboard/", searchInput);

    QPushButton* applyButton = new QPushButton("Apply", this);
    connect(applyButton, &QPushButton::clicked, this, &AutoPopup::applyAction);

    QPushButton* cancelButton = new QPushButton("Cancel", this);
    connect(cancelButton, &QPushButton::clicked, this, &AutoPopup::cancelAction);

    layout->addRow(cancelButton, applyButton);

    setLayout(layout);

    show();

    // exec();
}