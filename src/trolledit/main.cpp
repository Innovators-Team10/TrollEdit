#include <QApplication>
#include <QAction>
#include "main_window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName("Ufopak");
    app.setApplicationName("TrollEdit");
    app.setStartDragDistance(app.startDragDistance() * 2);

    // find the directory of the program
    QFileInfo program(argv[0]);
    QString path = program.absoluteDir().path();

    MainWindow w(path);
    w.show();

    // activate "new file" action
    w.actions().first()->activate(QAction::Trigger);

    w.open("tests/in.c"); // TEMP

    // open all files given as parameters
    for (int i = 1; i < argc; i++)
        w.open(argv[i]);

    return app.exec();
}
