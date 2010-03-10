#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include "document_tabs.h"
#include "document_scene.h"
#include "language_manager.h"
//#include "settingsDialog.h"
//#include "helpbrowser.h"
//#include "settingspages.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);

public slots:
    void setCurrentFile(int tabNumber);
    void applyChanges();

signals:
    void apply(QBrush *brush, QPen *pen);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void openRecentFile();
    void about();    
    void help();
    void settings();
    bool closeTab();
    void handleFontChange();
    void analyzeTab();

private:
    enum { MaxRecentFiles = 4 };

    QAction *aboutQtAction;
    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *closeAction;
    QAction *recentFileActions[MaxRecentFiles];
    QAction *separatorAction;
    QAction *exitAction;

    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *deleteAction;

    QAction *stylesAction;

    QAction *helpAction;
    QAction *aboutAction;

    QAction *textBoldAction;
    QAction *textItalicAction;
    QAction *textUnderlineAction;

    QAction *analyzeAction;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *optionsMenu;
    QMenu *helpMenu;

    QToolBar *formatToolBar;

   // Analyzer *cAnalyzer;

    LanguageManager *langManager;
    DocumentTabs *documentTabs;
    QStringList recentFiles;
    QString currentFile;
    //SettingsDialog *settingsDialog;

    void createActions();
    void createMenus();
    void createToolBars();    
    QString strippedName(const QString &fullFileName);
    void updateRecentFileActions();
    void load(QString fileName);
    //void writeSettings();
    //void readSettings();
};

#endif // MAINWINDOW_H
