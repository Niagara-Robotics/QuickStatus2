#include "SpacerWidget.h"
#include <QBoxLayout>
#include <QLabel>
SpacerWidget::SpacerWidget(QWidget* parent):QWidget(parent) {
    setWindowTitle("Spacer");
    QLabel* chinese = new QLabel("瘋狂的油", this);
    QLabel* english = new QLabel("mad oil");
    chinese->setFont(QFont("B612", 60));
    english->setFont(QFont("B612", 18));
    chinese->setAlignment(Qt::AlignCenter);
    english->setAlignment(Qt::AlignCenter);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(chinese, 0);
    layout->addWidget(english,0);
    setLayout(layout);
}