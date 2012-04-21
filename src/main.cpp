/**
* @file main.cpp
* @author Team 04 Ufopak + Team 10 Innovators
* @version 
* 
* @section DESCRIPTION
* Contains the defintion of class Main. Contains main function. Initialize Qt applicationin which editor is running.
*/

#include <QApplication>
#include "main_window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName("Innovators");
    app.setApplicationName("TrollEdit");
    app.setStartDragDistance(app.startDragDistance() * 2);

    // set splashScreen
    QPixmap pixmap(":/splash");
    QSplashScreen splashScreen(pixmap,Qt::WindowStaysOnTopHint);

    // find the directory of the program
    QFileInfo program(argv[0]);
    QString path = QApplication::applicationDirPath();

    MainWindow w(path);

    w.setWindowOpacity(0);

    splashScreen.show();
 
    w.setWindowIcon (QIcon(":/icon16"));
    w.show();

    QTimer::singleShot(2000, &splashScreen, SLOT(close()));
    QTimer::singleShot(1000, &w, SLOT(wInit()));
    // open all files given as parameters

    //    w.newFile();
    //    w.open("../input/in.c"); // TEMP
    
    // open all files given as parameters
    for (int i = 1; i < argc; i++)
        w.open(argv[i]);

    return app.exec();
}

void MainWindow::wInit()
 {
    setWindowOpacity(1);
 }
