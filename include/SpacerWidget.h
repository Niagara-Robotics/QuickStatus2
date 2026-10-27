#pragma once

#include <QWidget>

class SpacerWidget : public QWidget {
    Q_OBJECT

public:
    explicit SpacerWidget(QWidget* parent = nullptr);
    ~SpacerWidget() {}
};