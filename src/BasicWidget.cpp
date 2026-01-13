#include <QLabel>
#include <QBoxLayout>
#include <QtCore/qnamespace.h>
#include "BasicWidget.h"

BasicWidget::BasicWidget(QWidget* parent, QString text):QWidget(parent) {
    QLabel* textLabel = new QLabel(text, this);
    // textLabel->setMinimumSize(200,200);
}