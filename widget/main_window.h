#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>

class DocumentTabs;
class DocumentScene;
class LanguageManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QString programPath, QWidget *parent = 0);

public slots:
    void setCurrentFile(int tabNumber);
    void applyChanges();
    void open(QString fileName);

signals:
    void apply(QBrush *brush, QPen *pen);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void setCurrentScene(int tabNumber);
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

    LanguageManager *langManager;
    DocumentTabs *documentTabs;
    DocumentScene *currentScene;
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
