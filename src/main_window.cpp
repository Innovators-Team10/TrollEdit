/**
* @file main_window.cpp
* @author Team 04 Ufopak + Team 10 Innovators
* @version
*
* @section DESCRIPTION
* Contains the defintion of class MainWindow.
*/


#include "main_window.h"
#include "ui_main_window.h"
#include "document_scene.h"
#include "language_manager.h"
#include "setting.h"
#include "abouttrolledit.h"
#include "tips_tricks.h"
#include "analyzer.h"
#include "block_group.h"
#include <QTableWidget>
#include <QFont>
#include <QPushButton>
#include <QtGui>
#include <QString>
#include <QTextStream>
#include <QtWebKit>

#define CONFIG_DIR "/../share/trolledit"

extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

static MainWindow *p_window;

void loadConfig(lua_State *L, const char *fname, int *w, int *h, QString *style) {
    if (luaL_loadfile(L, fname) || lua_pcall(L, 0, 0, 0))
        qDebug() << "cannot run config. file: " <<  lua_tostring(L, -1);
    lua_getglobal(L, "style");
    lua_getglobal(L, "width");
    lua_getglobal(L, "height");
    if (!lua_isstring(L, -3))
        qDebug() << "'style' should be a string\n";
    if (!lua_isnumber(L, -2))
        qDebug() << "'width' should be a number\n";
    if (!lua_isnumber(L, -1))
        qDebug() << "'height' should be a number\n";
    *style = lua_tostring(L, -3);
    *w = lua_tointeger(L, -2);
    *h = lua_tointeger(L, -1);
}

static int setstyle(lua_State *L) {
    QString str = lua_tostring(L, 1); /* get argument */
    p_window->setStyleSheet(str);
    return 0;                         /* number of results in LUA*/
}

//...Shortcuts
static int l_newAction(lua_State *L) {
    QString str = lua_tostring(L, 1);
    QString tool_str = lua_tostring(L, 2);

    //! new
    QIcon newIcon(":/icons/new.png");
    p_window->newAction = new QAction(newIcon, "&New", p_window);
    p_window->newAction->setShortcut(str);
    p_window->newAction->setToolTip(tool_str);//tr("Create a new file")
    p_window->connect(p_window->newAction, SIGNAL(triggered()), p_window, SLOT(newFile()));
    p_window->addAction(p_window->newAction);
    return 0;
}

static int l_openAction(lua_State *L) {
    QString str = lua_tostring(L, 1);
    QString tool_str = lua_tostring(L, 2);

    //! open
    QIcon openIcon(":/icons/open.png");
    p_window->openAction = new QAction(openIcon, "&Open...", p_window);
    p_window->openAction->setShortcut(str);
    p_window->openAction->setToolTip(tool_str);//tr("Open an existing file"));
    p_window->connect(p_window->openAction, SIGNAL(triggered()), p_window, SLOT(open()));;
    return 0;
}

static int l_revertAction(lua_State *L) {
    QString str = lua_tostring(L, 1);
    QString tool_str = lua_tostring(L, 2);

    //! revert
    p_window->revertAction = new QAction("&Revert", p_window);
    p_window->revertAction->setShortcut(str);
    p_window->revertAction->setToolTip(tool_str);//tr("Revert to last save"));
    p_window->connect(p_window->revertAction, SIGNAL(triggered()), p_window, SLOT(revertGroupWrapper()));
    p_window->groupActions->addAction(p_window->revertAction);
    return 0;
}

/*
    groupActions = new QActionGroup(this);

    //! save
    QIcon saveIcon(":/icons/save.png"); // works (only for 1 scene)
    saveAction = new QAction(saveIcon, tr("&Save"), this);
    textstring = file.readLine();
    textstring.remove(6,1);
    saveAction->setShortcut((textstring));
    saveAction->setToolTip(tr("Save file"));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveGroupWrapper()));
    groupActions->addAction(saveAction);


    //! save as
    QIcon saveAsIcon(":/icons/save.png");  // probably same as saveAction
    saveAsAction = new QAction(saveAsIcon, tr("Save &As..."), this);
    saveAsAction->setToolTip(tr("Save file as..."));
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveGroupAsWrapper()));
    groupActions->addAction(saveAsAction);


        saveAsNoDocAction = new QAction(tr("Save Without Comments"), this); // ??? is this used ???
        saveAsNoDocAction->setToolTip(tr("Save file without any comments"));
        connect(saveAsNoDocAction, SIGNAL(triggered()), this, SLOT(saveGroupAsWithoutDocWrapper()));
        groupActions->addAction(saveAsNoDocAction);

        //! save all
        QIcon saveAllIcon(":/icons/saveAll.png");
        saveAllAction = new QAction(saveAllIcon,tr("Save All"), this); // works for 1 tab ??? does saveAllGroups work as it should ???
        saveAllAction->setToolTip(tr("Save all files"));
        connect(saveAllAction, SIGNAL(triggered()), this, SLOT(saveAllGroupsWrapper()));

        //! close
        QIcon closeIcon(":/icons/closeFile.png");
        closeAction = new QAction(closeIcon, tr("&Close File"), this);
        textstring = file.readLine();
        textstring.remove(6,1);
        closeAction->setShortcut((textstring));
        closeAction->setToolTip(tr("Close file"));
        connect(closeAction, SIGNAL(triggered()), this, SLOT(closeGroupWrapper()));
        groupActions->addAction(closeAction);

        //! close all
        closeAllAction = new QAction(tr("Close All"), this);
        closeAllAction->setToolTip(tr("Close all files"));
        connect(closeAllAction, SIGNAL(triggered()), this, SLOT(closeAllGroupsWrapper()));

        //! print pdf
        QIcon printIcon(":/icons/print.png");
        printPdfAction = new QAction(printIcon, tr("&Print PDF"), this);
        textstring = file.readLine();
        textstring.remove(6,1);
        printPdfAction->setShortcut((textstring));
        printPdfAction->setToolTip(tr("Print scene to PDF"));
        connect(printPdfAction, SIGNAL(triggered()), this, SLOT(printPdf()));
        groupActions->addAction(printPdfAction);

        //! show plain text editor
        QIcon editIcon(":/icons/textMode");
        plainEditAction = new QAction(editIcon, tr("&Edit Plain Text"), this);
        textstring = file.readLine();
        textstring.remove(6,1);
        plainEditAction->setShortcut((textstring));
        plainEditAction->setToolTip(tr("Edit file as plain text"));
        connect(plainEditAction, SIGNAL(triggered()), this, SLOT(showPreviewWrapper()));
        groupActions->addAction(plainEditAction);

        //! clear search results
        QIcon clearIcon(":/icons/close");
        clearAction = new QAction(clearIcon, tr("Clea&n Search"), this);
        //clearAction->setShortcut(tr("CTRL+S"));
        clearAction->setToolTip(tr("Clean search results"));
        connect(clearAction, SIGNAL(triggered()), this, SLOT(cleanGroupWrapper()));
        groupActions->addAction(clearAction);


    //! recent files
    for (int i = 0; i < MaxRecentFiles; ++i)
    {
        recentFileActions[i] = new QAction(this);
        recentFileActions[i]->setVisible(false);
        connect(recentFileActions[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }

    //! exit
    QIcon exitIcon(":/icons/exit.png");
    exitAction = new QAction(exitIcon, tr("E&xit"), this);
    exitAction->setToolTip(tr("Quit the application?"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    //! help
    QIcon helpIcon(":/icons/help.png");
    helpAction = new QAction(helpIcon, tr("&Help - online"), this);
    helpAction->setShortcut(tr("F1"));
    helpAction->setToolTip(tr("Show application help"));
    connect(helpAction, SIGNAL(triggered()), this, SLOT(help()));

    //! about
    QIcon aboutIcon(":/icons/info.png");
    aboutAction = new QAction(aboutIcon,tr("&About"), this);
    aboutAction->setToolTip(tr("Show application's about box"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    //! update
    updateAction = new QAction(tr("&Check for update"), this);
    updateAction->setToolTip(tr("Check new updates"));
    connect(updateAction, SIGNAL(triggered()), this, SLOT(update()));

    //! home page
    QIcon homeIcon(":/icons/home.png");
    homePageAction = new QAction(homeIcon,tr("&Home page"), this);
    homePageAction->setToolTip(tr("Open home page of TrollEdit"));
    connect(homePageAction, SIGNAL(triggered()), this, SLOT(homePage()));

    //! bugs report
    bugReportAction = new QAction(tr("&Report bug"), this);
    bugReportAction->setToolTip(tr("Send report about a bug"));
    connect(bugReportAction, SIGNAL(triggered()), this, SLOT(bugReport()));

    //! shortcuts
    shortAction = new QAction(tr("&Shortcuts"), this);
    shortAction->setStatusTip(tr("Setting shortcuts"));
    connect(shortAction, SIGNAL(triggered()), this, SLOT(setShort()));

    //! set Lua language
    setLuaAction = new QAction(tr("&Lua"), this);
    setLuaAction->setStatusTip(tr("Set Lua language"));
    connect(setLuaAction, SIGNAL(triggered()), this, SLOT(setLanguageLua()));

    //! set C language
    setCAction = new QAction(tr("&C"), this);
    setCAction->setStatusTip(tr("Set C language"));
    connect(setCAction, SIGNAL(triggered()), this, SLOT(setLanguageC()));

    //! set Xml language
    setXmlAction = new QAction(tr("&Xml"), this);
    setXmlAction->setStatusTip(tr("Set Xml language"));
    connect(setXmlAction, SIGNAL(triggered()), this, SLOT(setLanguageXml()));

    //! generate snapshot
    snapshotAction = new QAction(tr("&Snapshot"), this);
    snapshotAction->setStatusTip(tr("Generate snapshot"));
    connect( snapshotAction, SIGNAL(triggered()), this, SLOT(snapshot()));

    //! options
    QIcon optionIcon(":/icons/seeting.png");
    optionsAction = new QAction(optionIcon,tr("&Options"), this);
    optionsAction->setStatusTip(tr("Setting main funkcionality"));
    connect(optionsAction, SIGNAL(triggered()), this, SLOT(options()));

    //! sw metrics
    QIcon metricsIcon(":/icons/chart.png");
    metricsAction = new QAction(metricsIcon,tr("&SW metrics"), this);
    metricsAction->setStatusTip(tr("Dispaly of sw metrics"));
    connect(metricsAction, SIGNAL(triggered()), this, SLOT(swMetrics()));

    //! task list
    QIcon taskIcon(":/icons/taskList.png");
    taskListAction = new QAction(taskIcon,tr("&Task list"), this);
    taskListAction->setStatusTip(tr("Show task list"));
    connect(taskListAction, SIGNAL(triggered()), this, SLOT(taskList()));

    //! bug list
    QIcon bugIcon(":/icons/bugList.png");
    bugListAction = new QAction(bugIcon,tr("&Bug list"), this);
    bugListAction->setStatusTip(tr("Show bug list"));
    connect(bugListAction, SIGNAL(triggered()), this, SLOT(bugList()));

    //! basic toolbar
    basicToolbarAction = new QAction(tr("&Basic"), this);
    basicToolbarAction->setStatusTip(tr("Set basic toolbar"));
    connect(basicToolbarAction, SIGNAL(triggered()), this, SLOT(basicToolbar()));

    //! format toolbar
    formatToolbarAction = new QAction(tr("&Format"), this);
    formatToolbarAction->setStatusTip(tr("Set format toolbar"));
    connect(formatToolbarAction, SIGNAL(triggered()), this, SLOT(formatToolbars()));

    //! tools toolbar
    toolsToolbarAction = new QAction(tr("&Tools"), this);
    toolsToolbarAction->setStatusTip(tr("Set tools toolbar"));
    connect(toolsToolbarAction, SIGNAL(triggered()), this, SLOT(toolsToolbar()));

    //! editor toolbar
    editorToolbarAction = new QAction(tr("&Editor"), this);
    editorToolbarAction->setStatusTip(tr("Set editor toolbar"));
    connect(editorToolbarAction, SIGNAL(triggered()), this, SLOT(editorToolbar()));

    //! bottom dock panel
    setBottomDockAction = new QAction(tr("&Bottom dock"), this);
    setBottomDockAction->setStatusTip(tr("View bottom dock panel"));
    setBottomDockAction->setCheckable(true);
    connect(setBottomDockAction, SIGNAL(triggered()), this, SLOT(setBottomDock()));

    //! bottom right dock panel
    setRightDockAction = new QAction(tr("&Right dock"), this);
    setRightDockAction->setStatusTip(tr("View right dock panel"));
    setRightDockAction->setCheckable(true);
    connect(setRightDockAction, SIGNAL(triggered()), this, SLOT(setRightDock()));

    //! fullscreen
    QIcon fullScreenIcon(":/icons/fullScreen.png");
    fullScreenAction = new QAction(fullScreenIcon,tr("&FullScreen"), this);
    fullScreenAction->setShortcut(tr("F8"));
    fullScreenAction->setCheckable(true);
    fullScreenAction->setStatusTip(tr("View full screen"));
    connect(fullScreenAction, SIGNAL(triggered()), this, SLOT(fullScreen()));

    //! new window
    newWindowAction = new QAction(tr("&New window"), this);
    newWindowAction->setStatusTip(tr("Create new instance an application "));
    connect(newWindowAction, SIGNAL(triggered()), this, SLOT(newWindow()));

    //! zoom in
    QIcon zoomIncon(":/icons/plus.png");
    zoomInAction = new QAction(zoomIncon,tr("&Zoom In"), this);
    zoomInAction->setStatusTip(tr("Zoom in"));
    connect(zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));

    //! zoom out
    QIcon zoomOutIcon(":/icons/minus.png");
    zoomOutAction = new QAction(zoomOutIcon,tr("&Zoom Out"), this);
    zoomOutAction->setStatusTip(tr("Zoom out "));
    connect(zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));

    //! split
    splitAction = new QAction(tr("&Split"), this);
    splitAction->setStatusTip(tr("Split a workspace "));
    splitAction->setCheckable(true);
    connect(splitAction, SIGNAL(triggered()), this, SLOT(split()));

    //! CMD
    QIcon cmdIcon(":/icons/cmd.png");
    showCmdAction = new QAction(cmdIcon,tr("&CMD"), this);
    showCmdAction->setCheckable(true);
    showCmdAction->setStatusTip(tr("Run command line"));
    connect(showCmdAction, SIGNAL(triggered()), this, SLOT(showCmd()));

    //! undo
    QIcon undoIcon(":/icons/undo.png");
    undoAction = new QAction(undoIcon, tr("&Undo"), this);
    undoAction->setShortcut(tr("CTRL+Z"));
    undoAction->setStatusTip(tr("Undo"));
    undoAction->setEnabled(false);
    connect(MainWindow::undoAction, SIGNAL(triggered()), this, SLOT(undo()));
    actionList.append(undoAction);

    //! redo
    QIcon redoIcon(":/icons/redo.png");
    redoAction = new QAction(redoIcon, tr("&Redo"), this);
    redoAction->setShortcut(tr("CTRL+Y"));
    redoAction->setStatusTip(tr("Redo"));
    redoAction->setEnabled(false);
    connect(redoAction, SIGNAL(triggered()), this, SLOT(redo()));
    actionList.append(redoAction);

    //! cut
    QIcon cutIcon(":/icons/cut.png");
    cutAction = new QAction(cutIcon, tr("&Cut"), this);
    cutAction->setShortcut(tr("CTRL+X"));
    cutAction->setStatusTip(tr("Cut"));
    cutAction->setEnabled(false);
    connect(cutAction, SIGNAL(triggered()), this, SLOT(cut()));
    actionList.append(cutAction);

    //! copy
    QIcon copyIcon(":/icons/copy.png");
    copyAction = new QAction(copyIcon,tr("&Copy"), this);
    copyAction->setShortcut(tr("CTRL+C"));
    copyAction->setStatusTip(tr("Copy"));
    copyAction->setEnabled(false);
    connect(copyAction, SIGNAL(triggered()), this, SLOT(copy()));
    actionList.append(copyAction);

    //! paste
    QIcon pasteIcon(":/icons/paste.png");
    pasteAction = new QAction(pasteIcon,tr("&Paste"), this);
    pasteAction->setShortcut(tr("CTRL+V"));
    pasteAction->setStatusTip(tr("Paste"));
    pasteAction->setEnabled(false);
    connect(pasteAction, SIGNAL(triggered()), this, SLOT(paste()));
    actionList.append(pasteAction);

    //! delete
    QIcon deleteIcon(":/icons/delete.png");
    deleteAction = new QAction(deleteIcon,tr("&Delete"), this);
    deleteAction->setShortcut(tr("DEL"));
    deleteAction->setStatusTip(tr("Delete"));
    deleteAction->setEnabled(false);
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(delet()));
    actionList.append(deleteAction);

    //! selectAll
    selectAllAction = new QAction(tr("&Select All"), this);
    selectAllAction->setShortcut(tr("CTRL+A"));
    selectAllAction->setStatusTip(tr("Select All"));
    selectAllAction->setEnabled(false);
    connect(selectAllAction, SIGNAL(triggered()), this, SLOT(selectAll()));
    actionList.append(selectAllAction);

    //! attach file
    QIcon attachIcon(":/icons/spin.png");
    attachFileAction = new QAction(attachIcon,tr("&Attach file"), this);
    attachFileAction->setStatusTip(tr("Attach file"));
    connect(attachFileAction, SIGNAL(triggered()), this, SLOT(attachFile()));

    //! find
    QIcon findIcon(":/icons/find.png");
    findAction = new QAction(findIcon,tr("&Find"), this);
    findAction->setShortcut(tr("CTRL+F"));
    findAction->setStatusTip(tr("Find"));
    connect(findAction, SIGNAL(triggered()), this, SLOT(find()));

    //! find & replace
    find_ReplaceAction = new QAction(tr("&Find & Replace"), this);
    find_ReplaceAction->setShortcut(tr("CTRL+R"));
    find_ReplaceAction->setStatusTip(tr("Find and Replace"));
    connect(find_ReplaceAction, SIGNAL(triggered()), this, SLOT(find_Replace()));

    //! set bold font
    QIcon boldIcon(":/icons/bold.png");
    setBoldAction = new QAction(boldIcon,tr("&Bold font"), this);
    setBoldAction->setStatusTip(tr("Set bold font"));
    connect(setBoldAction, SIGNAL(triggered()), this, SLOT(setBold()));

    //! set italic font
    QIcon italicIcon(":/icons/italic.png");
    setItalicAction = new QAction(italicIcon,tr("&italic font"), this);
    setItalicAction->setStatusTip(tr("Set italic font"));
    connect(setItalicAction, SIGNAL(triggered()), this, SLOT(setItalic()));

    //! show printable area
    QIcon areaIcon(":/icons/printArea.png");
    printableAreaAction = new QAction(areaIcon, tr("Show Printable Area"), this);
    printableAreaAction->setToolTip(tr("Show margins of printable area"));
    connect(printableAreaAction, SIGNAL(triggered()), this, SLOT(showPrintableArea()));
    file.close();
}
*/

MainWindow::MainWindow(QString programPath, QWidget *parent) : QMainWindow(parent)
{
    langManager = new LanguageManager(programPath);

    createActions();
    initLuaState(programPath);
    createTabs();

    readSettings();

    updateRecentFileActions();
}

//! create lua state
void MainWindow::initLuaState(QString programPath){
    this->L = luaL_newstate();

    //! Load config from config_app.lua
    lua_State *L = getLuaState();
    luaL_openlibs(L);
    int width, height; QString style;
    QDir dir = QDir(programPath + CONFIG_DIR);
    //QFileInfoList configs = dir.entryInfoList(QStringList("*.lua"), QDir::Files | QDir::NoSymLinks);
    QFileInfo configFile(dir.absolutePath()+ QDir::separator() + "config_app.lua");

    p_window = this;
    //! Register function for lua
    lua_register(L, "setstyle", setstyle);
    //...shortcuts
    lua_register(L, "l_newAction", l_newAction);
    lua_register(L, "l_openAction", l_openAction);
    lua_register(L, "l_revertAction", l_revertAction);
    //...

    //! Loading config file
    loadConfig(L, qPrintable(configFile.absoluteFilePath()), &width, &height, &style);
    qDebug() << configFile.absoluteFilePath() << "width: " << width << " height: " << height << "\n style: " << style;
    //window size
    resize(width, height);
    //CSS style
    //w.setStyleSheet(style);
    //w.setStyleSheet();
}

//! give lua state with configuration
lua_State* MainWindow::getLuaState(){
    return this->L;
}

//! wrapper slots
//! in most functions is dynamically detected current BlockGroup,
//! so they are called with parameter 0
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
    getScene()->selectedGroup()->changeMode(actionList);
//    getScene()->showPreview(0); // povodny edit plain text, zobrazi okno s plain textom
}

void MainWindow::cleanGroupWrapper(){
    getScene()->cleanGroup(0);
}

void MainWindow::createActions()
{
    groupActions = new QActionGroup(this);

    //! loading file for shurtcuts
    QFile file(":/files/shortcutsFile");

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::information(0,"Error with shortcuts",file.errorString());
    }

    QString textstring;


    //! new
    QIcon newIcon(":/icons/new.png");
    newAction = new QAction(newIcon, tr("&New"), this);
    textstring = file.readLine();
    textstring.remove(6,1);
    newAction->setShortcut((textstring));
    newAction->setToolTip(tr("Create a new file"));
    connect(newAction, SIGNAL(triggered()), this, SLOT(newFile()));
    addAction(newAction);

    //! open
    QIcon openIcon(":/icons/open.png");
    openAction = new QAction(openIcon, tr("&Open..."), this);
    textstring = file.readLine();
    textstring.remove(6,1);
    openAction->setShortcut((textstring));
    openAction->setToolTip(tr("Open an existing file"));
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

    //! revert
    revertAction = new QAction(tr("&Revert"), this);
    textstring = file.readLine();
    textstring.remove(6,1);
    revertAction->setShortcut((textstring));
    revertAction->setToolTip(tr("Revert to last save"));
    connect(revertAction, SIGNAL(triggered()), this, SLOT(revertGroupWrapper()));
    groupActions->addAction(revertAction);


    //! save
    QIcon saveIcon(":/icons/save.png"); // works (only for 1 scene)
    saveAction = new QAction(saveIcon, tr("&Save"), this);
    textstring = file.readLine();
    textstring.remove(6,1);
    saveAction->setShortcut((textstring));
    saveAction->setToolTip(tr("Save file"));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveGroupWrapper()));
    groupActions->addAction(saveAction);


    //! save as
    QIcon saveAsIcon(":/icons/save.png");  // probably same as saveAction
    saveAsAction = new QAction(saveAsIcon, tr("Save &As..."), this);
    saveAsAction->setToolTip(tr("Save file as..."));
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveGroupAsWrapper()));
    groupActions->addAction(saveAsAction);


        saveAsNoDocAction = new QAction(tr("Save Without Comments"), this); // ??? is this used ???
        saveAsNoDocAction->setToolTip(tr("Save file without any comments"));
        connect(saveAsNoDocAction, SIGNAL(triggered()), this, SLOT(saveGroupAsWithoutDocWrapper()));
        groupActions->addAction(saveAsNoDocAction);

        //! save all
        QIcon saveAllIcon(":/icons/saveAll.png");
        saveAllAction = new QAction(saveAllIcon,tr("Save All"), this); // works for 1 tab ??? does saveAllGroups work as it should ???
        saveAllAction->setToolTip(tr("Save all files"));
        connect(saveAllAction, SIGNAL(triggered()), this, SLOT(saveAllGroupsWrapper()));

        //! close
        QIcon closeIcon(":/icons/closeFile.png");
        closeAction = new QAction(closeIcon, tr("&Close File"), this);
        textstring = file.readLine();
        textstring.remove(6,1);
        closeAction->setShortcut((textstring));
        closeAction->setToolTip(tr("Close file"));
        connect(closeAction, SIGNAL(triggered()), this, SLOT(closeGroupWrapper()));
        groupActions->addAction(closeAction);

        //! close all
        closeAllAction = new QAction(tr("Close All"), this);
        closeAllAction->setToolTip(tr("Close all files"));
        connect(closeAllAction, SIGNAL(triggered()), this, SLOT(closeAllGroupsWrapper()));

        //! print pdf
        QIcon printIcon(":/icons/print.png");
        printPdfAction = new QAction(printIcon, tr("&Print PDF"), this);
        textstring = file.readLine();
        textstring.remove(6,1);
        printPdfAction->setShortcut((textstring));
        printPdfAction->setToolTip(tr("Print scene to PDF"));
        connect(printPdfAction, SIGNAL(triggered()), this, SLOT(printPdf()));
        groupActions->addAction(printPdfAction);

        //! show plain text editor
        QIcon editIcon(":/icons/textMode");
        plainEditAction = new QAction(editIcon, tr("&Edit Plain Text"), this);
        textstring = file.readLine();
        textstring.remove(6,1);
        plainEditAction->setShortcut((textstring));
        plainEditAction->setToolTip(tr("Edit file as plain text"));
        connect(plainEditAction, SIGNAL(triggered()), this, SLOT(showPreviewWrapper()));
        groupActions->addAction(plainEditAction);

        //! clear search results
        QIcon clearIcon(":/icons/close");
        clearAction = new QAction(clearIcon, tr("Clea&n Search"), this);
        //clearAction->setShortcut(tr("CTRL+S"));
        clearAction->setToolTip(tr("Clean search results"));
        connect(clearAction, SIGNAL(triggered()), this, SLOT(cleanGroupWrapper()));
        groupActions->addAction(clearAction);


    //! recent files
    for (int i = 0; i < MaxRecentFiles; ++i)
    {
        recentFileActions[i] = new QAction(this);
        recentFileActions[i]->setVisible(false);
        connect(recentFileActions[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }

    //! exit
    QIcon exitIcon(":/icons/exit.png");
    exitAction = new QAction(exitIcon, tr("E&xit"), this);
    exitAction->setToolTip(tr("Quit the application?"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    //! help
    QIcon helpIcon(":/icons/help.png");
    helpAction = new QAction(helpIcon, tr("&Help - online"), this);
    helpAction->setShortcut(tr("F1"));
    helpAction->setToolTip(tr("Show application help"));
    connect(helpAction, SIGNAL(triggered()), this, SLOT(help()));

    //! about
    QIcon aboutIcon(":/icons/info.png");
    aboutAction = new QAction(aboutIcon,tr("&About"), this);
    aboutAction->setToolTip(tr("Show application's about box"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    //! tips and tricks
    tipsAction = new QAction(aboutIcon,tr("&Tips and Tricks"), this);
    tipsAction->setToolTip(tr("Show tips and tricks"));
    connect(tipsAction, SIGNAL(triggered()), this, SLOT(tipsAndTricks()));

    //! update
    updateAction = new QAction(tr("&Check for update"), this);
    updateAction->setToolTip(tr("Check new updates"));
    connect(updateAction, SIGNAL(triggered()), this, SLOT(update()));

    //! home page
    QIcon homeIcon(":/icons/home.png");
    homePageAction = new QAction(homeIcon,tr("&Home page"), this);
    homePageAction->setToolTip(tr("Open home page of TrollEdit"));
    connect(homePageAction, SIGNAL(triggered()), this, SLOT(homePage()));

    //! bugs report
    bugReportAction = new QAction(tr("&Report bug"), this);
    bugReportAction->setToolTip(tr("Send report about a bug"));
    connect(bugReportAction, SIGNAL(triggered()), this, SLOT(bugReport()));

    //! shortcuts
    shortAction = new QAction(tr("&Shortcuts"), this);
    shortAction->setStatusTip(tr("Setting shortcuts"));
    connect(shortAction, SIGNAL(triggered()), this, SLOT(setShort()));

    //! set Lua language
    setLuaAction = new QAction(tr("&Lua"), this);
    setLuaAction->setStatusTip(tr("Set Lua language"));
    connect(setLuaAction, SIGNAL(triggered()), this, SLOT(setLanguageLua()));

    //! set C language
    setCAction = new QAction(tr("&C"), this);
    setCAction->setStatusTip(tr("Set C language"));
    connect(setCAction, SIGNAL(triggered()), this, SLOT(setLanguageC()));

    //! set Xml language
    setXmlAction = new QAction(tr("&Xml"), this);
    setXmlAction->setStatusTip(tr("Set Xml language"));
    connect(setXmlAction, SIGNAL(triggered()), this, SLOT(setLanguageXml()));

    //! generate snapshot
    snapshotAction = new QAction(tr("&Snapshot"), this);
    snapshotAction->setStatusTip(tr("Generate snapshot"));
    connect( snapshotAction, SIGNAL(triggered()), this, SLOT(snapshot()));

    //! options
    QIcon optionIcon(":/icons/seeting.png");
    optionsAction = new QAction(optionIcon,tr("&Options"), this);
    optionsAction->setStatusTip(tr("Setting main funkcionality"));
    connect(optionsAction, SIGNAL(triggered()), this, SLOT(options()));

    //! sw metrics
    QIcon metricsIcon(":/icons/chart.png");
    metricsAction = new QAction(metricsIcon,tr("&SW metrics"), this);
    metricsAction->setStatusTip(tr("Dispaly of sw metrics"));
    connect(metricsAction, SIGNAL(triggered()), this, SLOT(swMetrics()));

    //! task list
    QIcon taskIcon(":/icons/taskList.png");
    taskListAction = new QAction(taskIcon,tr("&Task list"), this);
    taskListAction->setStatusTip(tr("Show task list"));
    connect(taskListAction, SIGNAL(triggered()), this, SLOT(taskList()));

    //! bug list
    QIcon bugIcon(":/icons/bugList.png");
    bugListAction = new QAction(bugIcon,tr("&Bug list"), this);
    bugListAction->setStatusTip(tr("Show bug list"));
    connect(bugListAction, SIGNAL(triggered()), this, SLOT(bugList()));

    //! basic toolbar
    basicToolbarAction = new QAction(tr("&Basic"), this);
    basicToolbarAction->setStatusTip(tr("Set basic toolbar"));
    connect(basicToolbarAction, SIGNAL(triggered()), this, SLOT(basicToolbar()));

    //! format toolbar
    formatToolbarAction = new QAction(tr("&Format"), this);
    formatToolbarAction->setStatusTip(tr("Set format toolbar"));
    connect(formatToolbarAction, SIGNAL(triggered()), this, SLOT(formatToolbars()));

    //! tools toolbar
    toolsToolbarAction = new QAction(tr("&Tools"), this);
    toolsToolbarAction->setStatusTip(tr("Set tools toolbar"));
    connect(toolsToolbarAction, SIGNAL(triggered()), this, SLOT(toolsToolbar()));

    //! editor toolbar
    editorToolbarAction = new QAction(tr("&Editor"), this);
    editorToolbarAction->setStatusTip(tr("Set editor toolbar"));
    connect(editorToolbarAction, SIGNAL(triggered()), this, SLOT(editorToolbar()));

    //! bottom dock panel
    setBottomDockAction = new QAction(tr("&Bottom dock"), this);
    setBottomDockAction->setStatusTip(tr("View bottom dock panel"));
    setBottomDockAction->setCheckable(true);
    connect(setBottomDockAction, SIGNAL(triggered()), this, SLOT(setBottomDock()));

    //! bottom right dock panel
    setRightDockAction = new QAction(tr("&Right dock"), this);
    setRightDockAction->setStatusTip(tr("View right dock panel"));
    setRightDockAction->setCheckable(true);
    connect(setRightDockAction, SIGNAL(triggered()), this, SLOT(setRightDock()));

    //! fullscreen
    QIcon fullScreenIcon(":/icons/fullScreen.png");
    fullScreenAction = new QAction(fullScreenIcon,tr("&FullScreen"), this);
    fullScreenAction->setShortcut(tr("F8"));
    fullScreenAction->setCheckable(true);
    fullScreenAction->setStatusTip(tr("View full screen"));
    connect(fullScreenAction, SIGNAL(triggered()), this, SLOT(fullScreen()));

    //! new window
    newWindowAction = new QAction(tr("&New window"), this);
    newWindowAction->setStatusTip(tr("Create new instance an application "));
    connect(newWindowAction, SIGNAL(triggered()), this, SLOT(newWindow()));

    //! zoom in
    QIcon zoomIncon(":/icons/plus.png");
    zoomInAction = new QAction(zoomIncon,tr("&Zoom In"), this);
    zoomInAction->setStatusTip(tr("Zoom in"));
    connect(zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));

    //! zoom out
    QIcon zoomOutIcon(":/icons/minus.png");
    zoomOutAction = new QAction(zoomOutIcon,tr("&Zoom Out"), this);
    zoomOutAction->setStatusTip(tr("Zoom out "));
    connect(zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));

    //! split
    splitAction = new QAction(tr("&Split"), this);
    splitAction->setStatusTip(tr("Split a workspace "));
    splitAction->setCheckable(true);
    connect(splitAction, SIGNAL(triggered()), this, SLOT(split()));

    //! CMD
    QIcon cmdIcon(":/icons/cmd.png");
    showCmdAction = new QAction(cmdIcon,tr("&CMD"), this);
    showCmdAction->setCheckable(true);
    showCmdAction->setStatusTip(tr("Run command line"));
    connect(showCmdAction, SIGNAL(triggered()), this, SLOT(showCmd()));

    //! undo
    QIcon undoIcon(":/icons/undo.png");
    undoAction = new QAction(undoIcon, tr("&Undo"), this);
    undoAction->setShortcut(tr("CTRL+Z"));
    undoAction->setStatusTip(tr("Undo"));
    undoAction->setEnabled(false);
    connect(MainWindow::undoAction, SIGNAL(triggered()), this, SLOT(undo()));
    actionList.append(undoAction);

    //! redo
    QIcon redoIcon(":/icons/redo.png");
    redoAction = new QAction(redoIcon, tr("&Redo"), this);
    redoAction->setShortcut(tr("CTRL+Y"));
    redoAction->setStatusTip(tr("Redo"));
    redoAction->setEnabled(false);
    connect(redoAction, SIGNAL(triggered()), this, SLOT(redo()));
    actionList.append(redoAction);

    //! cut
    QIcon cutIcon(":/icons/cut.png");
    cutAction = new QAction(cutIcon, tr("&Cut"), this);
    cutAction->setShortcut(tr("CTRL+X"));
    cutAction->setStatusTip(tr("Cut"));
    cutAction->setEnabled(false);
    connect(cutAction, SIGNAL(triggered()), this, SLOT(cut()));
    actionList.append(cutAction);

    //! copy
    QIcon copyIcon(":/icons/copy.png");
    copyAction = new QAction(copyIcon,tr("&Copy"), this);
    copyAction->setShortcut(tr("CTRL+C"));
    copyAction->setStatusTip(tr("Copy"));
    copyAction->setEnabled(false);
    connect(copyAction, SIGNAL(triggered()), this, SLOT(copy()));
    actionList.append(copyAction);

    //! paste
    QIcon pasteIcon(":/icons/paste.png");
    pasteAction = new QAction(pasteIcon,tr("&Paste"), this);
    pasteAction->setShortcut(tr("CTRL+V"));
    pasteAction->setStatusTip(tr("Paste"));
    pasteAction->setEnabled(false);
    connect(pasteAction, SIGNAL(triggered()), this, SLOT(paste()));
    actionList.append(pasteAction);

    //! delete
    QIcon deleteIcon(":/icons/delete.png");
    deleteAction = new QAction(deleteIcon,tr("&Delete"), this);
    deleteAction->setShortcut(tr("DEL"));
    deleteAction->setStatusTip(tr("Delete"));
    deleteAction->setEnabled(false);
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(delet()));
    actionList.append(deleteAction);

    //! selectAll
    selectAllAction = new QAction(tr("&Select All"), this);
    selectAllAction->setShortcut(tr("CTRL+A"));
    selectAllAction->setStatusTip(tr("Select All"));
    selectAllAction->setEnabled(false);
    connect(selectAllAction, SIGNAL(triggered()), this, SLOT(selectAll()));
    actionList.append(selectAllAction);

    //! attach file
    QIcon attachIcon(":/icons/spin.png");
    attachFileAction = new QAction(attachIcon,tr("&Attach file"), this);
    attachFileAction->setStatusTip(tr("Attach file"));
    connect(attachFileAction, SIGNAL(triggered()), this, SLOT(attachFile()));

    //! find
    QIcon findIcon(":/icons/find.png");
    findAction = new QAction(findIcon,tr("&Find"), this);
    findAction->setShortcut(tr("CTRL+F"));
    findAction->setStatusTip(tr("Find"));
    connect(findAction, SIGNAL(triggered()), this, SLOT(find()));

    //! find & replace
    find_ReplaceAction = new QAction(tr("&Find & Replace"), this);
    find_ReplaceAction->setShortcut(tr("CTRL+R"));
    find_ReplaceAction->setStatusTip(tr("Find and Replace"));
    connect(find_ReplaceAction, SIGNAL(triggered()), this, SLOT(find_Replace()));

    //! set bold font
    QIcon boldIcon(":/icons/bold.png");
    setBoldAction = new QAction(boldIcon,tr("&Bold font"), this);
    setBoldAction->setStatusTip(tr("Set bold font"));
    connect(setBoldAction, SIGNAL(triggered()), this, SLOT(setBold()));

    //! set italic font
    QIcon italicIcon(":/icons/italic.png");
    setItalicAction = new QAction(italicIcon,tr("&italic font"), this);
    setItalicAction->setStatusTip(tr("Set italic font"));
    connect(setItalicAction, SIGNAL(triggered()), this, SLOT(setItalic()));

    //! show printable area
    QIcon areaIcon(":/icons/printArea.png");
    printableAreaAction = new QAction(areaIcon, tr("Show Printable Area"), this);
    printableAreaAction->setToolTip(tr("Show margins of printable area"));
    connect(printableAreaAction, SIGNAL(triggered()), this, SLOT(showPrintableArea()));
    file.close();
}

//! Items in MenuBar

QList<QAction *> MainWindow::getActionList()
{
    return actionList;
}

void MainWindow::createMenus()
{
    //! file menu
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

    //! edit menu
    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);
    editMenu->addAction(cutAction);
    editMenu->addAction(copyAction);
    editMenu->addAction(pasteAction);
    editMenu->addAction(deleteAction);
    editMenu->addAction(selectAllAction);
    editMenu->addSeparator();
    editMenu->addAction(findAction);
    editMenu->addAction(find_ReplaceAction);

    //! View menu
    viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(newWindowAction);
    viewMenu->addAction(plainEditAction);
    viewMenu->addAction(fullScreenAction);
    viewMenu->addAction(splitAction);
    viewMenu->addSeparator();
    //! submenu toolbars
    setToolbarsMenu = viewMenu->addMenu("&Toolbars");
    setToolbarsMenu->addAction(basicToolbarAction);
    setToolbarsMenu->addAction(formatToolbarAction);
    setToolbarsMenu->addAction(toolsToolbarAction);
    setToolbarsMenu->addAction(editorToolbarAction);
    //! submenu panels
    panelsMenu = viewMenu->addMenu("&Output panels");
    panelsMenu->addAction(setBottomDockAction);
    panelsMenu->addAction(setRightDockAction);
    viewMenu->addSeparator();
    viewMenu->addAction(zoomInAction);
    viewMenu->addAction(zoomOutAction);

    //! tolls menu
    tollsMenu = menuBar()->addMenu(tr("&Tools"));
    tollsMenu->addAction(taskListAction);
    tollsMenu->addAction(bugListAction);
    tollsMenu->addAction(showCmdAction);
    tollsMenu->addAction(metricsAction);
    tollsMenu->addSeparator();
    //! submenu language
    languageMenu = tollsMenu->addMenu(tr("&Language"));
    languageMenu->addAction(setCAction);
    languageMenu->addAction(setLuaAction);
    languageMenu->addAction(setXmlAction);
    //! submenu generate
    generateMenu= tollsMenu->addMenu(tr("&Generate to"));
    generateMenu->addAction(printPdfAction);
    generateMenu->addAction(snapshotAction);
    
    tollsMenu->addAction(shortAction);
    
    tollsMenu->addSeparator();
    tollsMenu->addAction(optionsAction);

    //! help menu
    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(homePageAction);
    helpMenu->addSeparator();
    helpMenu->addAction(helpAction);
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(tipsAction);
    helpMenu->addSeparator();
    helpMenu->addAction(updateAction);
    helpMenu->addAction(bugReportAction);
}

//! functions for Shortcusts --------------------------------------------------------------------------

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

    QFile file(":/files/shortcutsFile");
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
    QFile file(":/files/shortcutsFile");
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
//! End functions for Shortucts -------------------------------------------------------------------------------


void MainWindow::createToolBars()
{
    try
    {
        //! basic toolbar
        formatToolBar = addToolBar(tr("Basic"));
        formatToolBar->setMovable(true);

        formatToolBar->addAction(newAction);
        formatToolBar->addAction(openAction);
        formatToolBar->addAction(saveAction);
        formatToolBar->addAction(closeAction);
        formatToolBar->addSeparator();

        formatToolBar->addAction(plainEditAction);
        formatToolBar->addAction(printableAreaAction);
        formatToolBar->addAction(printPdfAction);
        
        formatToolBar->addSeparator();

        scriptsBox = new QComboBox();
        scriptsBox->setMaxVisibleItems(10);
        //! Style Combox for language
        scriptsBox->setStyle(new QPlastiqueStyle);


        scriptsBox->addItems(langManager->getLanguages());
        connect(scriptsBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(langChanged(QString)));
        formatToolBar->addWidget(scriptsBox);
        formatToolBar->addSeparator();

        searchLineEdit = new QLineEdit();
        searchLineEdit->setFixedSize(150, 20);
        searchLineEdit->setToolTip(tr("For result press Enter"));
        searchLineEdit->setText("search");
        searchLineEdit->setStyleSheet( "QLineEdit{"
                                       "color: black;"
                                       "font-style: italic;"
                                       "border-radius: 5px;"
                                       "}");
        connect(searchLineEdit, SIGNAL(editingFinished()), this, SLOT(search()));
        formatToolBar->addWidget(searchLineEdit);
        formatToolBar->addAction(clearAction);
        formatToolBar->addAction(aboutAction);
        formatToolBar->addAction(helpAction);
    }

    catch(...)
    {
        QMessageBox::information(this,"Error","Error somewhere!");
    }
}
   
void MainWindow::createEditorToolbars()
{
    try
    {
        //! editor toolbar
        editorToolbars = addToolBar(tr("Editor"));
        editorToolbars->setMovable(true);
        editorToolbars->addAction(undoAction);
        editorToolbars->addAction(redoAction);
        editorToolbars->addSeparator();
        editorToolbars->addAction(copyAction);
        editorToolbars->addAction(cutAction);
        editorToolbars->addAction(pasteAction); 
        editorToolbars->addAction(deleteAction);
        editorToolbars->addAction(attachFileAction);
    }

    catch(...)
    {
        QMessageBox::information(this,"Error","Editor toolbars!");
    }
}

//! for attach file as img ...
void MainWindow::attachFile()
{
    QMessageBox::information(this,"title","On Function is working!");
}


void MainWindow::createToolsToolbars()
{
    try
    {
        //! tools toolbar
        toolsToolbars = addToolBar(tr("Tools"));
        toolsToolbars->setMovable(true);
        toolsToolbars->addAction(taskListAction);
        toolsToolbars->addAction(bugListAction);
        toolsToolbars->addAction(showCmdAction);
        toolsToolbars->addAction(metricsAction);
        toolsToolbars->addSeparator();
        toolsToolbars->addAction(zoomOutAction);
        toolsToolbars->addAction(zoomInAction);
        // pridat notepad
    }

    catch(...)
    {
        QMessageBox::information(this,"Error","Tools toolbars!");
    }
}

void MainWindow::createFormatingToolbars()
{
    try
    {
        //! format toolbar
        formatingToolbars = addToolBar(tr("Format"));
        formatingToolbars->setMovable(true);
        setFont = new QFontComboBox();
        setFont->setStyle(new QPlastiqueStyle);
        connect(setFont,SIGNAL(currentIndexChanged(QString)), this, SLOT(changeFont(QString)));
        formatingToolbars->addWidget(setFont);

        setSizeFont = new QComboBox();
        setSizeFont->setStyle(new QPlastiqueStyle);
        setSizeFont->addItem("6");
        setSizeFont->addItem("8");
        setSizeFont->addItem("10");
        setSizeFont->addItem("12");
        setSizeFont->addItem("14");
        setSizeFont->addItem("16");
        setSizeFont->addItem("18");
        setSizeFont->addItem("20"); // lepšie asi ako spinbox
        connect(setSizeFont,SIGNAL(currentIndexChanged(QString)), this, SLOT(sizeFont(QString)));
        formatingToolbars->addWidget(setSizeFont);
        formatingToolbars->addAction(setBoldAction);
        formatingToolbars->addAction(setItalicAction);
    }

    catch(...)
    {
        QMessageBox::information(this,"Error","Format toolbars!");
    }
}

void MainWindow::changeFont()
{
   QMessageBox::information(this,"title","On Function is working!");
}

void MainWindow::sizeFont()
{
   QMessageBox::information(this,"title","On Function is working!");
}

void MainWindow::setBold()
{
   QMessageBox::information(this,"title","On Function is working!");
}

void MainWindow::setItalic()
{
   QMessageBox::information(this,"title","On Function is working!");
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
    //! check tab, if tab is Start page open new tab
    if((tabWidget->tabText(tabWidget->currentIndex())).compare("Start page")==0)
    {
        newTab();
        newFile();
    }
    else
    {
        DocumentScene* dScene=getScene();
        if(dScene==0)  // this should not ever happen
        {
            qDebug("newFile() Error: dScene = null");
            return;
        }
        else
        {
            dScene->newGroup(scriptsBox->currentText());
            setCurrentFile(dScene->selectedGroup());
            scriptsBox->setCurrentIndex(0); // select item in scriptsBox on C (in future select previously used item etc...)
        }
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
    if((tabWidget->tabText(position)).compare("Start page")==0) return;
    else tabWidget->removeTab(position);
}

void MainWindow::tabChanged(int position)
{
    BlockGroup *group=getScene()->selectedGroup();
        if(group==0){
            setCurrentFile(0);
            return;
        }
        else
        {
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

    //! button addTab
    QIcon addTabIcon(":/icons/plusTab.png");
    QPushButton *m_addButton = new QPushButton(addTabIcon,"", this);
    m_addButton->setObjectName("addButton");

    //! set style for buton- add tab
    m_addButton->setStyleSheet("background-color: transparent;");

    connect(m_addButton, SIGNAL(clicked()), this, SLOT(newTab()));
    tabWidget->setCornerWidget(m_addButton, Qt::TopLeftCorner);

    QWidget* widget=createView(); // get QGraphicView
    QGraphicsView* view=(QGraphicsView *) widget;
    DocumentScene* dScene=(DocumentScene *) view->scene();
    dScene->main=this;

    dScene->addPixmap(QPixmap(":/img/startScreen"));
    QPixmap obr(":/img/newWay");
    dScene->addPixmap(obr)->setPos(184,335);
    createMenus();
    createToolBars();
    statusBar();

    QIcon homeIcon(":/icons/home.png");
    tabWidget->addTab(widget,homeIcon, "Start page");
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
//        scriptsBox->setEnabled(true);
    }
    else
    {
        groupActions->setEnabled(true);
        searchLineEdit->setEnabled(true);
//        scriptsBox->setEnabled(false);

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
    if((tabWidget->tabText(tabWidget->currentIndex())).compare("Start page")==0)
    {
        newTab();
        open();
    }
    else if (tabWidget->currentIndex()!=0)
    {
        QString fileFilters = tr("All files (*)");
        QString dir = QFileInfo(windowFilePath()).absoluteDir().absolutePath();
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"), dir, fileFilters); // CHECK
        open(fileName);
    }
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

//! search in main window
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

    //! frame setting
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


//! FUNTIONS FOR EDIT MENU ---------------------------------------------------------------------------------

void MainWindow::undo()
{
    getScene()->selectedGroup()->getTextGroup()->undo();
}

void MainWindow::redo()
{
    getScene()->selectedGroup()->getTextGroup()->redo();
}

void MainWindow::cut()
{
    getScene()->selectedGroup()->getTextGroup()->cut();
}

void MainWindow::copy()
{
    getScene()->selectedGroup()->getTextGroup()->copy();
}

void MainWindow::paste()
{
    getScene()->selectedGroup()->getTextGroup()->paste();
}

void MainWindow::delet()
{
    getScene()->selectedGroup()->getTextGroup()->deleteFunction();
}

void MainWindow::selectAll()
{
    getScene()->selectedGroup()->getTextGroup()->selectAll();
}

//! window for find keyword
void MainWindow::find()
{
  findWindow = new QDialog();
  findLabel = new QLabel("Inser keyword:");
  findLineEdit = new QLineEdit();
  connect(findLineEdit, SIGNAL(editingFinished()), this, SLOT(search2()));
  findLineEdit->setStyleSheet("QLineEdit{"
                              "color: black;"
                              "font-style: italic;"
                              "border-radius: 6px;"
                              "}");
  findButton = new QPushButton("Find");
  findButton->setStyle(new QPlastiqueStyle);

  layout = new QHBoxLayout(findWindow);
  layout->addWidget(findLabel);
  layout->addWidget(findLineEdit);
  layout->addWidget(findButton);
  findWindow->resize(430,40);
  findWindow->setWindowTitle("Find keyword");
  findWindow->show();
  // tlaèidlo find bude hladat dal
}

//! search for find
void MainWindow::search2()
{
    try
    {
        QString searchText2 = findLineEdit->text();
        getScene()->findText(searchText2);
    }
    catch (...)
    {
        QMessageBox::information(this,"Error","Error with Search Text!");
    }
}

void MainWindow::find_Replace()
{
  QMessageBox::information(this,"title","On Function is working!");
}

//! END OF FUNCTIONS FOR EDIT MENU -------------------------------------------------------------------------



//! FUNTIONS FOR VIEW MENU ---------------------------------------------------------------------------------


//! basic toolbar
void MainWindow::basicToolbar()
{
    QMessageBox::information(this,"title","On Function is working!");
}

//! formating toolbar
void MainWindow::formatToolbars()
{
    createFormatingToolbars();
}

//! tools toolbar
void MainWindow::toolsToolbar()
{
     createToolsToolbars();
}

//! editor toolbar
void MainWindow::editorToolbar()
{
    createEditorToolbars();
}

//! bottom dockpanel - for bug,task list
void MainWindow::setBottomDock()
{
    dock = new QDockWidget(tr("Buttom dock"), this);
    dock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    dock->setFeatures(QDockWidget::DockWidgetClosable);
    dock->setStyle(new QPlastiqueStyle);

    //! add tab to buttom dockpanel
    QTabWidget *tabView = new QTabWidget();
    tabView->addTab (new QTextEdit,("&Task list"));
    tabView->addTab(new QTextEdit,("&Bug list"));
    tabView->addTab(new QTextEdit,("&Note"));

    dock->setWidget(tabView);
    addDockWidget(Qt::BottomDockWidgetArea, dock);
}

//! right dockpanel - for fileBrowser
void MainWindow::setRightDock()
{
      dock1 = new QDockWidget(tr("File browser tree"), this);
      dock1->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
      dock1->setFeatures(QDockWidget::DockWidgetClosable);
      dock1->setStyle(new QPlastiqueStyle);

      model = new QDirModel();
      tree = new QTreeView(dock1);
      tree->setModel(model);
      dock1->setWidget(tree);

      addDockWidget(Qt::RightDockWidgetArea, dock1);
}

//! full screen
void MainWindow::fullScreen()
{
    try
    {
        this->showMaximized();
        formatToolBar->hide();
        /*editorToolbars->hide();
        toolsToolbars->hide();
        formatingToolbars->hide();*/
    }
    catch(...)
    {
        //nothing
    }
}

//! new window
void MainWindow::newWindow()
{
    QMessageBox::information(this,"title","On Function is working!");
}

//! zoom in
void MainWindow::zoomIn()
{
    getScene()->adjustScale(1.2);
}

//! zoom out
void MainWindow::zoomOut()
{
    getScene()->adjustScale(-1.2);
}

//! split
void MainWindow::split()
{
    this->showNormal();
    formatToolBar->show();
}

//! END OF FUNCTIONS FOR VIEW MENU ------------------------------------------------------------------------



//! FUNTIONS FOR TOOLS MENU ---------------------------------------------------------------------------------

//! set Lua language
void MainWindow:: setLanguageLua()
{
    QMessageBox::information(this,"title","On Function is working!");
}

//! set C language
void MainWindow:: setLanguageC()
{
    QMessageBox::information(this,"title","On Function is working!");
}

//! set Xml language
void MainWindow:: setLanguageXml()
{
    QMessageBox::information(this,"title","On Function is working!");
}

//! genarate to snapshot
void MainWindow::snapshot()
{
    QMessageBox::information(this,"title","On Function is working!");
}

//! task list
void MainWindow::taskList()
{
    setBottomDock();
}

//! bug list
void MainWindow::bugList()
{
    setBottomDock();
}

//! options
void MainWindow::options()
{
    setting *windowOption = new setting;
    windowOption->showSetting();
}

//! sw metrics
void MainWindow::swMetrics()
{
    QMessageBox::information(this,"title","On Function is working!");
}

//! rum CMD
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

//! END OF FUNCTIONS FOR TOOLS MENU ------------------------------------------------------------------------




//! FUNTIONS FOR HELP MENU ----------------------------------------------------------------------------------

//! about TrollEdit
void MainWindow::about()
{
    aboutTrollEdit *aboutWindow = new aboutTrollEdit;
    aboutWindow->showWindow();
}

//! show TrollEdit web page
void MainWindow::homePage()
{
    QDesktopServices::openUrl(QUrl("http://innovators-team10.github.com"));
}

//! html help in new tab
void MainWindow::help()
{
        newTab();
        QWebView *view = new QWebView();
        QToolBar *helpToolBar = new QToolBar(tr("&Navigation"),view);

        helpToolBar->addAction(view->pageAction(QWebPage::Back));
        helpToolBar->addAction(view->pageAction(QWebPage::Forward));
        helpToolBar->addAction(view->pageAction(QWebPage::Reload));
        helpToolBar->addAction(view->pageAction(QWebPage::Stop));
        view->load(QUrl("http://innovators-team10.github.com/u-manual_simple.html"));
        view->resize(1000,500);
        tabWidget->setTabText(tabWidget->currentIndex(),"On-line help");
        QIcon helpsIcon(":/icons/help.png");
        tabWidget->setTabIcon(tabWidget->currentIndex(),helpsIcon);
        DocumentScene* dScene=getScene();
        dScene->addWidget(view);
}

//! check new update
void MainWindow::update()
{
    QMessageBox::information(this,"title","On Function is working!");
}


//! show Tips and Tricks
void MainWindow::tipsAndTricks()
{
    tips_tricks *aboutWindow = new tips_tricks;
    aboutWindow->showWindow();
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


void MainWindow::closeEvent(QCloseEvent *event)
{
    getScene()->closeAllGroups();
    writeSettings();
    event->accept();
}


//! set size of Mainwindow
void MainWindow::readSettings()
{
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    QPoint pos = settings.value("pos", QPoint(100, 100)).toPoint();
    QSize size = settings.value("size", QSize(500, 300)).toSize();
    resize(size);
    move(pos);

    if (settings.value("minimized", false).toBool())
        showMinimized();
}

void MainWindow::writeSettings()
{
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("minimized", isMinimized());
}
