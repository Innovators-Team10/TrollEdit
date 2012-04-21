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
<<<<<<< HEAD
=======
#include "setting.h"
#include "abouttrolledit.h"
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
#include "analyzer.h"
#include "block_group.h"
#include <QTableWidget>
#include <QFont>
#include <QPushButton>
#include <QtGui>
#include <QString>
#include <QTextStream>
#include <QtWebKit>


MainWindow::MainWindow(QString programPath, QWidget *parent) : QMainWindow(parent)
{
    initLuaState();
    langManager = new LanguageManager(programPath);

    createActions();
    createTabs();

    readSettings();

    updateRecentFileActions();
}

//! create lua state
void MainWindow::initLuaState(){
    this->L = luaL_newstate();
    //...
}

//! give lua state with configuration
lua_State* MainWindow::getLuaState(){
    return this->L;
}

<<<<<<< HEAD
// wrapper slots
// in most functions is dynamically detected current BlockGroup,
// so they are called with parameter 0
=======
//! wrapper slots
//! in most functions is dynamically detected current BlockGroup,
//! so they are called with parameter 0
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
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

<<<<<<< HEAD
    // loading file for shurtcuts
    QFile file(":/files/shortcuts.ini");
=======
    //! loading file for shurtcuts
    QFile file(":/files/shortcutsFile");
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::information(0,"Error with shortcuts",file.errorString());
    }

    QString textstring;


<<<<<<< HEAD
    // new
=======
    //! new
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    QIcon newIcon(":/icons/new.png");
    newAction = new QAction(newIcon, tr("&New"), this);
    textstring = file.readLine();
    textstring.remove(6,1);
    newAction->setShortcut((textstring));
    newAction->setToolTip(tr("Create a new file"));
    connect(newAction, SIGNAL(triggered()), this, SLOT(newFile()));
    addAction(newAction);

<<<<<<< HEAD
    // open
=======
    //! open
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    QIcon openIcon(":/icons/open.png");
    openAction = new QAction(openIcon, tr("&Open..."), this);
    textstring = file.readLine();
    textstring.remove(6,1);
    openAction->setShortcut((textstring));
    openAction->setToolTip(tr("Open an existing file"));
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

<<<<<<< HEAD
    // revert
    revertAction = new QAction(tr("&Revert"), this); // ??? is this used ???
//    QIcon revertIcon(":/m/open"); openIcon.addFile(":/s/open");
    //textstring = file.readLine();

    // QIcon revertIcon(":/m/open"); openIcon.addFile(":/s/open");
=======
    //! revert
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    revertAction = new QAction(tr("&Revert"), this);
    textstring = file.readLine();
    textstring.remove(6,1);
    revertAction->setShortcut((textstring));
    revertAction->setToolTip(tr("Revert to last save"));
    connect(revertAction, SIGNAL(triggered()), this, SLOT(revertGroupWrapper()));
    groupActions->addAction(revertAction);


<<<<<<< HEAD
    // save
=======
    //! save
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    QIcon saveIcon(":/icons/save.png"); // works (only for 1 scene)
    saveAction = new QAction(saveIcon, tr("&Save"), this);
    textstring = file.readLine();
    textstring.remove(6,1);
    saveAction->setShortcut((textstring));
    saveAction->setToolTip(tr("Save file"));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveGroupWrapper()));
    groupActions->addAction(saveAction);


<<<<<<< HEAD
    // save as
=======
    //! save as
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    QIcon saveAsIcon(":/icons/save.png");  // probably same as saveAction
    saveAsAction = new QAction(saveAsIcon, tr("Save &As..."), this);
    saveAsAction->setToolTip(tr("Save file as..."));
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveGroupAsWrapper()));
    groupActions->addAction(saveAsAction);


        saveAsNoDocAction = new QAction(tr("Save Without Comments"), this); // ??? is this used ???
        saveAsNoDocAction->setToolTip(tr("Save file without any comments"));
        connect(saveAsNoDocAction, SIGNAL(triggered()), this, SLOT(saveGroupAsWithoutDocWrapper()));
        groupActions->addAction(saveAsNoDocAction);

<<<<<<< HEAD
        // save all
=======
        //! save all
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
        QIcon saveAllIcon(":/icons/saveAll.png");
        saveAllAction = new QAction(saveAllIcon,tr("Save All"), this); // works for 1 tab ??? does saveAllGroups work as it should ???
        saveAllAction->setToolTip(tr("Save all files"));
        connect(saveAllAction, SIGNAL(triggered()), this, SLOT(saveAllGroupsWrapper()));

<<<<<<< HEAD
        // close
=======
        //! close
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
        QIcon closeIcon(":/icons/closeFile.png");
        closeAction = new QAction(closeIcon, tr("&Close File"), this);
        textstring = file.readLine();
        textstring.remove(6,1);
        closeAction->setShortcut((textstring));
        closeAction->setToolTip(tr("Close file"));
        connect(closeAction, SIGNAL(triggered()), this, SLOT(closeGroupWrapper()));
        groupActions->addAction(closeAction);

<<<<<<< HEAD
        // close all
=======
        //! close all
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
        closeAllAction = new QAction(tr("Close All"), this);
        closeAllAction->setToolTip(tr("Close all files"));
        connect(closeAllAction, SIGNAL(triggered()), this, SLOT(closeAllGroupsWrapper()));

<<<<<<< HEAD
        // print pdf
=======
        //! print pdf
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
        QIcon printIcon(":/icons/print.png");
        printPdfAction = new QAction(printIcon, tr("&Print PDF"), this);
        textstring = file.readLine();
        textstring.remove(6,1);
        printPdfAction->setShortcut((textstring));
        printPdfAction->setToolTip(tr("Print scene to PDF"));
        connect(printPdfAction, SIGNAL(triggered()), this, SLOT(printPdf()));
        groupActions->addAction(printPdfAction);

<<<<<<< HEAD
        // show plain text editor
=======
        //! show plain text editor
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
        QIcon editIcon(":/icons/textMode");
        plainEditAction = new QAction(editIcon, tr("&Edit Plain Text"), this);
        textstring = file.readLine();
        textstring.remove(6,1);
        plainEditAction->setShortcut((textstring));
        plainEditAction->setToolTip(tr("Edit file as plain text"));
        connect(plainEditAction, SIGNAL(triggered()), this, SLOT(showPreviewWrapper()));
        groupActions->addAction(plainEditAction);

<<<<<<< HEAD
        // new Tab
        newTabAction = new QAction(this);
        textstring = file.readLine();
        textstring.remove(6,1);
        newTabAction->setShortcut((textstring));
        connect(newTabAction, SIGNAL(triggered()), this, SLOT(newTab()));
        addAction(newTabAction);

        // clear search results
=======
        //! clear search results
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
        QIcon clearIcon(":/icons/close");
        clearAction = new QAction(clearIcon, tr("Clea&n Search"), this);
        //clearAction->setShortcut(tr("CTRL+S"));
        clearAction->setToolTip(tr("Clean search results"));
        connect(clearAction, SIGNAL(triggered()), this, SLOT(cleanGroupWrapper()));
        groupActions->addAction(clearAction);


<<<<<<< HEAD
    // recent files
=======
    //! recent files
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    for (int i = 0; i < MaxRecentFiles; ++i)
    {
        recentFileActions[i] = new QAction(this);
        recentFileActions[i]->setVisible(false);
        connect(recentFileActions[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }

<<<<<<< HEAD
    // exit
=======
    //! exit
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    QIcon exitIcon(":/icons/exit.png");
    exitAction = new QAction(exitIcon, tr("E&xit"), this);
    exitAction->setToolTip(tr("Quit the application?"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

<<<<<<< HEAD
    // help
=======
    //! help
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    QIcon helpIcon(":/icons/help.png");
    helpAction = new QAction(helpIcon, tr("&Help - online"), this);
    helpAction->setShortcut(tr("F1"));
    helpAction->setToolTip(tr("Show application help"));
    connect(helpAction, SIGNAL(triggered()), this, SLOT(help()));

<<<<<<< HEAD
    // about
=======
    //! about
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    QIcon aboutIcon(":/icons/info.png");
    aboutAction = new QAction(aboutIcon,tr("&About"), this);
    aboutAction->setToolTip(tr("Show application's about box"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

<<<<<<< HEAD
    // license
    showLicenseAction = new QAction(tr("&License"), this);
    showLicenseAction->setToolTip(tr("TrollEdit license"));
    connect(showLicenseAction, SIGNAL(triggered()), this, SLOT(showLicense()));

    // update
=======
    //! update
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    updateAction = new QAction(tr("&Check for update"), this);
    updateAction->setToolTip(tr("Check new updates"));
    connect(updateAction, SIGNAL(triggered()), this, SLOT(update()));

<<<<<<< HEAD
    // home page
=======
    //! home page
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    QIcon homeIcon(":/icons/home.png");
    homePageAction = new QAction(homeIcon,tr("&Home page"), this);
    homePageAction->setToolTip(tr("Open home page of TrollEdit"));
    connect(homePageAction, SIGNAL(triggered()), this, SLOT(homePage()));

<<<<<<< HEAD
    // about this version
    versionAction = new QAction(tr("&About this version"), this);
    versionAction->setToolTip(tr("View news on this a version"));
    connect(versionAction, SIGNAL(triggered()), this, SLOT(aboutVersion()));

    // bugs report
=======
    //! bugs report
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    bugReportAction = new QAction(tr("&Report bug"), this);
    bugReportAction->setToolTip(tr("Send report about a bug"));
    connect(bugReportAction, SIGNAL(triggered()), this, SLOT(bugReport()));

<<<<<<< HEAD
    // shortcuts
=======
    //! shortcuts
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    shortAction = new QAction(tr("&Shortcuts"), this);
    shortAction->setStatusTip(tr("Setting shortcuts"));
    connect(shortAction, SIGNAL(triggered()), this, SLOT(setShort()));

<<<<<<< HEAD
    // set Lua language
=======
    //! set Lua language
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    setLuaAction = new QAction(tr("&Lua"), this);
    setLuaAction->setStatusTip(tr("Set Lua language"));
    connect(setLuaAction, SIGNAL(triggered()), this, SLOT(setLanguageLua()));

<<<<<<< HEAD
    // set C language
=======
    //! set C language
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    setCAction = new QAction(tr("&C"), this);
    setCAction->setStatusTip(tr("Set C language"));
    connect(setCAction, SIGNAL(triggered()), this, SLOT(setLanguageC()));

<<<<<<< HEAD
    // set Xml language
=======
    //! set Xml language
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    setXmlAction = new QAction(tr("&Xml"), this);
    setXmlAction->setStatusTip(tr("Set Xml language"));
    connect(setXmlAction, SIGNAL(triggered()), this, SLOT(setLanguageXml()));

<<<<<<< HEAD
    // generate snapshot
=======
    //! generate snapshot
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    snapshotAction = new QAction(tr("&Snapshot"), this);
    snapshotAction->setStatusTip(tr("Generate snapshot"));
    connect( snapshotAction, SIGNAL(triggered()), this, SLOT(snapshot()));

<<<<<<< HEAD
    // options
=======
    //! options
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    QIcon optionIcon(":/icons/seeting.png");
    optionsAction = new QAction(optionIcon,tr("&Options"), this);
    optionsAction->setStatusTip(tr("Setting main funkcionality"));
    connect(optionsAction, SIGNAL(triggered()), this, SLOT(options()));

<<<<<<< HEAD
    // sw metrics
=======
    //! sw metrics
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    QIcon metricsIcon(":/icons/chart.png");
    metricsAction = new QAction(metricsIcon,tr("&SW metrics"), this);
    metricsAction->setStatusTip(tr("Dispaly of sw metrics"));
    connect(metricsAction, SIGNAL(triggered()), this, SLOT(swMetrics()));

<<<<<<< HEAD
    // task list
=======
    //! task list
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    QIcon taskIcon(":/icons/taskList.png");
    taskListAction = new QAction(taskIcon,tr("&Task list"), this);
    taskListAction->setStatusTip(tr("Show task list"));
    connect(taskListAction, SIGNAL(triggered()), this, SLOT(taskList()));

<<<<<<< HEAD
    // bug list
=======
    //! bug list
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    QIcon bugIcon(":/icons/bugList.png");
    bugListAction = new QAction(bugIcon,tr("&Bug list"), this);
    bugListAction->setStatusTip(tr("Show bug list"));
    connect(bugListAction, SIGNAL(triggered()), this, SLOT(bugList()));

<<<<<<< HEAD
    // basic toolbar
=======
    //! basic toolbar
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    basicToolbarAction = new QAction(tr("&Basic"), this);
    basicToolbarAction->setStatusTip(tr("Set basic toolbar"));
    connect(basicToolbarAction, SIGNAL(triggered()), this, SLOT(basicToolbar()));

<<<<<<< HEAD
    // format toolbar
=======
    //! format toolbar
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    formatToolbarAction = new QAction(tr("&Format"), this);
    formatToolbarAction->setStatusTip(tr("Set format toolbar"));
    connect(formatToolbarAction, SIGNAL(triggered()), this, SLOT(formatToolbars()));

<<<<<<< HEAD
    // tools toolbar
=======
    //! tools toolbar
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    toolsToolbarAction = new QAction(tr("&Tools"), this);
    toolsToolbarAction->setStatusTip(tr("Set tools toolbar"));
    connect(toolsToolbarAction, SIGNAL(triggered()), this, SLOT(toolsToolbar()));

<<<<<<< HEAD
    // editor toolbar
=======
    //! editor toolbar
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    editorToolbarAction = new QAction(tr("&Editor"), this);
    editorToolbarAction->setStatusTip(tr("Set editor toolbar"));
    connect(editorToolbarAction, SIGNAL(triggered()), this, SLOT(editorToolbar()));

<<<<<<< HEAD
    // bottom dock panel
=======
    //! bottom dock panel
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    setBottomDockAction = new QAction(tr("&Bottom dock"), this);
    setBottomDockAction->setStatusTip(tr("View bottom dock panel"));
    setBottomDockAction->setCheckable(true);
    connect(setBottomDockAction, SIGNAL(triggered()), this, SLOT(setBottomDock()));

<<<<<<< HEAD
    // bottom right dock panel
=======
    //! bottom right dock panel
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    setRightDockAction = new QAction(tr("&Right dock"), this);
    setRightDockAction->setStatusTip(tr("View right dock panel"));
    setRightDockAction->setCheckable(true);
    connect(setRightDockAction, SIGNAL(triggered()), this, SLOT(setRightDock()));

<<<<<<< HEAD
    // fullscreen
=======
    //! fullscreen
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    QIcon fullScreenIcon(":/icons/fullScreen.png");
    fullScreenAction = new QAction(fullScreenIcon,tr("&FullScreen"), this);
    fullScreenAction->setShortcut(tr("F8"));
    fullScreenAction->setCheckable(true);
    fullScreenAction->setStatusTip(tr("View full screen"));
    connect(fullScreenAction, SIGNAL(triggered()), this, SLOT(fullScreen()));

<<<<<<< HEAD
    // new window
=======
    //! new window
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    newWindowAction = new QAction(tr("&New window"), this);
    newWindowAction->setStatusTip(tr("Create new instance an application "));
    connect(newWindowAction, SIGNAL(triggered()), this, SLOT(newWindow()));

<<<<<<< HEAD
    // zoom in
=======
    //! zoom in
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    QIcon zoomIncon(":/icons/plus.png");
    zoomInAction = new QAction(zoomIncon,tr("&Zoom In"), this);
    zoomInAction->setStatusTip(tr("Zoom in"));
    connect(zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));

<<<<<<< HEAD
    // zoom out
=======
    //! zoom out
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    QIcon zoomOutIcon(":/icons/minus.png");
    zoomOutAction = new QAction(zoomOutIcon,tr("&Zoom Out"), this);
    zoomOutAction->setStatusTip(tr("Zoom out "));
    connect(zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));

<<<<<<< HEAD
    // split
=======
    //! split
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    splitAction = new QAction(tr("&Split"), this);
    splitAction->setStatusTip(tr("Split a workspace "));
    splitAction->setCheckable(true);
    connect(splitAction, SIGNAL(triggered()), this, SLOT(split()));

<<<<<<< HEAD
    // CMD
=======
    //! CMD
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    QIcon cmdIcon(":/icons/cmd.png");
    showCmdAction = new QAction(cmdIcon,tr("&CMD"), this);
    showCmdAction->setCheckable(true);
    showCmdAction->setStatusTip(tr("Run command line"));
    connect(showCmdAction, SIGNAL(triggered()), this, SLOT(showCmd()));

<<<<<<< HEAD
    // undo
    QIcon undoIcon(":/icons/undo.png");
    undoAction = new QAction(undoIcon, tr("&Undo"), this);
    textstring = file.readLine();
    textstring.remove(6,1);
    undoAction->setShortcut((textstring));
=======
    //! undo
    QIcon undoIcon(":/icons/undo.png");
    undoAction = new QAction(undoIcon, tr("&Undo"), this);
    undoAction->setShortcut(tr("CTRL+Z"));
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    undoAction->setStatusTip(tr("Undo"));
    undoAction->setEnabled(false);
    connect(MainWindow::undoAction, SIGNAL(triggered()), this, SLOT(undo()));
    actionList.append(undoAction);

<<<<<<< HEAD
    // redo
    QIcon redoIcon(":/icons/redo.png");
    redoAction = new QAction(redoIcon, tr("&Redo"), this);
    textstring = file.readLine();
    textstring.remove(6,1);
    redoAction->setShortcut((textstring));
=======
    //! redo
    QIcon redoIcon(":/icons/redo.png");
    redoAction = new QAction(redoIcon, tr("&Redo"), this);
    redoAction->setShortcut(tr("CTRL+Y"));
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    redoAction->setStatusTip(tr("Redo"));
    redoAction->setEnabled(false);
    connect(redoAction, SIGNAL(triggered()), this, SLOT(redo()));
    actionList.append(redoAction);

<<<<<<< HEAD
    // cut
    QIcon cutIcon(":/icons/cut.png");
    cutAction = new QAction(cutIcon, tr("&Cut"), this);
    textstring = file.readLine();
    textstring.remove(6,1);
    cutAction->setShortcut((textstring));
=======
    //! cut
    QIcon cutIcon(":/icons/cut.png");
    cutAction = new QAction(cutIcon, tr("&Cut"), this);
    cutAction->setShortcut(tr("CTRL+X"));
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    cutAction->setStatusTip(tr("Cut"));
    cutAction->setEnabled(false);
    connect(cutAction, SIGNAL(triggered()), this, SLOT(cut()));
    actionList.append(cutAction);

<<<<<<< HEAD
    // copy
    QIcon copyIcon(":/icons/copy.png");
    copyAction = new QAction(copyIcon,tr("&Copy"), this);
    textstring = file.readLine();
    textstring.remove(6,1);
    copyAction->setShortcut((textstring));
=======
    //! copy
    QIcon copyIcon(":/icons/copy.png");
    copyAction = new QAction(copyIcon,tr("&Copy"), this);
    copyAction->setShortcut(tr("CTRL+C"));
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    copyAction->setStatusTip(tr("Copy"));
    copyAction->setEnabled(false);
    connect(copyAction, SIGNAL(triggered()), this, SLOT(copy()));
    actionList.append(copyAction);

<<<<<<< HEAD
    // paste
    QIcon pasteIcon(":/icons/paste.png");
    pasteAction = new QAction(pasteIcon,tr("&Paste"), this);
    textstring = file.readLine();
    textstring.remove(6,1);
    pasteAction->setShortcut((textstring));
=======
    //! paste
    QIcon pasteIcon(":/icons/paste.png");
    pasteAction = new QAction(pasteIcon,tr("&Paste"), this);
    pasteAction->setShortcut(tr("CTRL+V"));
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    pasteAction->setStatusTip(tr("Paste"));
    pasteAction->setEnabled(false);
    connect(pasteAction, SIGNAL(triggered()), this, SLOT(paste()));
    actionList.append(pasteAction);

<<<<<<< HEAD
    // delete
    QIcon deleteIcon(":/icons/delete.png");
    deleteAction = new QAction(deleteIcon,tr("&Delete"), this);
    deleteAction->setShortcut(tr("DEL"));
    textstring = file.readLine();
    textstring.remove(6,1);
    deleteAction->setShortcut(textstring);
=======
    //! delete
    QIcon deleteIcon(":/icons/delete.png");
    deleteAction = new QAction(deleteIcon,tr("&Delete"), this);
    deleteAction->setShortcut(tr("DEL"));
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    deleteAction->setStatusTip(tr("Delete"));
    deleteAction->setEnabled(false);
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(delet()));
    actionList.append(deleteAction);

<<<<<<< HEAD
    // selectAll
    selectAllAction = new QAction(tr("&Select All"), this);
    textstring = file.readLine();
    textstring.remove(6,1);
    selectAllAction->setShortcut(textstring);
=======
    //! selectAll
    selectAllAction = new QAction(tr("&Select All"), this);
    selectAllAction->setShortcut(tr("CTRL+A"));
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    selectAllAction->setStatusTip(tr("Select All"));
    selectAllAction->setEnabled(false);
    connect(selectAllAction, SIGNAL(triggered()), this, SLOT(selectAll()));
    actionList.append(selectAllAction);

<<<<<<< HEAD
    //attach file
=======
    //! attach file
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    QIcon attachIcon(":/icons/spin.png");
    attachFileAction = new QAction(attachIcon,tr("&Attach file"), this);
    attachFileAction->setStatusTip(tr("Attach file"));
    connect(attachFileAction, SIGNAL(triggered()), this, SLOT(attachFile()));

<<<<<<< HEAD
    // find
    QIcon findIcon(":/icons/find.png");
    findAction = new QAction(findIcon,tr("&Find"), this);
    textstring = file.readLine();
    textstring.remove(6,1);
    findAction->setShortcut((textstring));
    findAction->setStatusTip(tr("Find"));
    connect(findAction, SIGNAL(triggered()), this, SLOT(find()));

    // find & replace
    find_ReplaceAction = new QAction(tr("&Find & Replace"), this);
    textstring = file.readLine();
    textstring.remove(6,1);
    find_ReplaceAction->setShortcut((textstring));
    find_ReplaceAction->setStatusTip(tr("Find and Replace"));
    connect(find_ReplaceAction, SIGNAL(triggered()), this, SLOT(find_Replace()));

    // set bold font
=======
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
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    QIcon boldIcon(":/icons/bold.png");
    setBoldAction = new QAction(boldIcon,tr("&Bold font"), this);
    setBoldAction->setStatusTip(tr("Set bold font"));
    connect(setBoldAction, SIGNAL(triggered()), this, SLOT(setBold()));

<<<<<<< HEAD
    // set italic font
=======
    //! set italic font
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    QIcon italicIcon(":/icons/italic.png");
    setItalicAction = new QAction(italicIcon,tr("&italic font"), this);
    setItalicAction->setStatusTip(tr("Set italic font"));
    connect(setItalicAction, SIGNAL(triggered()), this, SLOT(setItalic()));

<<<<<<< HEAD
    // show printable area
=======
    //! show printable area
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    QIcon areaIcon(":/icons/printArea.png");
    printableAreaAction = new QAction(areaIcon, tr("Show Printable Area"), this);
    printableAreaAction->setToolTip(tr("Show margins of printable area"));
    connect(printableAreaAction, SIGNAL(triggered()), this, SLOT(showPrintableArea()));
    file.close();
}

<<<<<<< HEAD
// Items in MenuBar
=======
//! Items in MenuBar
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59

QList<QAction *> MainWindow::getActionList()
{
    return actionList;
}

void MainWindow::createMenus()
{
<<<<<<< HEAD
    // file menu
=======
    //! file menu
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
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

<<<<<<< HEAD
    //edit menu
=======
    //! edit menu
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
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

<<<<<<< HEAD
    // View menu
=======
    //! View menu
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(newWindowAction);
    viewMenu->addAction(plainEditAction);
    viewMenu->addAction(fullScreenAction);
    viewMenu->addAction(splitAction);
    viewMenu->addSeparator();
<<<<<<< HEAD
    // submenu toolbars
=======
    //! submenu toolbars
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    setToolbarsMenu = viewMenu->addMenu("&Toolbars");
    setToolbarsMenu->addAction(basicToolbarAction);
    setToolbarsMenu->addAction(formatToolbarAction);
    setToolbarsMenu->addAction(toolsToolbarAction);
    setToolbarsMenu->addAction(editorToolbarAction);
<<<<<<< HEAD
    // submenu panels
=======
    //! submenu panels
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    panelsMenu = viewMenu->addMenu("&Output panels");
    panelsMenu->addAction(setBottomDockAction);
    panelsMenu->addAction(setRightDockAction);
    viewMenu->addSeparator();
    viewMenu->addAction(zoomInAction);
    viewMenu->addAction(zoomOutAction);

<<<<<<< HEAD
    // tolls menu
=======
    //! tolls menu
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    tollsMenu = menuBar()->addMenu(tr("&Tools"));
    tollsMenu->addAction(taskListAction);
    tollsMenu->addAction(bugListAction);
    tollsMenu->addAction(showCmdAction);
    tollsMenu->addAction(metricsAction);
    tollsMenu->addSeparator();
<<<<<<< HEAD
    // submenu language
=======
    //! submenu language
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    languageMenu = tollsMenu->addMenu(tr("&Language"));
    languageMenu->addAction(setCAction);
    languageMenu->addAction(setLuaAction);
    languageMenu->addAction(setXmlAction);
<<<<<<< HEAD
    // submenu generate
=======
    //! submenu generate
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    generateMenu= tollsMenu->addMenu(tr("&Generate to"));
    generateMenu->addAction(printPdfAction);
    generateMenu->addAction(snapshotAction);
    
    tollsMenu->addAction(shortAction);
    
    tollsMenu->addSeparator();
    tollsMenu->addAction(optionsAction);

<<<<<<< HEAD
    // help menu
=======
    //! help menu
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(homePageAction);
    helpMenu->addSeparator();
    helpMenu->addAction(helpAction);
    helpMenu->addAction(aboutAction);
<<<<<<< HEAD
    helpMenu->addAction(showLicenseAction);
    helpMenu->addSeparator();
    helpMenu->addAction(updateAction);
    helpMenu->addAction(versionAction);
    helpMenu->addAction(bugReportAction);
}

// functions for Shortcusts --------------------------------------------------------------------------
=======
    helpMenu->addSeparator();
    helpMenu->addAction(updateAction);
    helpMenu->addAction(bugReportAction);
}

//! functions for Shortcusts --------------------------------------------------------------------------
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59

void MainWindow::setShort()
{
    set_shortcuts = new QDialog();
    QPushButton *Savebutton = new QPushButton("OK", set_shortcuts);
    QPushButton *Closebutton = new QPushButton("Close", set_shortcuts);

<<<<<<< HEAD
    m_table = new QTableWidget(17, 2, set_shortcuts);
=======
    m_table = new QTableWidget(7, 2, set_shortcuts);
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59

    m_table->setHorizontalHeaderItem(0, new QTableWidgetItem("Function"));
    m_table->setHorizontalHeaderItem(1, new QTableWidgetItem("Shortcut"));
    m_table->setItem(0,0, new QTableWidgetItem("New"));
    m_table->setItem(1,0, new QTableWidgetItem("Open"));
    m_table->setItem(2,0, new QTableWidgetItem("Revert"));
    m_table->setItem(3,0, new QTableWidgetItem("Save"));
    m_table->setItem(4,0, new QTableWidgetItem("Close"));
    m_table->setItem(5,0, new QTableWidgetItem("Print"));
    m_table->setItem(6,0, new QTableWidgetItem("Edit plain text"));
<<<<<<< HEAD
    m_table->setItem(7,0, new QTableWidgetItem("New tab"));
    m_table->setItem(8,0, new QTableWidgetItem("Undo"));
    m_table->setItem(9,0, new QTableWidgetItem("Redo"));
    m_table->setItem(10,0, new QTableWidgetItem("Cut"));
    m_table->setItem(11,0, new QTableWidgetItem("Copy"));
    m_table->setItem(12,0, new QTableWidgetItem("Paste"));
    m_table->setItem(13,0, new QTableWidgetItem("Delete"));
    m_table->setItem(14,0, new QTableWidgetItem("Select all"));
    m_table->setItem(15,0, new QTableWidgetItem("Find"));
    m_table->setItem(16,0, new QTableWidgetItem("Find & raplace"));


    QFile file(":/files/shortcuts.ini");
=======

    QFile file(":/files/shortcutsFile");
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
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
<<<<<<< HEAD
    m_table->resize(245,440);
    set_shortcuts->resize(250,490);
    Savebutton->move(30,450);
    Closebutton->move(140,450);
=======
    m_table->resize(220,240);
    set_shortcuts->resize(225,290);
    Savebutton->move(20,250);
    Closebutton->move(130,250);
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    set_shortcuts->show();

    QObject::connect(Savebutton, SIGNAL(clicked()),this,SLOT(savedShortcuts()));
    QObject::connect(Closebutton, SIGNAL(clicked()),this,SLOT(closeShortcuts()));
}


void MainWindow::savedShortcuts()
{
<<<<<<< HEAD
    QFile file("shortcuts.ini");
=======
    QFile file(":/files/shortcutsFile");
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
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
<<<<<<< HEAD
    textstring = m_table->item(7,1)->text();
    newTabAction->setShortcut((textstring));
    textstring = m_table->item(8,1)->text();
    undoAction->setShortcut((textstring));
    textstring = m_table->item(9,1)->text();
    redoAction->setShortcut((textstring));
    textstring = m_table->item(10,1)->text();
    cutAction->setShortcut((textstring));
    textstring = m_table->item(11,1)->text();
    copyAction->setShortcut((textstring));
    textstring = m_table->item(12,1)->text();
    pasteAction->setShortcut((textstring));
    textstring = m_table->item(13,1)->text();
    deleteAction->setShortcut((textstring));
    textstring = m_table->item(14,1)->text();
    selectAllAction->setShortcut((textstring));
    textstring = m_table->item(15,1)->text();
    findAction->setShortcut((textstring));
    textstring = m_table->item(16,1)->text();
    find_ReplaceAction->setShortcut((textstring));
=======
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    set_shortcuts->close();
}

void MainWindow::closeShortcuts()
{
    set_shortcuts->close();
}
<<<<<<< HEAD
//End functions for Shortucts -------------------------------------------------------------------------------
=======
//! End functions for Shortucts -------------------------------------------------------------------------------
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59


void MainWindow::createToolBars()
{
    try
    {
<<<<<<< HEAD
        // basic toolbar
=======
        //! basic toolbar
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
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
<<<<<<< HEAD
        //Style Combox for language
=======
        //! Style Combox for language
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
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
<<<<<<< HEAD
        // editor toolbar
=======
        //! editor toolbar
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
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

<<<<<<< HEAD
// for attach file as img ...
=======
//! for attach file as img ...
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
void MainWindow::attachFile()
{
    QMessageBox::information(this,"title","On Function is working!");
}


void MainWindow::createToolsToolbars()
{
    try
    {
<<<<<<< HEAD
        // tools toolbar
=======
        //! tools toolbar
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
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
<<<<<<< HEAD
        // format toolbar
=======
        //! format toolbar
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
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
        setSizeFont->addItem("20"); // lep�ie asi ako spinbox
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
<<<<<<< HEAD
    //check tab, if tab is Start page open new tab
    if(tabWidget->currentIndex()==0)
    {
        newTab();
    }
    else if (tabWidget->currentIndex()!=0)
    {

        qDebug("newFile()");
=======
    //! check tab, if tab is Start page open new tab
    if((tabWidget->tabText(tabWidget->currentIndex())).compare("Start page")==0)
    {
        newTab();
        newFile();
    }
    else
    {
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
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
<<<<<<< HEAD
    if(tabWidget->count()==1){
        return;
    }
    tabWidget->removeTab(position);
=======
    if((tabWidget->tabText(position)).compare("Start page")==0) return;
    else tabWidget->removeTab(position);
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
}

void MainWindow::tabChanged(int position)
{
<<<<<<< HEAD
    qDebug("tabChanged()");
    BlockGroup *group=getScene()->selectedGroup();
        if(group==0){
            qDebug("tabChanged() group=0");
=======
    BlockGroup *group=getScene()->selectedGroup();
        if(group==0){
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
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

<<<<<<< HEAD
    //button addTab
=======
    //! button addTab
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    QIcon addTabIcon(":/icons/plusTab.png");
    QPushButton *m_addButton = new QPushButton(addTabIcon,"", this);
    m_addButton->setObjectName("addButton");

<<<<<<< HEAD
    //set style for buton- add tab
=======
    //! set style for buton- add tab
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
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
<<<<<<< HEAD

=======
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
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
<<<<<<< HEAD
=======
//        scriptsBox->setEnabled(true);
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    }
    else
    {
        groupActions->setEnabled(true);
        searchLineEdit->setEnabled(true);
<<<<<<< HEAD
=======
//        scriptsBox->setEnabled(false);
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59

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
<<<<<<< HEAD
    if(tabWidget->currentIndex()==0)
    {
        newTab();
=======
    if((tabWidget->tabText(tabWidget->currentIndex())).compare("Start page")==0)
    {
        newTab();
        open();
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
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

<<<<<<< HEAD

=======
//! search in main window
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
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
<<<<<<< HEAD

    //    QColor color;
    //    color.setBlue(250);
=======
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
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
<<<<<<< HEAD
    // frame setting
=======

    //! frame setting
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
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


<<<<<<< HEAD

=======
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
void MainWindow::hideArea()
{
    for(int i=0; i<list.size(); i++)
        getScene()->removeItem(list.at(i));
}

<<<<<<< HEAD


=======
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());

    if (action)
    {
        open(action->data().toString());
    }
}


<<<<<<< HEAD
//FUNTIONS FOR EDIT MENU ---------------------------------------------------------------------------------
=======
//! FUNTIONS FOR EDIT MENU ---------------------------------------------------------------------------------
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59

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

<<<<<<< HEAD
=======
//! window for find keyword
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
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
  // tla�idlo find bude hladat dal
}

<<<<<<< HEAD
// for find
=======
//! search for find
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
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

<<<<<<< HEAD
//END OF FUNCTIONS FOR EDIT MENU -------------------------------------------------------------------------



//FUNTIONS FOR VIEW MENU ---------------------------------------------------------------------------------


// basic toolbar
=======
//! END OF FUNCTIONS FOR EDIT MENU -------------------------------------------------------------------------



//! FUNTIONS FOR VIEW MENU ---------------------------------------------------------------------------------


//! basic toolbar
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
void MainWindow::basicToolbar()
{
    QMessageBox::information(this,"title","On Function is working!");
}

<<<<<<< HEAD
// formating toolbar
=======
//! formating toolbar
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
void MainWindow::formatToolbars()
{
    createFormatingToolbars();
}

<<<<<<< HEAD
// tools toolbar
=======
//! tools toolbar
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
void MainWindow::toolsToolbar()
{
     createToolsToolbars();
}

<<<<<<< HEAD
// editor toolbar
=======
//! editor toolbar
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
void MainWindow::editorToolbar()
{
    createEditorToolbars();
}

<<<<<<< HEAD
// bottom dock
=======
//! bottom dockpanel - for bug,task list
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
void MainWindow::setBottomDock()
{
    dock = new QDockWidget(tr("Buttom dock"), this);
    dock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
<<<<<<< HEAD
    //add tab to dock panel
=======
    dock->setFeatures(QDockWidget::DockWidgetClosable);
    dock->setStyle(new QPlastiqueStyle);

    //! add tab to buttom dockpanel
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
    QTabWidget *tabView = new QTabWidget();
    tabView->addTab (new QTextEdit,("&Task list"));
    tabView->addTab(new QTextEdit,("&Bug list"));
    tabView->addTab(new QTextEdit,("&Note"));

    dock->setWidget(tabView);
    addDockWidget(Qt::BottomDockWidgetArea, dock);
}

<<<<<<< HEAD
// right dock
=======
//! right dockpanel - for fileBrowser
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
void MainWindow::setRightDock()
{
      dock1 = new QDockWidget(tr("File browser tree"), this);
      dock1->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
<<<<<<< HEAD
      //text1 = new QTextEdit(dock1);

      QFileSystemModel *model = new QFileSystemModel();
      model->setRootPath(QDir::homePath());

      QTreeView *tree = new QTreeView(dock1);
      tree->setModel(model);
      tree->setRootIndex(model->index(QDir::homePath()));
=======
      dock1->setFeatures(QDockWidget::DockWidgetClosable);
      dock1->setStyle(new QPlastiqueStyle);

      model = new QDirModel();
      tree = new QTreeView(dock1);
      tree->setModel(model);
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
      dock1->setWidget(tree);

      addDockWidget(Qt::RightDockWidgetArea, dock1);
}

<<<<<<< HEAD
// full screen
=======
//! full screen
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
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
<<<<<<< HEAD

}

// new window
=======
}

//! new window
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
void MainWindow::newWindow()
{
    QMessageBox::information(this,"title","On Function is working!");
}

<<<<<<< HEAD
// zoom in
=======
//! zoom in
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
void MainWindow::zoomIn()
{
    getScene()->adjustScale(1.2);
}

<<<<<<< HEAD
// zoom out
=======
//! zoom out
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
void MainWindow::zoomOut()
{
    getScene()->adjustScale(-1.2);
}

<<<<<<< HEAD
// split
=======
//! split
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
void MainWindow::split()
{
    this->showNormal();
    formatToolBar->show();
}

<<<<<<< HEAD
// END OF FUNCTIONS FOR VIEW MENU ------------------------------------------------------------------------



//FUNTIONS FOR TOOLS MENU ---------------------------------------------------------------------------------

// set Lua language
=======
//! END OF FUNCTIONS FOR VIEW MENU ------------------------------------------------------------------------



//! FUNTIONS FOR TOOLS MENU ---------------------------------------------------------------------------------

//! set Lua language
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
void MainWindow:: setLanguageLua()
{
    QMessageBox::information(this,"title","On Function is working!");
}

<<<<<<< HEAD
// set C language
=======
//! set C language
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
void MainWindow:: setLanguageC()
{
    QMessageBox::information(this,"title","On Function is working!");
}

<<<<<<< HEAD
// set Xml language
=======
//! set Xml language
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
void MainWindow:: setLanguageXml()
{
    QMessageBox::information(this,"title","On Function is working!");
}

<<<<<<< HEAD
// genarate to snapshot
=======
//! genarate to snapshot
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
void MainWindow::snapshot()
{
    QMessageBox::information(this,"title","On Function is working!");
}

<<<<<<< HEAD
// task list
=======
//! task list
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
void MainWindow::taskList()
{
    setBottomDock();
}

<<<<<<< HEAD
// bug list
=======
//! bug list
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
void MainWindow::bugList()
{
    setBottomDock();
}

<<<<<<< HEAD
// options
void MainWindow::options()
{
     setOptions = new QDialog();

     setOptions->setWindowTitle("Options");
     setOptions->resize(520,390);
     setOptions->show();
}

// sw metrics
=======
//! options
void MainWindow::options()
{
    setting *windowOption = new setting;
    windowOption->showSetting();
}

//! sw metrics
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
void MainWindow::swMetrics()
{
    QMessageBox::information(this,"title","On Function is working!");
}

<<<<<<< HEAD
// rum CMD
=======
//! rum CMD
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
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

<<<<<<< HEAD
// END OF FUNCTIONS FOR TOOLS MENU ------------------------------------------------------------------------
=======
//! END OF FUNCTIONS FOR TOOLS MENU ------------------------------------------------------------------------
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59




<<<<<<< HEAD
//FUNTIONS FOR HELP MENU ----------------------------------------------------------------------------------

// about TrollEdit
void MainWindow::about()
{
    QMessageBox::about(this, tr("About TrollEdit"),
                       tr("<p><img src=\":/img/logoBig\" aling=\"center\" width=\"\"/></p>"

                          "</br>"
                          "<p><b>What is TrollEdit?</b></p>"
                          "<p>TrollEdit is a Qt based text editor developed by students at <a href=\"http://www.fiit.stuba.sk/generate_page.php?page_id=749\">Slovak University of Technology.</a>"
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
=======
//! FUNTIONS FOR HELP MENU ----------------------------------------------------------------------------------

//! about TrollEdit
void MainWindow::about()
{
    aboutTrollEdit *aboutWindow = new aboutTrollEdit;
    aboutWindow->showWindow();
}

//! show TrollEdit web page
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
void MainWindow::homePage()
{
    QDesktopServices::openUrl(QUrl("http://innovators-team10.github.com"));
}

<<<<<<< HEAD
// html help
void MainWindow::help()
{
    QDialog *okno = new QDialog();
    QWebView *view = new QWebView(okno);
    QToolBar *helpToolBar = new QToolBar(tr("&Navigation"),okno);

    helpToolBar->addAction(view->pageAction(QWebPage::Back));
    helpToolBar->addAction(view->pageAction(QWebPage::Forward));
    helpToolBar->addAction(view->pageAction(QWebPage::Reload));
    helpToolBar->addAction(view->pageAction(QWebPage::Stop));
    view->load(QUrl("http://innovators-team10.github.com/u-manual_simple.html"));
    view->resize(900,700);
    okno->setFixedSize(900,700);
    okno->setWindowIcon (QIcon(":/icon16"));
    okno->setWindowTitle("On-line help");
    okno->show();
}

// check new update
=======
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
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
void MainWindow::update()
{
    QMessageBox::information(this,"title","On Function is working!");
}

<<<<<<< HEAD
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
=======
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59

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


<<<<<<< HEAD
// set size of Mainwindow
=======
//! set size of Mainwindow
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
void MainWindow::readSettings()
{
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    QPoint pos = settings.value("pos", QPoint(100, 100)).toPoint();
    QSize size = settings.value("size", QSize(500, 300)).toSize();
    resize(size);
    move(pos);

<<<<<<< HEAD
    if (settings.value("minimized", false).toBool()) //predtym maximized
=======
    if (settings.value("minimized", false).toBool())
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
        showMinimized();
}

void MainWindow::writeSettings()
{
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    settings.setValue("pos", pos());
    settings.setValue("size", size());
<<<<<<< HEAD
    settings.setValue("minimized", isMinimized()); //predtym maximized
=======
    settings.setValue("minimized", isMinimized());
>>>>>>> ae1be7bbc6558fe8c94e08687561d76b7a7c6b59
}
