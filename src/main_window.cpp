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
#include <QString>
#include <QTextStream>


MainWindow::MainWindow(QString programPath, QWidget *parent) : QMainWindow(parent)
{
    langManager = new LanguageManager(programPath);

    createTabs();
    //createActions();
    createMenus();
    createToolBars();
    statusBar();
    readSettings();
    updateRecentFileActions();

    QIcon icon = QIcon();
    //icon.addFile(":/icon16.png");
    icon.addFile(":/icon32.png");
    setWindowIcon(icon);
    //setCurrentFile(0);

    //CSS style
    setStyleSheet(
        "QMainWindow {"
                        "color: black;"
                        "background-color: #C2C2C2;"
                        "border-color: silver;"
                        "font-size: 10px;"
                     "}"

                    "QTabWidget::tab-bar {"
                        "left: 2px;" /* odsadenie od plus*/
                    "}"

                    "QTabBar::tab {"
                        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                                                    "stop: 0 #E1E1E1, stop: 0.4 #DDDDDD,"
                                                    "stop: 0.5 #D8D8D8, stop: 1.0 #D3D3D3);"
                        "border: 1px solid silver;" /* vrchne oramovanie tabu*/

                        "border-top-left-radius: 7px;"
                        "border-top-right-radius: 7px;"
                        "min-width: 18ex;"
                        "padding: 5px;" /*vyska tabu*/
                    "}"

                    "QTabBar::tab:selected, QTabBar::tab:hover {"
                                                     "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                                                     "stop: 0 #fafafa, stop: 0.4 #f4f4f4,"
                                                     "stop: 0.5 #e7e7e7, stop: 1.0 #fafafa);"
                    "}"

                    "QTabBar::tab:selected {"
                         "border-color: #9B9B9B;"
                         "border-bottom-color: red;"
                    "}"

                     "QTabBar::tab:!selected {"
                         "margin-top: 2px;"
                     "}"

                    "QMenu {"
                        "background-color: #D6DADE;" /*vnutro menu*/
                        "color: white;"
                    "}"

                    "QMenu::item {"
                        "background-color: transparent;"
                        "color: black;"
                    "}"

                    "QMenu::item:selected {"
                        "background-color: #A6A8AB;"
                    "}"

                    "QMenu::item:disabled {"
                        "color:gray;"
                    "}"


                    "QMenuBar {"
                        "background-color: #787A7C;"
                        "color: white;"
                    "}"

                    "QMenuBar::item {"
                        "spacing: 13px;"
                        "padding: 2px 12px;"
                        "background: transparent;"
                        "border-radius: 2px;"
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


                    "QToolButton {"
                        "padding: 2px;"
                        "background-color: transparent;"
                        "min-width: 35px;"
                    "}"

                    "QToolButton:hover {"
                        "background: qradialgradient(cx: 0.3, cy: -0.4,"
                        "fx: 0.3, fy: -0.4,"
                        "radius: 1.35, stop: 0 #fff, stop: 1 #ddd);"
                    "}"

                    "QToolButton:pressed {"
                        "background: qradialgradient(cx: 0.4, cy: -0.1,"
                        "fx: 0.4, fy: -0.1,"
                        "radius: 1.35, stop: 0 #fff, stop: 1 #ddd);"
                    "}"

                    "QToolButton:disabled {"
                        "background: transparent"
                        "fx: 0.4, fy: -0.1,"
                        "radius: 1.35, stop: 0 #777, stop: 1 #333);"
                    "}"

                    "QStatusBar{ "
                        "color: black;"
                    "}"
                "QMessageBox {"
                "background-color:silver;"
                "}"
            );
}


void MainWindow::createActions(DocumentScene *scene)
{
    groupActions = new QActionGroup(this);

    // loading file for shurtcuts
    QFile file(":/files/shortcuts.ini");

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::information(0,"Error with shortcuts",file.errorString());
    }

    QString textstring;


    // new
    QIcon newIcon(":/icons/newFile"); newIcon.addFile(":/s/new"); // works
    newAction = new QAction(newIcon, tr("&New"), this);
    textstring = file.readLine();
    textstring.remove(6,1);
    newAction->setShortcut((textstring));
    newAction->setToolTip(tr("Create a new file"));
    connect(newAction, SIGNAL(triggered()), this, SLOT(newFile()));
    addAction(newAction);

    // open
    QIcon openIcon(":/icons/openFiles"); openIcon.addFile(":/s/open"); // works
    openAction = new QAction(openIcon, tr("&Open..."), this);
    textstring = file.readLine();
    textstring.remove(6,1);
    openAction->setShortcut((textstring));
    openAction->setToolTip(tr("Open an existing file"));
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

    // revert
    revertAction = new QAction(tr("&Revert"), this); // ??? is this used ???
    textstring = file.readLine();

    // QIcon revertIcon(":/m/open"); openIcon.addFile(":/s/open");
    revertAction = new QAction(tr("&Revert"), this);
    textstring = file.readLine();
    textstring.remove(6,1);
    revertAction->setShortcut((textstring));
    revertAction->setToolTip(tr("Revert to last save"));
    connect(revertAction, SIGNAL(triggered()), scene, SLOT(revertGroup()));
    groupActions->addAction(revertAction);


    // save
    QIcon saveIcon(":/icons/saveFile"); saveIcon.addFile(":/s/save"); // works (only for 1 scene)
    saveAction = new QAction(saveIcon, tr("&Save"), this);
    textstring = file.readLine();
    textstring.remove(6,1);
    saveAction->setShortcut((textstring));
    saveAction->setToolTip(tr("Save file"));
    connect(saveAction, SIGNAL(triggered()), scene, SLOT(saveGroup()));
    groupActions->addAction(saveAction);


    // save as
    QIcon saveAsIcon(":/m/save-as"); saveAsIcon.addFile(":/s/save-as"); // probably same as saveAction
    saveAsAction = new QAction(saveAsIcon, tr("Save &As..."), this);
    saveAsAction->setToolTip(tr("Save file as..."));
    connect(saveAsAction, SIGNAL(triggered()), scene, SLOT(saveGroupAs()));
    groupActions->addAction(saveAsAction);


    // save as
    // QIcon saveAsNoDocIcon(":/m/save-as"); saveAsIcon.addFile(":/s/save-as");
    saveAsNoDocAction = new QAction(tr("Save without comments"), this); // ??? is this used ???
    saveAsNoDocAction->setToolTip(tr("Save file without any comments"));
    connect(saveAsNoDocAction, SIGNAL(triggered()), scene, SLOT(saveGroupAsWithoutDoc()));
    groupActions->addAction(saveAsNoDocAction);


    // save all
    saveAllAction = new QAction(tr("Save All"), this); // works for 1 tab ??? does saveAllGroups work as it should ???
    saveAllAction->setToolTip(tr("Save all files"));
    connect(saveAllAction, SIGNAL(triggered()), scene, SLOT(saveAllGroups()));


    // close
    QIcon closeIcon(":/icons/closeFile"); closeIcon.addFile(":/s/close"); // works
    closeAction = new QAction(closeIcon, tr("&Close file"), scene);
    textstring = file.readLine();
    textstring.remove(6,1);
    closeAction->setShortcut((textstring));
    closeAction->setToolTip(tr("Close file"));
    connect(closeAction, SIGNAL(triggered()), scene, SLOT(closeGroup()));
    groupActions->addAction(closeAction);


    // close all
    closeAllAction = new QAction(tr("Close All"), this);
    closeAllAction->setToolTip(tr("Close all files"));
    connect(closeAllAction, SIGNAL(triggered()), this, SLOT(closeAllGroups()));


    // print pdf
    QIcon printIcon(":/icons/print"); printIcon.addFile(":/s/print");
    printPdfAction = new QAction(printIcon, tr("&Print PDF"), this);
    textstring = file.readLine();
    textstring.remove(6,1);
    printPdfAction->setShortcut((textstring));
    printPdfAction->setToolTip(tr("Print scene to PDF"));
    connect(printPdfAction, SIGNAL(triggered()), this, SLOT(printPdf()));
    groupActions->addAction(printPdfAction);


    // show plain text editor
    QIcon editIcon(":/icons/text"); printIcon.addFile(":/s/edit");
    plainEditAction = new QAction(editIcon, tr("&Edit plain text"), this);
    textstring = file.readLine();
    textstring.remove(6,1);
    plainEditAction->setShortcut((textstring));
    plainEditAction->setToolTip(tr("Edit file as plain text"));
    connect(plainEditAction, SIGNAL(triggered()), scene, SLOT(showPreview()));
    groupActions->addAction(plainEditAction);

    // clear search results
    QIcon clearIcon(":/icons/clean"); saveIcon.addFile(":/s/clear");
    clearAction = new QAction(clearIcon, tr("Clea&n search"), this);
    clearAction->icon().addFile(":/m/save.png");
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
    exitAction->setToolTip(tr("Quit the application?"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    // settings
    QIcon settingsIcon(":/m/settings"); settingsIcon.addFile(":/s/settings");
    settingsAction = new QAction(settingsIcon, tr("Se&ttings"), this);
    settingsAction->setToolTip(tr("General application settings"));
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(settings()));

    // help
    QIcon helpIcon(":/m/help"); helpIcon.addFile(":/s/help");
    helpAction = new QAction(helpIcon, tr("&Help - online"), this);
    helpAction->setShortcut(tr("F1"));
    helpAction->setToolTip(tr("Show application help"));
    connect(helpAction, SIGNAL(triggered()), this, SLOT(help()));

    // about
    aboutAction = new QAction(tr("&About"), this);
    aboutAction->setToolTip(tr("Show application's about box"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    // license
    showLicenseAction = new QAction(tr("&License"), this);
    showLicenseAction->setToolTip(tr("TrollEdit license"));
    connect(showLicenseAction, SIGNAL(triggered()), this, SLOT(showLicense()));

    // update
    updateAction = new QAction(tr("&Check for update"), this);
    updateAction->setToolTip(tr("Check new updates"));
    connect(updateAction, SIGNAL(triggered()), this, SLOT(update()));

    // home page
    homePageAction = new QAction(tr("&Home page"), this);
    homePageAction->setToolTip(tr("Open home page of TrollEdit"));
    connect(homePageAction, SIGNAL(triggered()), this, SLOT(homePage()));

    // about this version
    versionAction = new QAction(tr("&About this version"), this);
    versionAction->setToolTip(tr("View news on this a version"));
    connect(versionAction, SIGNAL(triggered()), this, SLOT(aboutVersion()));

    // bugs report
    bugReportAction = new QAction(tr("&Report bug"), this);
    bugReportAction->setToolTip(tr("Send report about a bug"));
    connect(bugReportAction, SIGNAL(triggered()), this, SLOT(bugReport()));

    // shortcuts
    shortAction = new QAction(tr("&Shortcuts"), this);
    shortAction->setStatusTip(tr("Setting shortcuts"));
    connect(shortAction, SIGNAL(triggered()), this, SLOT(setShort()));

    // set Lua language
    setLuaAction = new QAction(tr("&Lua"), this);
    setLuaAction->setStatusTip(tr("Set Lua language"));
    connect(setLuaAction, SIGNAL(triggered()), this, SLOT(setLanguageLua()));

    // set C language
    setCAction = new QAction(tr("&C"), this);
    setCAction->setStatusTip(tr("Set C language"));
    connect(setCAction, SIGNAL(triggered()), this, SLOT(setLanguageC()));

    // set Xml language
    setXmlAction = new QAction(tr("&Xml"), this);
    setXmlAction->setStatusTip(tr("Set Xml language"));
    connect(setXmlAction, SIGNAL(triggered()), this, SLOT(setLanguageXml()));

    // generate snapshot
    snapshotAction = new QAction(tr("&Snapshot"), this);
    snapshotAction->setStatusTip(tr("Generate snapshot"));
    connect( snapshotAction, SIGNAL(triggered()), this, SLOT(snapshot()));

    // options
    optionsAction = new QAction(tr("&Options"), this);
    optionsAction->setStatusTip(tr("Setting main funkcionality"));
    connect(optionsAction, SIGNAL(triggered()), this, SLOT(options()));

    // sw metrics
    metricsAction = new QAction(tr("&SW metrics"), this);
    metricsAction->setStatusTip(tr("Dispaly of sw metrics"));
    connect(metricsAction, SIGNAL(triggered()), this, SLOT(swMetrics()));

    // task list
    taskListAction = new QAction(tr("&Task list"), this);
    taskListAction->setStatusTip(tr("Show task list"));
    connect(taskListAction, SIGNAL(triggered()), this, SLOT(taskList()));

    // bug list
    bugListAction = new QAction(tr("&Bug list"), this);
    bugListAction->setStatusTip(tr("Show bug list"));
    connect(bugListAction, SIGNAL(triggered()), this, SLOT(bugList()));

    // text mode
    twoModeAction = new QAction(tr("&Text mode"), this);
    twoModeAction->setStatusTip(tr("Set text mode"));
    connect(twoModeAction, SIGNAL(triggered()), this, SLOT(twoMode()));

    // start page
    startUpAction = new QAction(tr("&Start page"), this);
    startUpAction->setStatusTip(tr("View StartUp screen"));
    connect(startUpAction, SIGNAL(triggered()), this, SLOT(startUp()));

    // basic toolbar
    basicToolbarAction = new QAction(tr("&Basic"), this);
    basicToolbarAction->setStatusTip(tr("Set basic toolbar"));
    connect(basicToolbarAction, SIGNAL(triggered()), this, SLOT(basicToolbar()));

    // formating toolbar
    formatingToolbarAction = new QAction(tr("&Formating"), this);
    formatingToolbarAction->setStatusTip(tr("Set formating toolbar"));
    connect(formatingToolbarAction, SIGNAL(triggered()), this, SLOT(formatingToolbar()));

    // web toolbar
    webToolbarAction = new QAction(tr("&Web"), this);
    webToolbarAction->setStatusTip(tr("Set web toolbar"));
    connect(webToolbarAction, SIGNAL(triggered()), this, SLOT(webToolbar()));

    // editor toolbar
    editorToolbarAction = new QAction(tr("&Editor"), this);
    editorToolbarAction->setStatusTip(tr("Set editor toolbar"));
    connect(editorToolbarAction, SIGNAL(triggered()), this, SLOT(editorToolbar()));

    // bottom dock panel
    setBottomDockAction = new QAction(tr("&Bottom dock"), this);
    setBottomDockAction->setStatusTip(tr("View bottom dock panel"));
    setBottomDockAction->setCheckable(true);
    connect(setBottomDockAction, SIGNAL(triggered()), this, SLOT(setBottomDock()));

    // bottom right panel
    setRightDockAction = new QAction(tr("&Right dock"), this);
    setRightDockAction->setStatusTip(tr("View right dock panel"));
    setRightDockAction->setCheckable(true);
    connect(setRightDockAction, SIGNAL(triggered()), this, SLOT(setRightDock()));

    // full screen
    fullScreenAction = new QAction(tr("&FullScreen"), this);
    fullScreenAction->setShortcut(tr("F8"));
    fullScreenAction->setCheckable(true);
    fullScreenAction->setStatusTip(tr("View full screen"));
    connect(fullScreenAction, SIGNAL(triggered()), this, SLOT(fullScreen()));

    // new window
    newWindowAction = new QAction(tr("&New window"), this);
    newWindowAction->setStatusTip(tr("Create new instance an application "));
    connect(newWindowAction, SIGNAL(triggered()), this, SLOT(newWindow()));

    // zoom in
    zoomInAction = new QAction(tr("&Zoom In"), this);
    zoomInAction->setStatusTip(tr("Zoom in"));
    connect(zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));

    // zoom out
    zoomOutAction = new QAction(tr("&Zoom Out"), this);
    zoomOutAction->setStatusTip(tr("Zoom out "));
    connect(zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));

    // split
    splitAction = new QAction(tr("&Split"), this);
    splitAction->setStatusTip(tr("Split a workspace "));
    splitAction->setCheckable(true);
    connect(splitAction, SIGNAL(triggered()), this, SLOT(split()));

    // CMD
    showCmdAction = new QAction(tr("&CMD"), this);
    showCmdAction->setCheckable(true);
    showCmdAction->setStatusTip(tr("Run command line"));
    connect(showCmdAction, SIGNAL(triggered()), this, SLOT(showCmd()));

    // undo
    undoAction = new QAction(tr("&Undo"), this);
    undoAction->setShortcut(tr("CTRL+Z"));
    undoAction->setStatusTip(tr("Undo"));
    connect(undoAction, SIGNAL(triggered()), this, SLOT(undo()));

    // redo
    redoAction = new QAction(tr("&Redo"), this);
    redoAction->setShortcut(tr("CTRL+Y"));
    redoAction->setStatusTip(tr("Redo"));
    connect(redoAction, SIGNAL(triggered()), this, SLOT(redo()));

    // cut
    cutAction = new QAction(tr("&Cut"), this);
    cutAction->setShortcut(tr("CTRL+X"));
    cutAction->setStatusTip(tr("Cut"));
    connect(cutAction, SIGNAL(triggered()), this, SLOT(cut()));

    // copy
    copyAction = new QAction(tr("&Copy"), this);
    copyAction->setShortcut(tr("CTRL+C"));
    copyAction->setStatusTip(tr("Copy"));
    connect(copyAction, SIGNAL(triggered()), this, SLOT(copy()));

    // paste
    pasteAction = new QAction(tr("&Paste"), this);
    pasteAction->setShortcut(tr("CTRL+V"));
    pasteAction->setStatusTip(tr("Paste"));
    connect(pasteAction, SIGNAL(triggered()), this, SLOT(paste()));

    // delete
    deleteAction = new QAction(tr("&Delete"), this);
    deleteAction->setStatusTip(tr("Delete"));
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(delet()));

    // selectAll
    selectAllAction = new QAction(tr("&Select All"), this);
    selectAllAction->setShortcut(tr("CTRL+A"));
    selectAllAction->setStatusTip(tr("Select All"));
    connect(selectAllAction, SIGNAL(triggered()), this, SLOT(selectAll()));

    // find
    findAction = new QAction(tr("&Find"), this);
    findAction->setShortcut(tr("CTRL+F"));
    findAction->setStatusTip(tr("Find"));
    connect(findAction, SIGNAL(triggered()), this, SLOT(find()));

    // find & replace
    find_ReplaceAction = new QAction(tr("&Find & Replace"), this);
    find_ReplaceAction->setShortcut(tr("CTRL+R"));
    find_ReplaceAction->setStatusTip(tr("Find and Replace"));
    connect(find_ReplaceAction, SIGNAL(triggered()), this, SLOT(find_Replace()));


    // show printable area
    QIcon areaIcon(":/icons/areaScreen"); areaIcon.addFile(":/s/area");
    printableAreaAction = new QAction(areaIcon, tr("Show Printable Area"), this);
    printableAreaAction->setToolTip(tr("Show margins of printable area"));
    connect(printableAreaAction, SIGNAL(triggered()), this, SLOT(showPrintableArea()));
    printableAreaAction->setCheckable(true);
    file.close();
}


// Items in MenuBar

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

    //edit menu
    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);
    editMenu->addAction(cutAction);
    editMenu->addAction(copyAction);
    editMenu->addAction(pasteAction);
    editMenu->addAction(deleteAction);
    editMenu->addSeparator();
    editMenu->addAction(selectAllAction);
    editMenu->addAction(findAction);
    editMenu->addAction(find_ReplaceAction);

    // View menu
    viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(newWindowAction);
    viewMenu->addAction(twoModeAction);
    viewMenu->addAction(startUpAction);
    viewMenu->addSeparator();
    // submenu toolbars
    setToolbarsMenu = viewMenu->addMenu("&Toolbars");
    setToolbarsMenu->addAction(basicToolbarAction);
    setToolbarsMenu->addAction(formatingToolbarAction);
    setToolbarsMenu->addAction(webToolbarAction);
    setToolbarsMenu->addAction(editorToolbarAction);
    // submenu panels
    panelsMenu = viewMenu->addMenu("&Panels");
    panelsMenu->addAction(setBottomDockAction);
    panelsMenu->addAction(setRightDockAction);

    viewMenu->addAction(fullScreenAction);
    viewMenu->addAction(splitAction);
    viewMenu->addSeparator();
    viewMenu->addAction(zoomInAction);
    viewMenu->addAction(zoomOutAction);

    // tolls menu
    tollsMenu = menuBar()->addMenu(tr("&Tools"));
    // submenu language
    languageMenu = tollsMenu->addMenu(tr("&Language"));
    languageMenu->addAction(setCAction);
    languageMenu->addAction(setLuaAction);
    languageMenu->addAction(setXmlAction);
    // submenu generate
    generateMenu= tollsMenu->addMenu(tr("&Generate to"));
    generateMenu->addAction(printPdfAction);
    generateMenu->addAction(snapshotAction);
    tollsMenu->addAction(taskListAction);
    tollsMenu->addAction(bugListAction);
    tollsMenu->addAction(showCmdAction);
    tollsMenu->addAction(shortAction);
    tollsMenu->addAction(metricsAction); // sw metrics
    tollsMenu->addSeparator();
    tollsMenu->addAction(optionsAction);

    // help menu
    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(homePageAction);
    helpMenu->addSeparator();
    helpMenu->addAction(helpAction);
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(showLicenseAction);
    helpMenu->addSeparator();
    helpMenu->addAction(updateAction);
    helpMenu->addAction(versionAction);
    helpMenu->addAction(bugReportAction);
}

// functions for Shortcusts --------------------------------------------------------------------------

void MainWindow::setShort()
{
    set_shortcuts = new QDialog();
    QPushButton *Savebutton = new QPushButton("OK", set_shortcuts);
    QPushButton *Closebutton = new QPushButton("Close", set_shortcuts);

    m_table = new QTableWidget(7, 2, set_shortcuts);

    m_table->setHorizontalHeaderItem(0, new QTableWidgetItem("Function"));
    m_table->setHorizontalHeaderItem(1, new QTableWidgetItem("Shortcut"));
    m_table->setItem(0,0, new QTableWidgetItem("New"));
    m_table->setItem(1,0, new QTableWidgetItem("Open"));
    m_table->setItem(2,0, new QTableWidgetItem("Revert"));
    m_table->setItem(3,0, new QTableWidgetItem("Save"));
    m_table->setItem(4,0, new QTableWidgetItem("Close"));
    m_table->setItem(5,0, new QTableWidgetItem("Print"));
    m_table->setItem(6,0, new QTableWidgetItem("Edit plain text"));

    QFile file(":/files/shortcuts.ini");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::information(0,"error",file.errorString());
    }

    for(int row=0; row < m_table->rowCount(); row++)
    {
        QString textstring = file.readLine();
        textstring.remove(6,1);
        m_table->setItem(row,1, new QTableWidgetItem(textstring));
    }

    file.close();
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
//End functions for Shortucts -------------------------------------------------------------------------------


void MainWindow::createToolBars()
{
    try
    {
        // format toolbar
        formatToolBar = addToolBar(tr("Format"));
        formatToolBar->setMovable(false);

        logoLabel = new QLabel();
        logoLabel->setPixmap(QPixmap(":/img/logoSmall"));
        formatToolBar->addWidget(logoLabel);

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
        //Style Combox for language
        scriptsBox->setStyle(new QPlastiqueStyle);


        scriptsBox->addItems(langManager->getLanguages());
        connect(scriptsBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(langChanged(QString)));
        formatToolBar->addWidget(scriptsBox);
        formatToolBar->addSeparator();

        searchLineEdit = new QLineEdit();
        searchLineEdit->setFixedSize(240, 20);
        searchLineEdit->setToolTip(tr("For result press Enter"));
        searchLineEdit->setText("search");
        searchLineEdit->setStyleSheet( "QLineEdit{"
                                       "color: black;"
                                       "font-style: italic;"
                                       "border-radius: 6px;"
                                       "}");
        connect(searchLineEdit, SIGNAL(editingFinished()), this, SLOT(search()));
        formatToolBar->addWidget(searchLineEdit);

        formatToolBar->addAction(clearAction);
    }

    catch(...)
    {
        QMessageBox::information(this,"Error","Error somewhere!");
    }
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

    createActions(dScene);

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
    qDebug("tabChanged()");
    setCurrentFile(getScene());
}

void MainWindow::createTabs()
{
    tabWidget = new QTabWidget(this);

    tabWidget->setDocumentMode(true);
    tabWidget->setMovable(true);
    tabWidget->setTabsClosable(true);
    connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));

    //button addTab
    QIcon addTabIcon(":/icons/plusTab.png");
    QPushButton *m_addButton = new QPushButton(addTabIcon,"", this);
    m_addButton->setObjectName("addButton");

    //set style for buton- add tab
    m_addButton->setStyleSheet("background-color: transparent;");

    connect(m_addButton, SIGNAL(clicked()), this, SLOT(newTab()));
    tabWidget->setCornerWidget(m_addButton, Qt::TopLeftCorner);

    QWidget* widget=createView(); // get QGraphicView
    QGraphicsView* view=(QGraphicsView *) widget;
    DocumentScene* dScene=(DocumentScene *) view->scene();
    dScene->main=this;
    createActions(dScene);

    tabWidget->addTab(widget, "Start page");
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
   //     lang = group->getAnalyzer()->getLanguageName();
  //      selectedGroup = group;
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

void MainWindow::setCurrentFile(DocumentScene *scene)
{
    QString fileName;
    QString lang;

    if(scene!=0){
        BlockGroup *group=scene->selectedGroup();

        if (group != 0)
        {
            fileName = group->getFilePath();
            lang = group->getAnalyzer()->getLanguageName();
            selectedGroup = group;
        }
        else
        {
            qDebug("selected group == 0");
            lang = "";
            fileName = "Empty";
            selectedGroup = 0;
        }


    if (fileName.isEmpty() || fileName == "Empty")
    {
        setWindowFilePath(fileName);
        groupActions->setEnabled(false);
     // searchLineEdit->setEnabled(false);
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
    try
    {
        QString searchText = searchLineEdit->text();
        getScene()->findText(searchText);
    }
    catch (...)
    {
        QMessageBox::information(this,"Error","Error with Search Text!");
    }
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
    // frame setting
    QColor color;
    color.setBlue(0);
    color.setGreen(0);
    color.setRed(138);
    QPen pen(color, 1, Qt::SolidLine);

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


//FUNTIONS FOR EDIT MENU ---------------------------------------------------------------------------------

void MainWindow::undo()
{
  QMessageBox::information(this,"title","On Function is working!");
}

void MainWindow::redo()
{
  QMessageBox::information(this,"title","On Function is working!");
}

void MainWindow::cut()
{
  QMessageBox::information(this,"title","On Function is working!");
}

void MainWindow::copy()
{
  QMessageBox::information(this,"title","On Function is working!");
}

void MainWindow::paste()
{
  QMessageBox::information(this,"title","On Function is working!");
}

void MainWindow::delet()
{
  QMessageBox::information(this,"title","On Function is working!");
}

void MainWindow::selectAll()
{
  QMessageBox::information(this,"title","On Function is working!");
}

void MainWindow::find()
{
  findWindow = new QDialog();
  QLabel *findLabel = new QLabel("Find:");
  QLineEdit *findLineEdit = new QLineEdit();
  findLineEdit->setStyleSheet("QLineEdit{"
                              "color: black;"
                              "font-style: italic;"
                              "border-radius: 6px;"
                              "}");
  QPushButton *findButton = new QPushButton("ok");
  findButton->setStyle(new QPlastiqueStyle);

  QHBoxLayout *layout = new QHBoxLayout(findWindow);
  layout->addWidget(findLabel);
  layout->addWidget(findLineEdit);
  layout->addWidget(findButton);

  findWindow->resize(500,40);
  findWindow->setWindowTitle("Find key word");
  findWindow->setLayout(layout);
  findWindow->show();
}

void MainWindow::find_Replace()
{
  QMessageBox::information(this,"title","On Function is working!");
}

//END OF FUNCTIONS FOR EDIT MENU -------------------------------------------------------------------------



//FUNTIONS FOR VIEW MENU ---------------------------------------------------------------------------------

// 2 modes
void MainWindow::twoMode()
{
    QMessageBox::information(this,"title","On Function is working!");
}

// startUp screen
void MainWindow::startUp()
{
    QMessageBox::information(this,"title","On Function is working!");
}

// basic toolbar
void MainWindow::basicToolbar()
{
     QMessageBox::information(this,"title","On Function is working!");
}

// formating toolbar
void MainWindow::formatingToolbar()
{
     QMessageBox::information(this,"title","On Function is working!");
}

// web toolbar
void MainWindow::webToolbar()
{
     QMessageBox::information(this,"title","On Function is working!");
}

// editor toolbar
void MainWindow::editorToolbar()
{
     QMessageBox::information(this,"title","On Function is working!");
}

// bottom dock
void MainWindow::setBottomDock()
{
    dock = new QDockWidget(tr("Buttom dock"), this);
    dock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    //add tab to dock panel
    QTabWidget *tabView = new QTabWidget();
    tabView->addTab (new QTextEdit,("&Task list"));
    tabView->addTab(new QTextEdit,("&Bug list"));
    tabView->addTab(new QTextEdit,("&Note"));

    dock->setWidget(tabView);
    addDockWidget(Qt::BottomDockWidgetArea, dock);
}

// right dock
void MainWindow::setRightDock()
{
      dock1 = new QDockWidget(tr("Right dock"), this);
      dock1->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
      text1 = new QTextEdit(dock1);
      dock1->setWidget(text1);
      addDockWidget(Qt::RightDockWidgetArea, dock1);
}

// full screen
void MainWindow::fullScreen()
{
    this->showMaximized();
    formatToolBar->hide();
}

// new window
void MainWindow::newWindow()
{
    QMessageBox::information(this,"title","On Function is working!");
}

// zoom in
void MainWindow::zoomIn()
{
    QMessageBox::information(this,"title","On Function is working!");
}

// zoom out
void MainWindow::zoomOut()
{
    QMessageBox::information(this,"title","On Function is working!");
}

// split
void MainWindow::split()
{
    this->showNormal();
    formatToolBar->show();
}

// END OF FUNCTIONS FOR VIEW MENU ------------------------------------------------------------------------



//FUNTIONS FOR TOOLS MENU ---------------------------------------------------------------------------------

// set Lua language
void MainWindow:: setLanguageLua()
{
    QMessageBox::information(this,"title","On Function is working!");
}

// set C language
void MainWindow:: setLanguageC()
{
    QMessageBox::information(this,"title","On Function is working!");
}

// set Xml language
void MainWindow:: setLanguageXml()
{
    QMessageBox::information(this,"title","On Function is working!");
}

// genarate to snapshot
void MainWindow::snapshot()
{
    QMessageBox::information(this,"title","On Function is working!");
}

// task list
void MainWindow::taskList()
{
    QMessageBox::information(this,"title","On Function is working!");
}

// bug list
void MainWindow::bugList()
{
    QMessageBox::information(this,"title","On Function is working!");
}

// options
void MainWindow::options()
{
     setOptions = new QDialog();

     setOptions->setWindowTitle("Options");
     setOptions->resize(520,390);
     setOptions->show();    
}

// sw metrics
void MainWindow::swMetrics()
{
    QMessageBox::information(this,"title","On Function is working!");
}

// rum CMD
void MainWindow::showCmd()
{
    try
    {
        QDesktopServices::openUrl(QUrl("cmd.exe"));
    }

    catch(...)
    {
        QMessageBox::information(this,"Warning","Cmd not could be found!");
    }

}

// END OF FUNCTIONS FOR TOOLS MENU ------------------------------------------------------------------------




//FUNTIONS FOR HELP MENU ----------------------------------------------------------------------------------

// about TrollEdit
void MainWindow::about()
{
    QMessageBox::about(this, tr("About TrollEdit"),
                       tr("<p><img src=\":/img/logoBig\" aling=\"center\" width=\"\"/></p>"

                          "</br>"
                          "<p><b>What is TrollEdit?</b></p>"
                          "<p>TrollEdit is a Qt based text editor developed by students at Slovak University of Technology."
                          "The main goal of the project is to teach students open source deveopmnet and team management."
                          "TrollEdit itself is an experiment to utilize full AST code analysis during writing of code."
                          "This can have benefits in various common tasks such as moving blocks of code, syntax checking,"
                          "syntax highlighting etc."
                          "</p>"

                          "<p>Version: 1.3.3.3</p>"
                          "<p>Copyright (C) 2012 TrollEdit</p>"
                          "<p></p>"
                          "<p><a href=\"http://innovators-team10.github.com/\">Visit our web  </a></p>"
                          "<p><a href=\"mailto:tp-team-10@googlegroups.com\">  Send feedaback</a></p>"
                          )
                       );
}


// license
void MainWindow::showLicense()
{
    QMessageBox::about(this, tr("License"),
                       tr("<p><img src=\":/img/logoBig\" aling=\"center\" width=\"\"/></p>"

                          "</br>"
                          "<p><b>Troll Edit License</b></p>"
                          "<p>TrollEdit is licensed under the terms of the MIT license reproduced below."
                          "This means that TrollEdit is free software and can be used for both academic"
                          "and commercial purposes at absolutely no cost.</p>"
                          "<p>------------------------------------------------</p>"

                          "<p>Copyright (C) 2012 TrollEdit.</p>"

                          "<p>Permission is hereby granted, free of charge, to any person obtaining a copy of this software and"
                          "associated documentation files (the 'Software'), to deal in the Software without restriction, including"
                          " without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell"
                          "copies of the Software, and to permit persons to whom the Software is furnished to do so, subject "
                          "to the following conditions:</p>"

                          "<p>The above copyright notice and this permission notice shall be included in all copies or"
                          "substantial portions of the Software.</p>"

                          "<p>THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING"
                          "BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND"
                          "NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES"
                          "OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR"
                          "IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.</p>"
                          "<p>-------------------------------------------------------------------------------</p>"
                          )
                       );

}

// show TrollEdit web page
void MainWindow::homePage()
{
    QDesktopServices::openUrl(QUrl("http://innovators-team10.github.com"));
}

// html help
void MainWindow::help()
{
    QDesktopServices::openUrl(QUrl("http://innovators-team10.github.com/user-manual.html"));
}

// check new update
void MainWindow::update()
{
    QMessageBox::information(this,"title","On Function is working!");
}

// about this version
void MainWindow::aboutVersion()
{
    aboutVersionWindow = new QDialog();
    aboutVersionLabel= new QLabel(aboutVersionWindow);
    aboutVersionLabel->setPixmap(QPixmap(":/img/logoSmall"));
    aboutVersionTextEdit = new QTextEdit(aboutVersionWindow);

    outer = new QVBoxLayout();
    outer->addWidget(aboutVersionLabel);
    outer->addWidget(aboutVersionTextEdit);
    aboutVersionWindow->setLayout(outer);

    // open file
    QFile sfile(":/files/aboutVersion.txt");
    if(sfile.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream in(&sfile);
        QString text= in.readAll();
        sfile.close();
        aboutVersionTextEdit->setPlainText(text);
        aboutVersionTextEdit->setReadOnly(true);
    }

    aboutVersionWindow ->setWindowTitle("About this version");
    aboutVersionWindow->resize(510,350);
    aboutVersionWindow ->show();
}

// for send bugs report
void MainWindow::bugReport()
{
    QDesktopServices::openUrl(QUrl("mailto:tp-team-10@googlegroups.com?subject=[TrollEdit]--report_bug&body=Descripe Yours problem:",QUrl::TolerantMode));
}

// END OF FUNCTIONS FOR HELP MENU -------------------------------------------------------------------------


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


//toto je na�o ta funkcia???
void MainWindow::settings()
{

}


void MainWindow::closeEvent(QCloseEvent *event)
{
    getScene()->closeAllGroups();
    writeSettings();
    event->accept();
}


// set size of Mainwindow
void MainWindow::readSettings()
{
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    QPoint pos = settings.value("pos", QPoint(100, 100)).toPoint();
    QSize size = settings.value("size", QSize(850, 700)).toSize();
    resize(size);
    move(pos);

    if (settings.value("minimized", false).toBool()) //predtym maximized
        showMinimized();
}

void MainWindow::writeSettings()
{
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("minimized", isMinimized()); //predtym maximized
}
