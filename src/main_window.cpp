#include "main_window.h"
#include "ui_main_window.h"
#include "document_scene.h"
#include "language_manager.h"
#include "analyzer.h"
#include "block_group.h"

MainWindow::MainWindow(QString programPath, QWidget *parent) : QMainWindow(parent)
{
    langManager = new LanguageManager(programPath);
    QGraphicsView *view = new QGraphicsView();

    scene = new DocumentScene(this);
    scene->setHighlighting(langManager->getConfigData());
    connect(scene, SIGNAL(modified(bool)), this, SLOT(setModified(bool)));
    connect(scene, SIGNAL(fileSelected(BlockGroup*)),
            this, SLOT(setCurrentFile(BlockGroup*)));

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
    setCurrentFile(0);

    setStyleSheet(
                "QMainWindow { "
                "color: white;"
                "background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #4e3d3d, stop:1 #271b1b);"
                "border-width: 3px;"
                "border-color: #002447;"
                "border-style: solid;"
                "border-radius: 7;"
                "padding: 3px;"
                "font-size: 10px;"
                "padding-left: 5px;"
                "padding-right: 5px;"
                "}"

                "QMenu {"
                "background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #442c2c, stop:1 #4b2929);"
                "border: 1px solid black;"
                "}"

                "QMenu::item {"
                "background-color: transparent;"
                "}"

                "QMenu::item:selected {"
                "background-color: #271b1b;"
                "}"

                "QMenuBar {"
                "background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #442c2c, stop:1 #4b2929);"
                "color: white;"
                "}"

                "QMenuBar::item {"
                "spacing: 3px;"
                "padding: 1px 4px;"
                "background: transparent;"
                "border-radius: 4px;"
                "}"

                "QMenuBar::item:selected {"
                "background: #a8a8a8;"
                "}"

                "QMenuBar::item:pressed {"
                "background: #888888;"
                "}"

                "QToolBar {"
                "min-height: 40px;"
                "}"

                "QToolButton {"
                "color: #333;"
                "border: 2px solid #555;"
                "border-radius: 5px;"
                "padding: 2px;"
                "background: qradialgradient(cx: 0.3, cy: -0.4,"
                "fx: 0.3, fy: -0.4,"
                "radius: 1.35, stop: 0 #fff, stop: 1 #888);"
                "min-width: 40px;"
                "}"

                "QToolButton:hover {"
                "background: qradialgradient(cx: 0.3, cy: -0.4,"
                "fx: 0.3, fy: -0.4,"
                "radius: 1.35, stop: 0 #fff, stop: 1 #bbb);"
                "}"

                "QToolButton:pressed {"
                "background: qradialgradient(cx: 0.4, cy: -0.1,"
                "fx: 0.4, fy: -0.1,"
                "radius: 1.35, stop: 0 #fff, stop: 1 #ddd);"
                "}"

                "QStatusBar{ "
                "color: white;"
                "}"

                );

}

void MainWindow::createActions()
{
    groupActions = new QActionGroup(this);

    // new
    QIcon newIcon(":/m/new"); newIcon.addFile(":/s/new");
    newAction = new QAction(newIcon, tr("&New"), this);
    newAction->setShortcut(tr("CTRL+N"));
    newAction->setToolTip(tr("Create a new file"));
    connect(newAction, SIGNAL(triggered()), this, SLOT(newFile()));
    addAction(newAction);

    // open
    QIcon openIcon(":/m/open"); openIcon.addFile(":/s/open");
    openAction = new QAction(openIcon, tr("&Open..."), this);
    openAction->setShortcut(tr("CTRL+O"));
    openAction->setToolTip(tr("Open an existing file"));
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

    // revert
//    QIcon revertIcon(":/m/open"); openIcon.addFile(":/s/open");
    revertAction = new QAction(tr("&Revert"), this);
    revertAction->setShortcut(tr("CTRL+R"));
    revertAction->setToolTip(tr("Revert to last save"));
    connect(revertAction, SIGNAL(triggered()), scene, SLOT(revertGroup()));
    groupActions->addAction(revertAction);

    // save
    QIcon saveIcon(":/m/save"); saveIcon.addFile(":/s/save");
    saveAction = new QAction(saveIcon, tr("&Save"), this);
    saveAction->setShortcut(tr("CTRL+S"));
    saveAction->setToolTip(tr("Save file"));
    connect(saveAction, SIGNAL(triggered()), scene, SLOT(saveGroup()));
    groupActions->addAction(saveAction);

    // save as
    QIcon saveAsIcon(":/m/save-as"); saveAsIcon.addFile(":/s/save-as");
    saveAsAction = new QAction(saveAsIcon, tr("Save &As..."), this);
    saveAsAction->setToolTip(tr("Save file as..."));
    connect(saveAsAction, SIGNAL(triggered()), scene, SLOT(saveGroupAs()));
    groupActions->addAction(saveAsAction);

    // save as
//    QIcon saveAsNoDocIcon(":/m/save-as"); saveAsIcon.addFile(":/s/save-as");
    saveAsNoDocAction = new QAction(tr("Save Without Comments"), this);
    saveAsNoDocAction->setToolTip(tr("Save file without any comments"));
    connect(saveAsNoDocAction, SIGNAL(triggered()), scene, SLOT(saveGroupAsWithoutDoc()));
    groupActions->addAction(saveAsNoDocAction);

    // save all
    saveAllAction = new QAction(tr("Save All"), this);
    saveAllAction->setToolTip(tr("Save all files"));
    connect(saveAllAction, SIGNAL(triggered()), scene, SLOT(saveAllGroups()));

    // close
    QIcon closeIcon(":/m/close"); closeIcon.addFile(":/s/close");
    closeAction = new QAction(closeIcon, tr("&Close File"), this);
    closeAction->setShortcut(tr("CTRL+Q"));
    closeAction->setToolTip(tr("Close file"));
    connect(closeAction, SIGNAL(triggered()), scene, SLOT(closeGroup()));
    groupActions->addAction(closeAction);

    // close all
    closeAllAction = new QAction(tr("Close All"), this);
    closeAllAction->setToolTip(tr("Close all files"));
    connect(closeAllAction, SIGNAL(triggered()), scene, SLOT(closeAllGroups()));

    // print pdf
    QIcon printIcon(":/m/print"); printIcon.addFile(":/s/print");
    printPdfAction = new QAction(printIcon, tr("&Print PDF"), this);
    printPdfAction->setShortcut(tr("CTRL+P"));
    printPdfAction->setToolTip(tr("Print scene to PDF"));
    connect(printPdfAction, SIGNAL(triggered()), this, SLOT(printPdf()));
	groupActions->addAction(printPdfAction);

    // show plain text editor
    QIcon editIcon(":/m/edit"); printIcon.addFile(":/s/edit");
    plainEditAction = new QAction(editIcon, tr("&Edit Plain Text"), this);
    plainEditAction->setShortcut(tr("CTRL+E"));
    plainEditAction->setToolTip(tr("Edit file as plain text"));
    connect(plainEditAction, SIGNAL(triggered()), scene, SLOT(showPreview()));
    groupActions->addAction(plainEditAction);

    // clear search results
    QIcon clearIcon(":/m/clear"); saveIcon.addFile(":/s/clear");
    clearAction = new QAction(clearIcon, tr("Clea&n Search"), this);
    clearAction->icon().addFile(":/m/save.png");
//    clearAction->setShortcut(tr("CTRL+S"));
    clearAction->setToolTip(tr("Clean search results"));
    connect(clearAction, SIGNAL(triggered()), scene, SLOT(cleanGroup()));
    groupActions->addAction(clearAction);

    // recent files
    for (int i = 0; i < MaxRecentFiles; ++i)
    {
        recentFileActions[i] = new QAction(this);
        recentFileActions[i]->setVisible(false);
        connect(recentFileActions[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }

    // exit
    QIcon exitIcon(":/s/exit");
    exitAction = new QAction(exitIcon, tr("E&xit"), this);
    exitAction->setToolTip(tr("Quit the application."));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    // settings
    QIcon settingsIcon(":/m/settings"); settingsIcon.addFile(":/s/settings");
    settingsAction = new QAction(settingsIcon, tr("Se&ttings"), this);
    settingsAction->setToolTip(tr("General application settings"));
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(settings()));

    // help
    QIcon helpIcon(":/m/help"); helpIcon.addFile(":/s/help");
    helpAction = new QAction(helpIcon, tr("&Help"), this);
    helpAction->setShortcut(tr("F1"));
    helpAction->setToolTip(tr("Show application help"));
    connect(helpAction, SIGNAL(triggered()), this, SLOT(help()));

    // about
    aboutAction = new QAction(tr("&About"), this);
    aboutAction->setToolTip(tr("Show application's about box"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    // about Qt
    aboutQtAction = new QAction(tr("About &Qt"), this);
    aboutQtAction->setStatusTip(tr("Show the Qt library�s About box"));
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    // show printable area
    QIcon areaIcon(":/m/area"); areaIcon.addFile(":/s/area");
    printableAreaAction = new QAction(areaIcon, tr("Show Printable Area"), this);
    printableAreaAction->setToolTip(tr("Show margins of printable area"));
    connect(printableAreaAction, SIGNAL(triggered()), this, SLOT(showPrintableArea()));
    printableAreaAction->setCheckable(true);
}

void MainWindow::createMenus()
{
    // file menu
    fileMenu = menuBar()->addMenu(tr("&File"));
     fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(revertAction);
    fileMenu->addSeparator();
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addAction(saveAsNoDocAction);
    fileMenu->addAction(saveAllAction);
    fileMenu->addSeparator();
    fileMenu->addAction(closeAction);
    fileMenu->addAction(closeAllAction);
    fileMenu->addSeparator();
    fileMenu->addAction(plainEditAction);
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

    formatToolBar->addAction(plainEditAction);
    formatToolBar->addAction(printableAreaAction);
    formatToolBar->addAction(printPdfAction);
    //formatToolBar->addAction(settingsAction);
    formatToolBar->addSeparator();

    scriptsBox = new QComboBox();
    scriptsBox->setMaxVisibleItems(10);
    scriptsBox->addItems(langManager->getLanguages());
    connect(scriptsBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(langChanged(QString)));
    formatToolBar->addWidget(scriptsBox);
    formatToolBar->addSeparator();
	
    searchLabel = new QLabel();
//    searchLabel->setText(" Search ");
    searchLabel->setPixmap(QPixmap(":/m/search"));
    formatToolBar->addWidget(searchLabel);

    searchLineEdit = new QLineEdit();
    searchLineEdit->setFixedSize(200, 20);
    connect(searchLineEdit, SIGNAL(editingFinished()), this, SLOT(search()));
    formatToolBar->addWidget(searchLineEdit);

    formatToolBar->addAction(clearAction);
}

void MainWindow::setModified(bool flag)
{
    setWindowModified(flag);
    saveAction->setEnabled(flag);
    revertAction->setEnabled(flag);
}

void MainWindow::setCurrentFile(BlockGroup *group)
{
    QString fileName;
    QString lang;

    if (group != 0)
    {
        fileName = group->getFilePath();
        lang = group->getAnalyzer()->getLanguageName();
        selectedGroup = group;
    }
    else
    {
        lang = "";
        fileName = "Empty";
        selectedGroup = 0;
    }

    if (fileName.isEmpty() || fileName == "Empty")
    {
        setWindowFilePath(fileName);
        groupActions->setEnabled(false);
        searchLineEdit->setEnabled(false);
    }
    else
    {
        groupActions->setEnabled(true);
        searchLineEdit->setEnabled(true);

        if (scriptsBox->currentText() != lang)
        {
            int index = scriptsBox->findText(lang, Qt::MatchFixedString);
            scriptsBox->blockSignals(true);
            scriptsBox->setCurrentIndex(index);
            scriptsBox->blockSignals(false);
        }
        if (windowFilePath() != fileName)
        {
            setWindowFilePath(fileName);

            if (!QFileInfo(fileName).fileName().isEmpty())
            {
                fileName = QFileInfo(fileName).fileName();
            }
            else
            {
                revertAction->setEnabled(false);
            }

            saveAction->setText(tr("&Save \"%1\"").arg(fileName));
            saveAsAction->setText(tr("Save \"%1\" &As...").arg(fileName));
            closeAction->setText(tr("&Close \"%1\"").arg(fileName));
        }
    }
}

void MainWindow::newFile()
{
    scene->newGroup(langManager->getAnalyzerForLang(scriptsBox->currentText()));
}

void MainWindow::open()
{
    QString fileFilters = tr("All files (*)");
    QString dir = QFileInfo(windowFilePath()).absoluteDir().absolutePath();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"), dir, fileFilters);
    open(fileName);
}

void MainWindow::open(QString fileName)
{
    if (!fileName.isEmpty() && QFile::exists(fileName))
    {
        QSettings settings(QApplication::organizationName(), QApplication::applicationName());
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

void MainWindow::langChanged(QString newLang)
{
    scene->setGroupLang(langManager->getAnalyzerForLang(newLang));
}

void MainWindow::search()
{
    QString searchText = searchLineEdit->text();
    scene->findText(searchText);
    
}

void MainWindow::printPdf()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Export PDF", QString(), "*.pdf");

    if (fileName.isEmpty()) return;

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

    int endCondition;

    if (selectedGroup == 0)
    {
        startPoint = QPointF();
        endCondition = scene->sceneRect().height();
    }
    else
    {
        startPoint = selectedGroup->pos();
        endCondition = startPoint.y() + selectedGroup->rect().height();
    }

    int x = startPoint.x() - 30;
    int y = startPoint.y() - 30;
    int h = 1200;
    int w = 802;
    QRectF rect2;
    rect2 = QRectF(x, y, w, h);

    //    QColor color;
    //    color.setBlue(250);
    QPainter painter( &printer );
    //    scene->setSceneRect(0, 0, 1600, 2000);

    if(printableAreaAction->isChecked()) hideArea();

//    for(int i=0; i<10; i++){
    while(endCondition)
    {
        scene->render(&painter, rect, rect2, Qt::KeepAspectRatio);
        y+=1200;

        if (y < endCondition)
        {
            rect2.setRect(x, y, w, h);
            printer.newPage();
        }
        else
            endCondition = 0;
    }

    if(printableAreaAction->isChecked())
        showArea();

    statusBar()->showMessage("Pdf export finished", 2000);
}

void MainWindow::showPrintableArea()
{
    if (selectedGroup == 0)
    {
        startPoint = QPointF();
    }
    else
    {
        startPoint = selectedGroup->pos();
    }

    QColor color;
    color.setBlue(255);
    color.setGreen(150);
    QPen pen(color, 2, Qt::DashDotDotLine);

    int pagelength = 1200;
    int endpage = 0;

    if(printableAreaAction->isChecked())
    {
        line = new QGraphicsLineItem(0);
        line->setLine(startPoint.x() - 30, startPoint.y() - 30, startPoint.x() - 30, scene->sceneRect().height());
        line->setVisible(true);
        line->setPen(pen);
        line->setZValue(50);
        list.append(line);

        line = new QGraphicsLineItem(0);
        line->setLine(startPoint.x() + 802 -30, startPoint.y() - 30, startPoint.x() + 802 - 30, scene->sceneRect().height());
        line->setVisible(true);
        line->setPen(pen);
        line->setZValue(50);
        list.append(line);

        while(endpage < scene->sceneRect().height())
        {
            line = new QGraphicsLineItem(0);
            line->setLine(startPoint.x() - 30, startPoint.y() + endpage - 30, startPoint.x() + 802 - 30, startPoint.y() + endpage - 30);
            line->setVisible(true);
            line->setPen(pen);
            line->setZValue(50);
            list.append(line);
            endpage += pagelength;
        }

        showArea();
    }
    else
    {
        hideArea();
        list.clear();
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

    if (action)
    {
        open(action->data().toString());
    }
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About TrollEdit"),
                       tr("<h2>TrollEdit 1.0</h2>"
                          "<p/>Team 5 - Ufopak"
                          "<p/>This is a prototype of novel text editor "
                          "which is being developed for Team project course."));
}

void MainWindow::updateRecentFileActions()
{
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i)
    {
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
    QDesktopServices::openUrl(QUrl(QApplication::applicationDirPath()+"//doc//index.html"));
}

void MainWindow::settings()
{

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    scene->closeAllGroups();
    writeSettings();
    event->accept();
}

void MainWindow::readSettings()
{
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    QPoint pos = settings.value("pos", QPoint(100, 100)).toPoint();
    QSize size = settings.value("size", QSize(850, 700)).toSize();
    resize(size);
    move(pos);

    if (settings.value("maximized", false).toBool())
        showMaximized();
}

void MainWindow::writeSettings()
{
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("maximized", isMaximized());
}
