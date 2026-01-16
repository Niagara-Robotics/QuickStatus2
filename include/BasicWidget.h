#pragma once

#include <QWidget>

class BasicWidget : public QWidget {
    Q_OBJECT

public:

    explicit BasicWidget(QString text = "", QWidget* parent = nullptr);
    ~BasicWidget() {}
};