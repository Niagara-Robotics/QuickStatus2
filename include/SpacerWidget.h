#pragma once

#include <QtWidgets/qwidget.h>

class SpacerWidget : public QWidget {
    Q_OBJECT

public:
    explicit SpacerWidget(QWidget* parent = nullptr);
    ~SpacerWidget() {}
};