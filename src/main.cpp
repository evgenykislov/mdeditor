#include <QApplication>
#include <QCoreApplication>
#include <QFileInfo>
#include <QStringList>

#include "gui/main_window.h"

int main(int argc, char* argv[]) {
    // The window looks the same on screens with a high density of pixels.
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    // The names tell the settings of the program where they belong.
    QCoreApplication::setOrganizationName(QStringLiteral("mdeditor"));
    QCoreApplication::setApplicationName(QStringLiteral("mdeditor"));
    QCoreApplication::setApplicationVersion(QStringLiteral(MDEDITOR_VERSION));

    QApplication application(argc, argv);

    mdeditor::MainWindow window;
    // The file that is named on the command line is opened: the program can
    // be made the editor of the Markdown files of the system.
    const QStringList arguments = QCoreApplication::arguments();
    if (arguments.size() > 1) window.openFile(QFileInfo(arguments.at(1)).absoluteFilePath());
    window.show();
    return application.exec();
}
