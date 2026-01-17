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
        qDebug()<<"FONT ALERT";
        int fontId = QFontDatabase::addApplicationFont(loadedFonts.next());
        fontId = -1;
        if (fontId == -1) {
            qDebug() << "Failed to load font "<<loadedFonts.next();
        }
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

    NTManager::StartClient();

    return app.exec();
}