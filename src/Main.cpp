#include <QApplication>
#include <QFile>
#include <QFontDatabase>
#include <QtCore/qdebug.h>
#include <QtCore/qsettings.h>
#include <QtCore/qtimer.h>
#include <QtGui/qfontdatabase.h>
#include "MainWindow.h"
#include "NetworkTables.h"

NetworkTables networkTables;

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("Niagara Robotics");
    QCoreApplication::setApplicationName("QuickStatus2");

    int fontId = QFontDatabase::addApplicationFont(":/fonts/B612-Bold");
    if (fontId == -1) {
        qDebug() << "Failed to load B612 font";
    }

    MainWindow mainWindow;
    QFile styleFile(":/qss/Dock.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text))
    {
        // qDebug() << "Resources found";
        QString styleSheet = QTextStream(&styleFile).readAll();
        app.setStyleSheet(styleSheet);
        styleFile.close();
    }

    mainWindow.show();
    mainWindow.setWindowTitle("QuickStatus2");

    NetworkTables::StartClient();

    return app.exec();
}