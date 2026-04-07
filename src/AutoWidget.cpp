#include <QRadioButton>
#include <QTimer>
#include <QtCore/qhashfunctions.h>
#include <QtCore/qnamespace.h>
#include <QtWidgets/qabstractbutton.h>
#include <QtWidgets/qgridlayout.h>
#include <QtWidgets/qlayoutitem.h>
#include <QtWidgets/qradiobutton.h>
#include <QtWidgets/qwidget.h>
#include <string>
#include <vector>
#include "AutoWidget.h"
#include "AutoPopup.h"
#include "ntcore_c.h"
#include "ntcore_cpp.h"
#include "ntcore_cpp_types.h"

void AutoWidget::setupNT() {
    QString autoName = (settings.value("autoChooser").toString());
    currentLabel.setText(QString::fromStdString("Current Auto Group: ") + autoName);
    currentLabel.adjustSize();

    nt::Unsubscribe(optionsSub);
    nt::Unsubscribe(activeSub);
    nt::Unsubscribe(defaultSub);
    nt::Unpublish(selectionPub);

    inst = nt::GetDefaultInstance();
    std::string autoPath = "/SmartDashboard/" + autoName.toStdString() + "/";
    optionsSub = nt::Subscribe(
        nt::GetTopic(inst,  autoPath + "options"), 
        NT_STRING_ARRAY, "string[]");
    activeSub = nt::Subscribe(
        nt::GetTopic(inst, autoPath + "active"), 
        NT_STRING, "string");
    defaultSub = nt::Subscribe(
        nt::GetTopic(inst, autoPath + "default"), 
        NT_STRING, "string");
    matchNumberSub = nt::Subscribe(
        nt::GetTopic(inst, "/FMSInfo/MatchNumber"),
        NT_INTEGER, "int");
    selectionPub = nt::Publish(
        nt::GetTopic(inst, autoPath + "selected"),
        NT_STRING, "string");
    
    auto listenerInst = nt::NetworkTableInstance::GetDefault();
    listenerInst.RemoveListener(updateListener);
    listenerInst.RemoveListener(disconnectListener);

    updateListener = listenerInst.AddListener(
        listenerInst.GetTopic(autoPath + "active"),
        nt::EventFlags::kValueAll,
        [this](const nt::Event&) {
            QMetaObject::invokeMethod(
                this,
                &AutoWidget::updateButtons,
                Qt::QueuedConnection
            );
        }
    );

    disconnectListener = listenerInst.AddListener(
        listenerInst.GetTopic(autoPath + "options"),
        nt::EventFlags::kUnpublish,
        [this](const nt::Event&) {
            QMetaObject::invokeMethod(
                this,
                &AutoWidget::updateButtons,
                Qt::QueuedConnection
            );
        }
    );

    listenerInst.AddListener(
        listenerInst.GetTopic("/FMSInfo/MatchNumber"),
        nt::EventFlags::kValueAll,
        [this](const nt::Event&) {
            QMetaObject::invokeMethod(
                this,
                &AutoWidget::updateMatch,
                Qt::QueuedConnection
            );
        }
    );

    updateButtons();
}

void AutoWidget::updateMatch() {
    QSettings settings;
    int currentMatch = nt::GetInteger(matchNumberSub, 0);
    int lastMatch = settings.value("lastMatch").toInt();
    if (currentMatch != lastMatch) {
        buttonClicked(nt::GetString(defaultSub, ""));
        updateButtons();
    }
    settings.setValue("lastMatch", currentMatch);
}

void AutoWidget::buttonClicked(std::string value) {
    nt::SetString(selectionPub, value);
}

void AutoWidget::categoryChanged(std::string value) {
    for (QAbstractButton* button : buttons.buttons()) {
        QString originalText = button->windowIconText();
        button->setVisible(
            originalText.startsWith(QString::fromStdString(value) + " ") ||
            originalText == "None"
        );
    }
}

void AutoWidget::removeButtons() {
    for (QAbstractButton* button : buttons.buttons()) {
        buttons.removeButton(button);
        button->deleteLater();
    }
}

void AutoWidget::updateButtons() {
    std::string active = nt::GetString(activeSub, "");
    std::string defaultSelected = nt::GetString(defaultSub, "");
    std::vector<std::string> options = nt::GetStringArray(optionsSub, std::span<std::string>());
    std::vector<std::string> buttonOrder[6];
    removeButtons();
    int i = 0;
    for (std::string option : options) {
        if (option == "None") {
            buttonOrder[0].insert(buttonOrder[0].begin(), option);
            options[i].erase();
        } 
        else if (option.find("LTrench") == 2) buttonOrder[0].push_back(option);
        else if (option.find("LBump") == 2) buttonOrder[1].push_back(option);
        else if (option.find("Hub") == 2) buttonOrder[2].push_back(option);
        else if (option.find("RBump") == 2) buttonOrder[3].push_back(option);
        else if (option.find("RTrench") == 2) buttonOrder[4].push_back(option);
        else buttonOrder[5].push_back(option);
        i++;
    }
    for (std::vector<std::string> order : buttonOrder) {
        for (std::string option : order) {
            QRadioButton* button = new QRadioButton(QString::fromStdString(option), this);
            if (active == option) button->setChecked(true);
            std::string text = button->text().toStdString();
            for (QAbstractButton* category: categories.buttons()) {
                std::string categoryText = category->windowIconText().toStdString() + " ";
                if (text.find(categoryText) == 0) {
                    text.erase(0, categoryText.length()); // first instance of text
                }
            }
            button->setText(QString::fromStdString(text));
            if (defaultSelected == option) button->setText(button->text()+" •");
            button->setContentsMargins(10,10,10,10);
            buttons.addButton(button);
            layout->addWidget(button);
            connect(button, &QRadioButton::clicked, this, [this, option]() {
                buttonClicked(option);
            });
            button->setWindowIconText(QString::fromStdString(option));
        }
    }
    noAutos->setVisible(options.empty());
    categoryChanged(categories.checkedButton()->windowIconText().toStdString());
}

void AutoWidget::openPopup() {
    AutoPopup* popup = new AutoPopup(this, this);
}

void AutoWidget::resizeEvent(QResizeEvent* event) {
    noAutos->setFont(QFont("B612", rect().width()*0.15, 900));
}

AutoWidget::AutoWidget(QWidget* parent):QWidget(parent) {
    setWindowTitle("Auto Chooser");
    layout = new QVBoxLayout(this);
    this->setLayout(layout);
    layout->setContentsMargins(10,20,10,10);

    //ensure value exists by giving it a default
    if (!settings.value("autoChooser").isValid()) {
        settings.setValue("autoChooser", "Auto Chooser");
    }
    currentLabel.setObjectName("currentLabel");
    // editButton.setIcon(QIcon(":/images/auto/edit"));
    editButton.setFlat(true);
    editButton.setObjectName("autoEditButton");

    connect(&editButton, &QPushButton::clicked, this, &AutoWidget::openPopup);
    QWidget* editContainer = new QWidget(this);
    editContainer->setFixedHeight(30);
    currentLabel.setParent(editContainer);
    editButton.setParent(editContainer);
    editContainer->adjustSize();
    QWidget* gridContainer = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(gridContainer);
    gridContainer->setFixedHeight(125);
    gridContainer->setLayout(gridLayout);
    gridContainer->setObjectName("categoriesLayout");
    layout->addWidget(gridContainer);
    categories.addButton(new QRadioButton("Simple", this));
    categories.addButton(new QRadioButton("Less Adv.", this));
    categories.addButton(new QRadioButton("More Adv.", this));
    categories.addButton(new QRadioButton("Complex", this));
    QString categoryIds[] = {"S", "L", "M", "C"};

    for (int i=0; i < categories.buttons().length(); i++) {
        QAbstractButton* button = categories.buttons()[i];
        button->setObjectName("categoryButton");
        button->setWindowIconText(categoryIds[i]);
        button->setChecked(i==0);

        std::string category = button->windowIconText().toStdString();
        connect(button, &QRadioButton::clicked, this, [this, category]() {
            categoryChanged(category);
        });

        gridLayout->addWidget(button, (i/2)%2,i%2);
    }

    buttons.setParent(this);
    categories.setParent(this);

    noAutos = new QLabel("No autos\ndetected", this);
    noAutos->setAlignment(Qt::AlignCenter);
    noAutos->setFont(QFont("B612", 40, 900));
    layout->addWidget(noAutos);

    // layout->setStretchFactor(gridLayout, 0);
    layout->setStretchFactor(noAutos, 35);
    layout->setAlignment(Qt::AlignTop);

    // updateButtons();

    QTimer::singleShot(0, this, &AutoWidget::setupNT);


}