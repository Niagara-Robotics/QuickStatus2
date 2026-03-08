#pragma once

#include <QtGui/qcolor.h>
#include <QtGui/qicon.h>

namespace Constants {
    static const QColor NO_STATUS = QColor("#FFFFFF");
    static const QColor IN_PROGRESS = QColor("#FFC600");
    static const QColor AT_TARGET = QColor("#44da5b");
    static const QColor OVERRIDE = QColor("#f92150");

    static QIcon createIconFromSvg(const QString& svgFilePath, const QColor& color, QSize size);
}

static QColor getStatusColour(int input) {
    switch (input) {
        case 0: return Constants::NO_STATUS;
        case 1: return Constants::IN_PROGRESS;
        case 2: return Constants::AT_TARGET;
        case 3: return Constants::OVERRIDE;
        default: return Constants::NO_STATUS;
    }
}
