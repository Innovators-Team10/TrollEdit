#include "main_window.h"
#include "ui_main_window.h"
#include "document_scene.h"
#include "language_manager.h"
#include "analyzer.h"
#include "block_group.h"
#include <QTableWidget>
#include <QFont>
#include <QPushButton>
#include <QtGui>
MainWindow::MainWindow(QString programPath, QWidget *parent) : QMainWindow(parent)
{
    langManager = new LanguageManager(programPath);

    createActions();
    createTabs();

    readSettings();

    updateRecentFileActions();
    QIcon icon = QIcon();
    icon.addFile(":/icon16.png");
    icon.addFile(":/icon32.png");
    setWindowIcon(icon);

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

                "QTabWidget::pane {"
                "border-top: 0px;"
                "}"

                "QPushButton{"
                "color: white;"
                "background: qradialgradient(cx: 0.4, cy: -0.1,"
                "fx: 0.4, fy: -0.1,"
                "radius: 1.35, stop: 0 #777, stop: 1 #333);"
                "}"

                "QTabBar{"
                "color: white;"
                "background: qradialgradient(cx: 0.4, cy: -0.1,"
                "fx: 0.4, fy: -0.1,"
                "radius: 1.35, stop: 0 #777, stop: 1 #333);"
                "}"

                "QTabBar::tab:selected {"
                "color: black;"
                "background: qradialgradient(cx: 0.3, cy: -0.4,"
                "fx: 0.3, fy: -0.4,"
                "radius: 1.35, stop: 0 #fff, stop: 1 #ddd);"
                "}"

                "QMenu {"
                "background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #442c2c, stop:1 #4b2929);"
                "border: 1px solid black;"
                "}"

                "QMenu::item {"
                "background-color: transparent;"
                "color: white;"
                "}"

                "QMenu::item:selected {"
                "background-color: #271b1b;"
                "}"

                "QMenu::item:disabled {"
                "background-color: #271b1b;"
                "color:gray;"
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

                "QMenuBar::item:disabled {"
                "background: #000000;"
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
                "radius: 1.35, stop: 0 #fff, stop: 1 #ddd);"
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

                "QToolButton:disabled {"
                "background: qradialgradient(cx: 0.4, cy: -0.1,"
                "fx: 0.4, fy: -0.1,"
                "radius: 1.35, stop: 0 #777, stop: 1 #333);"
                "}"

                "QStatusBar{ "
                "color: white;"
                "}"

                );
}

void MainWindow::createActions()
{
    groupActions = new QActionGroup(this);

        QFile file("shortcuts.ini");

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::information(0,"error",file.errorString());
    }
    QString textstring;
    //QString textstring = file.readLine();

    // new
    QIcon newIcon(":/m/new"); newIcon.addFile(":/s/new"); // works
    newAction = new QAction(newIcon, tr("&New"), this);
    textstring = file.readLine();
    textstring.remove(6,1);
    newAction->setShortcut((textstring));
    newAction->setToolTip(tr("Create a new file"));
    connect(newAction, SIGNAL(triggered()), this, SLOT(newFile()));
    addAction(newAction);

    // open
    QIcon openIcon(":/m/open"); openIcon.addFile(":/s/open"); // works
    openAction = new QAction(openIcon, tr("&Open..."), this);
    textstring = file.readLine();
    textstring.remove(6,1);
    openAction->setShortcut((textstring));
    openAction->setToolTip(tr("Open an existing file"));
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

    // revert
    revertAction = new QAction(tr("&Revert"), this); // ??? is this used ???
        textstring = file.readLine();
//    QIcon revertIcon(":/m/open"); openIcon.addFile(":/s/open");
    revertAction = new QAction(tr("&Revert"), this);
        textstring = file.readLine();
    textstring.remove(6,1);
    revertAction->setShortcut((textstring));
    revertAction->setToolTip(tr("Revert to last save"));
    connect(revertAction, SIGNAL(triggered()), this, SLOT(revertGroupWraper()));
    groupActions->addAction(revertAction);

    // save
    QIcon saveIcon(":/m/save"); saveIcon.addFile(":/s/save"); // works (only for 1 scene)
    saveAction = new QAction(saveIcon, tr("&Save"), this);
    textstring = file.readLine();
    textstring.remove(6,1);
    saveAction->setShortcut((textstring));
    saveAction->setToolTip(tr("Save file"));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveGroupWrapper()));
    groupActions->addAction(saveAction);

    // save as
    QIcon saveAsIcon(":/m/save-as"); saveAsIcon.addFile(":/s/save-as"); // probably same as saveAction
    saveAsAction = new QAction(saveAsIcon, tr("Save &As..."), this);
    saveAsAction->setToolTip(tr("Save file as..."));
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveGroupAsWrapper()));
    groupActions->addAction(saveAsAction);

    // save as
//    QIcon saveAsNoDocIcon(":/m/save-as"); saveAsIcon.addFile(":/s/save-as");
    saveAsNoDocAction = new QAction(tr("Save Without Comments"), this); // ??? is this used ???
    saveAsNoDocAction->setToolTip(tr("Save file without any comments"));
    connect(saveAsNoDocAction, SIGNAL(triggered()), this, SLOT(saveGroupAsWithoutDocWrapper()));
    groupActions->addAction(saveAsNoDocAction);

    // save all
    saveAllAction = new QAction(tr("Save All"), this); // works for 1 tab ??? does saveAllGroups work as it should ???
    saveAllAction->setToolTip(tr("Save all files"));
    connect(saveAllAction, SIGNAL(triggered()), this, SLOT(saveAllGroupsWrapper()));

    // close
    QIcon closeIcon(":/m/close"); closeIcon.addFile(":/s/close"); // works
    closeAction = new QAction(closeIcon, tr("&Close File"), this);
    textstring = file.readLine();
    textstring.remove(6,1);
    closeAction->setShortcut((textstring));
    closeAction->setToolTip(tr("Close file"));
    connect(closeAction, SIGNAL(triggered()), this, SLOT(closeGroupWrapper()));
    groupActions->addAction(closeAction);

    // close all
    closeAllAction = new QAction(tr("Close All"), this);
    closeAllAction->setToolTip(tr("Close all files"));
    connect(closeAllAction, SIGNAL(triggered()), this, SLOT(closeAllGroupsWrapper()));

    // print pdf
    QIcon printIcon(":/m/print"); printIcon.addFile(":/s/print");
    printPdfAction = new QAction(printIcon, tr("&Print PDF"), this);
    textstring = file.readLine();
    textstring.remove(6,1);
    printPdfAction->setShortcut((textstring));
    printPdfAction->setToolTip(tr("Print scene to PDF"));
    connect(printPdfAction, SIGNAL(triggered()), this, SLOT(printPdf()));
        groupActions->addAction(printPdfAction);

    // show plain text editor
    QIcon editIcon(":/m/edit"); printIcon.addFile(":/s/edit");
    plainEditAction = new QAction(editIcon, tr("&Edit Plain Text"), this);
    textstring = file.readLine();
    textstring.remove(6,1);
    plainEditAction->setShortcut((textstring));
    plainEditAction->setToolTip(tr("Edit file as plain text"));
    connect(plainEditAction, SIGNAL(triggered()), this, SLOT(showPreviewWrapper()));
    groupActions->addAction(plainEditAction);

    // clear search results
    QIcon clearIcon(":/m/clear"); saveIcon.addFile(":/s/clear");
    clearAction = new QAction(clearIcon, tr("Clea&n Search"), this);
    clearAction->icon().addFile(":/m/save.png");
//    clearAction->setShortcut(tr("CTRL+S"));
    clearAction->setToolTip(tr("Clean search results"));
    connect(clearAction, SIGNAL(triggered()), this, SLOT(cleanGroupWrapper()));
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
    aboutQtAction->setStatusTip(tr("Show the Qt library?s About box"));
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    // shortcuts
    shortAction = new QAction(tr("Shortcuts"), this);
    shortAction->setStatusTip(tr("Setting shortcuts"));
    connect(shortAction, SIGNAL(triggered()), this, SLOT(setShort()));
    // show printable area
    QIcon areaIcon(":/m/area"); areaIcon.addFile(":/s/area");
    printableAreaAction = new QAction(areaIcon, tr("Show Printable Area"), this);
    printableAreaAction->setToolTip(tr("Show margins of printable area"));
    connect(printableAreaAction, SIGNAL(triggered()), this, SLOT(showPrintableArea()));
    printableAreaAction->setCheckable(true);
        file.close();
}

// wrapper slots
// in most functions is dynamically detected current BlockGroup,
// so they are called with parameter 0
void MainWindow::closeGroupWrapper(){
    getScene()->closeGroup(getScene()->selectedGroup());
}

void MainWindow::revertGroupWrapper(){
    getScene()->revertGroup(getScene()->selectedGroup());
}

void MainWindow::saveGroupWrapper(){
    getScene()->saveGroup(getScene()->selectedGroup()->getFilePath(),0,false);
}

void MainWindow::saveGroupAsWrapper(){
    getScene()->saveGroupAs(0);
}

void MainWindow::saveAllGroupsWrapper(){
    getScene()->saveAllGroups();
}

void MainWindow::saveGroupAsWithoutDocWrapper(){
    getScene()->saveGroupAsWithoutDoc(0);
}

void MainWindow::closeAllGroupsWrapper(){
    getScene()->closeAllGroups();
}

void MainWindow::showPreviewWrapper(){
    getScene()->selectedGroup()->changeMode();
//    getScene()->showPreview(0); // povodny edit plain text, zobrazi okno s plain textom
}

void MainWindow::cleanGroupWrapper(){
    getScene()->cleanGroup(0);
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
    optionsMenu = menuBar()->addMenu(tr("&Options"));
    optionsMenu->addAction(shortAction);

    // help menu
    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(helpAction);
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);
}

void MainWindow::setShort()
{
    set_shortcuts = new QDialog();
    QPushButton *Savebutton = new QPushButton("OK", set_shortcuts);
    QPushButton *Closebutton = new QPushButton("Close", set_shortcuts);

    m_table = new QTableWidget(7, 2, set_shortcuts);

    m_table->setHorizontalHeaderItem(0, new QTableWidgetItem("Funkcia"));
    m_table->setHorizontalHeaderItem(1, new QTableWidgetItem("Skratka"));
    m_table->setItem(0,0, new QTableWidgetItem("New"));
    m_table->setItem(1,0, new QTableWidgetItem("Open"));
    m_table->setItem(2,0, new QTableWidgetItem("Revert"));
    m_table->setItem(3,0, new QTableWidgetItem("Save"));
    m_table->setItem(4,0, new QTableWidgetItem("Close"));
    m_table->setItem(5,0, new QTableWidgetItem("Print"));
    m_table->setItem(6,0, new QTableWidgetItem("Edit plain text"));


    QFile file("shortcuts.ini");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::information(0,"error",file.errorString());
    }

    QString textstring;
    for(int row=0; row < m_table->rowCount(); row++)
    {
        QString textstring = file.readLine();
        textstring.remove(6,1);
        m_table->setItem(row,1, new QTableWidgetItem(textstring));
    }
    file.close();
    //m_table->resizeColumnsToContents();
    //m_table->resizeRowsToContents();
    m_table->resize(220,240);

    set_shortcuts->resize(225,290);
    Savebutton->move(20,250);
    Closebutton->move(130,250);
    set_shortcuts->show();


    QObject::connect(Savebutton, SIGNAL(clicked()),this,SLOT(savedShortcuts()));
    QObject::connect(Closebutton, SIGNAL(clicked()),this,SLOT(closeShortcuts()));
}

void MainWindow::savedShortcuts()
{
    QFile file("shortcuts.ini");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::information(0,"error",file.errorString());
    }
    QString textstring;

    for(int row=0; row < m_table->rowCount(); row++)
    {
         textstring = m_table->item(row,1)->text();
         QTextStream out(&file);
         out << textstring << endl;
    }
    file.close();
    textstring = m_table->item(0,1)->text();
    newAction->setShortcut((textstring));
    textstring = m_table->item(1,1)->text();
    openAction->setShortcut((textstring));
    textstring = m_table->item(2,1)->text();
    revertAction->setShortcut((textstring));
    textstring = m_table->item(3,1)->text();
    saveAction->setShortcut((textstring));
    textstring = m_table->item(4,1)->text();
    closeAction->setShortcut((textstring));
    textstring = m_table->item(5,1)->text();
    printPdfAction->setShortcut((textstring));
    textstring = m_table->item(6,1)->text();
    plainEditAction->setShortcut((textstring));
    set_shortcuts->close();
}

    void MainWindow::closeShortcuts()
{
    set_shortcuts->close();
    }

void MainWindow::createToolBars()
{
    // format toolbar

    formatToolBar = addToolBar(tr("Format"));
  //  formatToolBar->setFloatable(false);
  //  formatToolBar->setMovable(false);
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

QGraphicsView* MainWindow::createView()
{
    QGraphicsView *view = new QGraphicsView();

    DocumentScene *scene = new DocumentScene(this);
    scene->setHighlighting(langManager->getConfigData());
    connect(scene, SIGNAL(modified(bool)), this, SLOT(setModified(bool)));
    connect(scene, SIGNAL(fileSelected(BlockGroup*)),
            this, SLOT(setCurrentFile(BlockGroup*))); // CHECK

    view->setScene(scene);
    return view;
}

DocumentScene* MainWindow::getScene()
{
    QGraphicsView* view=(QGraphicsView *) tabWidget->currentWidget();
    DocumentScene* dScene=(DocumentScene *) view->scene();
    return dScene;
}

void MainWindow::newTab()
{
    qDebug("newTab()");
    int count=tabWidget->count();
    QString* name=new QString("tab");
    QString* numb=new QString("");
    numb->setNum(count);
    name->append(numb);
    QWidget* widget=createView(); // get QGraphicView

    QGraphicsView* view=(QGraphicsView *) widget;
    DocumentScene* dScene=(DocumentScene *) view->scene();
    dScene->main=this;

    tabWidget->addTab(widget, *name);
    tabWidget->setCurrentWidget(widget); // focus on new tab
    return;
}

void MainWindow::newFile()
{
    qDebug("newFile()");
    DocumentScene* dScene=getScene();
    if(dScene==0){ // this should not ever happen
        qDebug("newFile() Error: dScene = null");
        return;
    }else{
        dScene->newGroup(scriptsBox->currentText());
        setCurrentFile(dScene->selectedGroup());
        scriptsBox->setCurrentIndex(0); // select item in scriptsBox on C (in future select previously used item etc...)
    }
}

LanguageManager* MainWindow::getLangManager(){
    return this->langManager;
}

QComboBox* MainWindow::getScriptBox(){
    return this->scriptsBox;
}

void MainWindow::load(QString fileName)
{
    Analyzer *analyzer = langManager->getAnalyzerFor(QFileInfo(fileName).suffix());
    DocumentScene* dScene=getScene();
    if(dScene==0){ // this should not ever happen
        qDebug("load() Error: dScene = null");
        return;
    }else{
        qDebug() << "load() filename=" << fileName;
     //   qDebug(fileName);
        dScene->loadGroup(fileName, QFileInfo(fileName).suffix());
    }
}

void MainWindow::langChanged(QString newLang)
{
    getScene()->setGroupLang(langManager->getAnalyzerForLang(newLang));
}

void MainWindow::closeTab(int position){
    if(tabWidget->count()==1){
        return;
    }
    tabWidget->removeTab(position);
}

void MainWindow::tabChanged(int position){
    BlockGroup *group=getScene()->selectedGroup();
    if(group==0){
        qDebug("tabChanged() group=0");
        setCurrentFile(0);
        return;
    }else{
        setCurrentFile(group);
    }
}

void MainWindow::createTabs()
{
    tabWidget = new QTabWidget(this);

    tabWidget->setDocumentMode(true);
    tabWidget->setMovable(true);
    tabWidget->setTabsClosable(true);
    connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));

    QIcon addTabIcon(":/plus.png");
    QPushButton *m_addButton = new QPushButton(addTabIcon,"", this);
    m_addButton->setObjectName("addButton");
    connect(m_addButton, SIGNAL(clicked()), this, SLOT(newTab()));
    tabWidget->setCornerWidget(m_addButton, Qt::TopLeftCorner);

    QWidget* widget=createView(); // get QGraphicView
    QGraphicsView* view=(QGraphicsView *) widget;
    DocumentScene* dScene=(DocumentScene *) view->scene();
    dScene->main=this;
    createMenus();
    createToolBars();
    statusBar();


    tabWidget->addTab(widget, "*tab0");
    tabWidget->setCurrentWidget(widget); // focus on new tab

    this->setCentralWidget(tabWidget);
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
        closeAction->setEnabled(true);
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

void MainWindow::open()
{
    QString fileFilters = tr("All files (*)");
    QString dir = QFileInfo(windowFilePath()).absoluteDir().absolutePath();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"), dir, fileFilters); // CHECK
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

void MainWindow::search()
{
    QString searchText = searchLineEdit->text();
    getScene()->findText(searchText);

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
    DocumentScene *scene=getScene();

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

    DocumentScene *scene=getScene();

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
        getScene()->addItem(list.at(i));
}

void MainWindow::hideArea()
{
    for(int i=0; i<list.size(); i++)
        getScene()->removeItem(list.at(i));
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
                       tr("<h2>TrollEdit 1.2</h2>"
                          "<p/>Team 5 - Ufopak - 2008"
                          "<p/>Team 10 - Innovators - 2012"
                          "<p/>This is a prototype of text editor enriched with graphical elemenets "
                          "which is being developed for Team project course."
                          "<p/>"
                          "<img src=\":logo2.png\" aling=\"center\" width=\"\"/>"
                          "<p/>"
                          "<a href=\"mailto:tp-team-10@googlegroups.com\">Send Feedback</a>"
                          ));
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
    getScene()->closeAllGroups();
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
