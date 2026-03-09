#pragma once

#include <QWidget>
#include <QTimer>
#include <QSvgRenderer>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qgridlayout.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qwidget.h>
#include <ntcore.h>

class ControlModeWidget : public QWidget {
    Q_OBJECT

public:

    explicit ControlModeWidget(QWidget* parent = nullptr);
    ~ControlModeWidget() {}
    NT_Inst inst;
    NT_Subscriber driverAssistedModeSub;
    NT_Subscriber operatorAssistedModeSub;

    QLabel* driverLabel = new QLabel();
    QLabel* operatorLabel = new QLabel();
    QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight, this);

protected:
    void resizeEvent(QResizeEvent* event);
    void updateText();
};