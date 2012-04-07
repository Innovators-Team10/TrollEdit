/**
 * main_window.h
 *  ---------------------------------------------------------------------------
 * Contains the declaration of class MainWindow and it's funtions and identifiers
 *
 */

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H


#include <QMainWindow>
#include <QtGui>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPainter>
#include <QList>
#include <QTableView>

extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

typedef struct pokus
{
        int test;
} POKUS;


class DocumentScene;
class LanguageManager;
class BlockGroup;
class QTableWidget;
class QTableWidgetItem;
class QDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QString programPath, QWidget *parent = 0);
    DocumentScene* getScene();
    LanguageManager* getLangManager();
    QComboBox* getScriptBox();
    lua_State* getLuaState();


public slots:
    void open(QString fileName);
    void setModified(bool flag);
    void setCurrentFile(BlockGroup *group);

signals:
    void apply(QBrush *brush, QPen *pen);

private slots:
    void closeGroupWrapper();
    void revertGroupWrapper();
    void saveGroupWrapper();
    void saveGroupAsWrapper();
    void saveAllGroupsWrapper();
    void saveGroupAsWithoutDocWrapper();
    void closeAllGroupsWrapper();
    void showPreviewWrapper();
    void cleanGroupWrapper();

    void newFile();
    void newTab();
    void closeTab(int );
    void tabChanged(int );
    void open();
    void openRecentFile();
    void about();
    void help();
    void homePage();
    void showLicense();
    void update();
    void aboutVersion();
    void bugReport();
    void settings(); // tato funkcia akože èo robi?
    void langChanged(QString);
    void search();
    void options();
    void taskList();
    void twoMode();
    void startUp();
    void fullScreen();
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void delet();
    void selectAll();
    void find();
    void find_Replace();
    void showCmd();
    void newWindow();
    void zoomIn();
    void zoomOut();
    void split();
    void snapshot();
    void bugList();

    void basicToolbar();
    void formatingToolbar();
    void webToolbar();
    void editorToolbar();
    void setBottomDock();
    void setRightDock();

    void setLanguageLua();
    void setLanguageC();
    void setLanguageXml();

    void printPdf();
    void swMetrics();
    void showPrintableArea();
    void setShort();
    void savedShortcuts();
    void closeShortcuts();
    void wInit();

protected:
    void closeEvent(QCloseEvent *event);

private:
    lua_State* L;                   //! load configuration from lua
    enum { MaxRecentFiles = 6 };
    QActionGroup *groupActions;     //! used to disable subset of actions when no group is selected
    
    // for file menu
    QAction *newAction;
    QAction *newTabAction;
    QAction *openAction;
    QAction *revertAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *saveAllAction;
    QAction *saveAsNoDocAction;
    QAction *closeAction;
    QAction *closeAllAction;
    QAction *printPdfAction;
    QAction *printableAreaAction;
    QAction *plainEditAction;
    QAction *recentFileActions[MaxRecentFiles];
    QAction *separatorAction;
    QAction *clearAction;
    QAction *exitAction;

    // for edit menu
    QAction *undoAction;
    QAction *redoAction;
    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *deleteAction;
    QAction *selectAllAction;
    QAction *findAction;
    QAction *find_ReplaceAction;

    QAction *settingsAction;  // to je t zbytocne uplne aj tak nic nerobi tak  funkcia

    // for help menu
    QAction *homePageAction;
    QAction *helpAction;
    QAction *aboutAction;
    QAction *updateAction;
    QAction *versionAction;
    QAction *bugReportAction;
    QAction *showLicenseAction;

    // for view menu
    QAction *twoModeAction;
    QAction *startUpAction;
    QAction *fullScreenAction;
    QAction *newWindowAction;
    QAction *zoomInAction;
    QAction *zoomOutAction;
    QAction *splitAction;
    QAction *basicToolbarAction;
    QAction *formatingToolbarAction;
    QAction *webToolbarAction;
    QAction *editorToolbarAction;
    QAction *setBottomDockAction;
    QAction *setRightDockAction;

    // for tools menu
    QAction *shortAction;
    QAction *optionsAction;
    QAction *metricsAction;
    QAction *setCAction;
    QAction *setLuaAction;
    QAction *setXmlAction;
    QAction *showCmdAction;
    QAction *snapshotAction;
    QAction *bugListAction;
    QAction *taskListAction;

    QAction *textBoldAction;
    QAction *textItalicAction;
    QAction *textUnderlineAction;

    // items for menu
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *tollsMenu;
    QMenu *helpMenu;
    QMenu *generateMenu;
    QMenu *toolbarsMenu;
    QMenu *languageMenu;
    QMenu *setToolbarsMenu;
    QMenu *panelsMenu;

    QToolBar *formatToolBar;
    QTabWidget *tabWidget;
    QSplashScreen *ico;
    QComboBox *scriptsBox;
    QLineEdit *searchLineEdit;
    QLabel *logoLabel;
    QDialog *set_shortcuts;

    // for function a find
    QDialog *findWindow;


    QDialog *aboutVersionWindow;
    QLabel *aboutVersionLabel;
    QVBoxLayout *outer;
    QTextEdit *aboutVersionTextEdit;

    QDialog *setOptions;
    QListWidget *zoznam;

    //for dock panels
    QDockWidget *dock;
    QTextEdit *text;
    QDockWidget *dock1;
    QTextEdit *text1;


    LanguageManager *langManager;

    //DocumentScene *scene;
    QHash<QString, QPair<QFont, QColor> > *highlightFormats;

    QPrinter *printer;
    QPainter *painter;

    QGraphicsLineItem *line;
    QList<QGraphicsLineItem *> list;

    QGraphicsView* createView();
    QTableWidget *m_table;

    void initLuaState();
    void createActions();
    void createMenus();
    void createGlobalActions();
    void disconnectAll();
    void createTabs();
    void createToolBars();
    QString strippedName(const QString &fullFileName);
    void updateRecentFileActions();
    void load(QString fileName);

    void showArea();
    void hideArea();
    QPointF startPoint;
    BlockGroup *selectedGroup;

    void readSettings();
    void writeSettings();
};

#endif // MAIN_WINDOW_H
