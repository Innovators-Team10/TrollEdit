#include <QApplication>
#include <QAction>

#include "main_window.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // find the directory of the program
    QFileInfo program(argv[0]);
    QString path = program.absoluteDir().path();

    MainWindow w(path);
    QSize s = QSize(800, 600);
    w.resize(s);
    w.show();

    // activate "new file" action
    w.actions().first()->activate(QAction::Trigger);

    w.open("tests/in.c");

    // open all files given as parameters
    for (int i = 1; i < argc; i++)
        w.open(argv[i]);

    return a.exec();
}
