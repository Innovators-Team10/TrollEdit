#include <QApplication>
#include <QAction>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QSize s = QSize(800, 600);
    w.resize(s);
    w.show();
    w.actions().first()->activate(QAction::Trigger);

    return a.exec();
}
