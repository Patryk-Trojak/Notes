#include "MainWindow.h"
#include "PersistenceManager.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PersistenceManager::notesDirectoryPathname = QCoreApplication::applicationDirPath() + QString("/notesDir/");
    MainWindow w;
    w.show();
    return a.exec();
}
