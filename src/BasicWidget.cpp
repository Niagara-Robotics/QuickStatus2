#include <QLabel>
#include "BasicWidget.h"

BasicWidget::BasicWidget(QString text, QWidget* parent):QWidget(parent) {
    setWindowTitle("Test Widget");
    QLabel* textLabel = new QLabel(text, this);
}