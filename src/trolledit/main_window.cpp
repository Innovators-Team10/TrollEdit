#include "main_window.h"
#include "ui_main_window.h"
#include "document_scene.h"
#include "language_manager.h"
#include "analyzer.h"

MainWindow::MainWindow(QString programPath, QWidget *parent) : QMainWindow(parent)
{
    langManager = new LanguageManager(programPath);
    QGraphicsView *view = new QGraphicsView();

    scene = new DocumentScene(this);
    scene->setHighlighting(langManager->getConfigData());

    view->setScene(scene);
    setCentralWidget(view);

    createActions();
    createMenus();
    createToolBars();
    statusBar();

    readSettings();

    updateRecentFileActions();
    QIcon icon = QIcon();
    icon.addFile(":/icon16.png");
    icon.addFile(":/icon32.png");
    setWindowIcon(icon);
}

void MainWindow::createActions()
{
    // new file
    newAction = new QAction(QIcon(":/new.png"), tr("&New"), this);
    newAction->setShortcut(tr("CTRL+N"));
    newAction->setToolTip(tr("Create a new file"));
    connect(newAction, SIGNAL(triggered()), this, SLOT(newFile()));
    addAction(newAction);

    // open
    openAction = new QAction(QIcon(":/open.png"), tr("&Open..."), this);
    openAction->setShortcut(tr("CTRL+O"));
    openAction->setToolTip(tr("Open an existing file"));
    openAction->setStatusTip(tr("Open an existing file"));                 // TODO - status msg for actions
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

    // save
    saveAction = new QAction(QIcon(":/save.png"), tr("&Save"), this);
    saveAction->setShortcut(tr("CTRL+S"));
    saveAction->setToolTip(tr("Save the file to disk"));
    connect(saveAction, SIGNAL(triggered()), scene, SLOT(saveGroup()));

    // save as
    saveAsAction = new QAction(QIcon(":/save-as.png"), tr("Save As..."), this);
    saveAsAction->setToolTip(tr("Save the file as..."));
    connect(saveAsAction, SIGNAL(triggered()), scene, SLOT(saveGroupAs()));

    // save all
    saveAllAction = new QAction(tr("Save All..."), this);
    saveAllAction->setToolTip(tr("Save all files"));
    connect(saveAllAction, SIGNAL(triggered()), scene, SLOT(saveAllGroups()));

    // close
    closeAction = new QAction(QIcon(":/close.png"), tr("&Close File"), this);
//    closeAction->setShortcut(tr("CTRL+P"));
    closeAction->setToolTip(tr("Close current file"));
    connect(closeAction, SIGNAL(triggered()), scene, SLOT(closeGroup()));

    // close all
    closeAllAction = new QAction(tr("&Close All"), this);
    closeAllAction->setToolTip(tr("Close all files"));
    connect(closeAllAction, SIGNAL(triggered()), scene, SLOT(closeAllGroups()));

    // print pdf
    printPdfAction = new QAction(QIcon(":/print.png"), tr("&Print PDF"), this);
    printPdfAction->setShortcut(tr("CTRL+P"));
    printPdfAction->setToolTip(tr("Print scene to PDF"));
    connect(printPdfAction, SIGNAL(triggered()), this, SLOT(printPdf()));

    // recent files
    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActions[i] = new QAction(this);
        recentFileActions[i]->setVisible(false);
        connect(recentFileActions[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }

    // exit
    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcut(tr("CTRL+Q"));
    exitAction->setToolTip(tr("Quit the application. Prompts to save files"));
    exitAction->setStatusTip(tr("Exit the application"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    // settings
    settingsAction = new QAction(QIcon(":/properties.png"), tr("Se&ttings"), this);
    settingsAction->setToolTip(tr("General application settings"));
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(settings()));

    // help
    helpAction = new QAction(tr("&Help"), this);
    helpAction->setToolTip(tr("Show application's help"));
    connect(helpAction, SIGNAL(triggered()), this, SLOT(help()));

    // about
    aboutAction = new QAction(tr("&About"), this);
    aboutAction->setToolTip(tr("Show application's about box"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    // about Qt
    aboutQtAction = new QAction(tr("About &Qt"), this);
    aboutQtAction->setStatusTip(tr("Show the Qt library’s About box"));
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    // show printable area
    printableAreaAction = new QAction(QIcon(":/area.png"), tr("Show Printable Area"), this);
    aboutAction->setToolTip(tr("Show margins of printable area"));
    connect(printableAreaAction, SIGNAL(triggered()), this, SLOT(showPrintableArea()));
    printableAreaAction->setCheckable(true);
}

void MainWindow::createMenus()
{
    // file menu
    fileMenu = menuBar()->addMenu(tr("&File"));
     fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addSeparator();
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addAction(saveAllAction);
    fileMenu->addSeparator();
    fileMenu->addAction(closeAction);
    fileMenu->addAction(closeAllAction);
    fileMenu->addSeparator();
    fileMenu->addAction(printableAreaAction);
    fileMenu->addAction(printPdfAction);
    separatorAction = fileMenu->addSeparator();
    for (int i = 0; i < MaxRecentFiles; ++i)
        fileMenu->addAction(recentFileActions[i]);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    // edit menu
    /* editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(cutAction);
    editMenu->addAction(copyAction);
    editMenu->addAction(pasteAction);
    editMenu->addAction(deleteAction);*/

    // options menu
    /* optionsMenu = menuBar()->addMenu(tr("&Options"));
    optionsMenu->addAction(stylesAction);*/

    // help menu
    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(helpAction);
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);
}

void MainWindow::createToolBars()
{
    // format toolbar
    formatToolBar = addToolBar(tr("Format"));
    formatToolBar->addAction(newAction);
    formatToolBar->addAction(openAction);
    formatToolBar->addAction(saveAction);
    formatToolBar->addAction(closeAction);
    formatToolBar->addSeparator();
    formatToolBar->addAction(printableAreaAction);
    formatToolBar->addAction(printPdfAction);
    formatToolBar->addAction(settingsAction);
//    formatToolBar->addAction();

}

void MainWindow::newFile()
{
    scene->newGroup(langManager->getAnalyzerFor("*"));
}

void MainWindow::open()
{
    QString fileFilters = tr("All files (*)");
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"), ".", fileFilters);
    open(fileName);
}

void MainWindow::open(QString fileName)
{
    if (!fileName.isEmpty() && QFile::exists(fileName)) {
        QSettings settings("Ufopak", "TrollEdit");
        QStringList files = settings.value("recentFileList").toStringList();
        files.removeAll(fileName);
        files.prepend(fileName);
        while (files.size() > MaxRecentFiles)
            files.removeLast();

        settings.setValue("recentFileList", files);

        updateRecentFileActions();
        load(fileName);
    }
}

void MainWindow::load(QString fileName)
{
    Analyzer *analyzer = langManager->getAnalyzerFor(QFileInfo(fileName).suffix());
    scene->loadGroup(fileName, analyzer);
}

//void MainWindow::save()
//{
//    scene->saveGroup();
//}
//
//void MainWindow::saveAs()
//{
//    scene->saveGroupAs("");
//}
//
//void MainWindow::saveAll()
//{
//    scene->saveAllGroups(currentFile);
//}

void MainWindow::printPdf()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Export PDF", QString(), "*.pdf");
    if (fileName.isEmpty())
        return;
    int resolution = 1200;
    QPrinter printer(QPrinter::HighResolution);
    //    printer.setOrientation(QPrinter::Landscape);
    printer.setPageSize(QPrinter::A4);
    //    printer.setFullPage(TRUE);
    printer.setResolution(resolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    QRectF rect;
    rect = QRectF(printer.pageRect());
    rect.setHeight(printer.pageRect().height() - (printer.paperRect().height() - printer.pageRect().height()));
    rect.setWidth(printer.pageRect().width() - (printer.paperRect().width() - printer.pageRect().width()));

    int y = 0;
    int h = 1200;
    int w = 802;
    QRectF rect2;
    rect2 = QRectF(0, y, w, h);


    //    QColor color;
    //    color.setBlue(250);
    QPainter painter( &printer );
    //    scene->setSceneRect(0, 0, 1600, 2000);

    if(printableAreaAction->isChecked())
        hideArea();
    for(int i=0; i<10; i++){
        scene->render(&painter, rect, rect2, Qt::KeepAspectRatio);
        y+=1200;
        if(y < scene->sceneRect().height())
        {
            rect2.setRect(0, y, w, h);
            printer.newPage();
        }
        else
            break;
    }
    if(printableAreaAction->isChecked())
        showArea();
}

void MainWindow::showPrintableArea()
{
    QColor color;
    color.setBlue(255);
    color.setGreen(150);
    QPen pen(color, 2, Qt::DashDotDotLine);

    int pagelength = 1200;
    int endpage = pagelength;

    if(printableAreaAction->isChecked())
    {
        line = new QGraphicsLineItem(0);
        line->setLine(802, 0, 802, scene->sceneRect().height());
        line->setVisible(true);
        line->setPen(pen);
        line->setZValue(-50);
        list.append(line);

        while(endpage < scene->sceneRect().height())
        {
            line = new QGraphicsLineItem(0);
            line->setLine(0, endpage, 802, endpage);
            line->setVisible(true);
            line->setPen(pen);
            line->setZValue(-50);
            list.append(line);
            endpage += pagelength;
        }
        showArea();
    }

    else
    {
        hideArea();
    }
}

void MainWindow::showArea()
{
    for(int i=0; i<list.size(); i++)
        scene->addItem(list.at(i));
}

void MainWindow::hideArea()
{
    for(int i=0; i<list.size(); i++)
        scene->removeItem(list.at(i));
}


void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        open(action->data().toString());
    }
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About TrollText"),
                       tr("<h2>TrollEdit 1.0</h2>"
                          "<p/>Team 5 - UFOPAK"
                          "<p/>This is just a prototype of text editor"
                          "which is being developed for Team project course."));
}

void MainWindow::updateRecentFileActions()
{
    QSettings settings("Ufopak", "TrollEdit");
    QStringList files = settings.value("recentFileList").toStringList();

    //    QMutableStringListIterator i(files);
    //    while (i.hasNext()) {
    //        if (!QFile::exists(i.next()))
    //            i.remove();
    //    }

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
        recentFileActions[i]->setText(text);
        recentFileActions[i]->setData(files[i]);
        recentFileActions[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentFileActions[j]->setVisible(false);

    separatorAction->setVisible(numRecentFiles > 0);
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::help()
{
    //HelpBrowser::showPage("index.html");
}

void MainWindow::settings()
{

}

void MainWindow::closeEvent(QCloseEvent *event)
{
//    if (maybeSave()) {
        writeSettings();
        event->accept();
//    } else {
//        event->ignore();
//    }
}

void MainWindow::readSettings()
{
    QSettings settings("Ufopak", "TrollEdit");
    QPoint pos = settings.value("pos", QPoint(100, 100)).toPoint();
    QSize size = settings.value("size", QSize(850, 700)).toSize();
    resize(size);
    move(pos);
}

void MainWindow::writeSettings()
{
    QSettings settings("Ufopak", "TrollEdit");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}
