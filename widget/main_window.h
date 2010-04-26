#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPainter>
#include <QList>
#include <QBrush>
#include <QColor>
#include <QRectF>
#include <QLine>
#include <QGraphicsLineItem>

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

    void printPdf();
    void showPrintableArea();

private:
    enum { MaxRecentFiles = 4 };

    QAction *aboutQtAction;
    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *printPdfAction;
    QAction *printableAreaAction;
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
    QHash<QString, QPair<QFont, QColor> > *highlightFormats;
    QHash<QString, QHash<QString, QColor> > *blockFormats;
    //SettingsDialog *settingsDialog;

    QPrinter *printer;
    QPainter *painter;

    QGraphicsLineItem *line;
    QList<QGraphicsLineItem *> list;


    void createActions();
    void createMenus();
    void createToolBars();
    QString strippedName(const QString &fullFileName);
    void updateRecentFileActions();
    void load(QString fileName);
    void initializeHighlightning();
    void initializeBlockFormatting();
    bool toBool(QString textBool);    

    void showArea();
    void hideArea();
    //void writeSettings();
    //void readSettings();
};

#endif // MAIN_WINDOW_H
