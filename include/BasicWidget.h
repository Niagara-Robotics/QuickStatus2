#pragma once

#include <QWidget>

class BasicWidget : public QWidget {
    Q_OBJECT

public:

    explicit BasicWidget(QWidget* parent = nullptr, QString text = "");
    ~BasicWidget() {}
};