#include <QApplication>
#include <QFile>
#include <QFontDatabase>
#include <QDirIterator>

#include "MainWindow.h"
#include "NTManager.h"

NTManager networkTables;

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("Niagara Robotics");
    QCoreApplication::setApplicationName("QuickStatus2");

    QDirIterator loadedFonts(":/fonts", QDir::Files);
    while (loadedFonts.hasNext()) {
        QString fontPath = loadedFonts.next(); // advance once, store the path
        // qDebug() << "FONT ALERT";
        int fontId = QFontDatabase::addApplicationFont(fontPath);
        if (fontId == -1) {
            qDebug() << "Failed to load font " << fontPath;
        }
    }

    MainWindow mainWindow;
    QFile styleFile(":/qss/stylesheet.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text))
    {
        // qDebug() << "Resources found";
        QString styleSheet = QTextStream(&styleFile).readAll();
        app.setStyleSheet(styleSheet);
        styleFile.close();
    }

    mainWindow.show();
    mainWindow.setWindowTitle("QuickStatus2");

    NTManager::StartClient();

    return app.exec();
}