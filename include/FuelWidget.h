#pragma once

#include "ntcore_c.h"
#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QtGui/qpainter.h>
#include <QtGui/qpen.h>
#include <QtGui/qwindowdefs.h>
#include <QtWidgets/qwidget.h>
#include <ntcore.h>

class FuelWidget : public QWidget {
    Q_OBJECT

public:

    explicit FuelWidget(QWidget* parent = nullptr);
    ~FuelWidget() {}
    NT_Inst inst;
    NT_Subscriber detectorSub;
    NT_Subscriber targetDataSub;
    QLabel* textLabel = new QLabel();
    std::string detector;
    int tagCount;
    float pointSize;
    QPen pen;
    QPainter* painter;
    QRect boundingRect;
private:
protected:
    void check();
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
};