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

class DocumentScene;
class LanguageManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QString programPath, QWidget *parent = 0);

public slots:
    void open(QString fileName);

signals:
    void apply(QBrush *brush, QPen *pen);

private slots:
    void newFile();
    void open();
    void openRecentFile();
    void about();
    void help();
    void settings();

    void printPdf();
    void showPrintableArea();

protected:
    void closeEvent(QCloseEvent *event);

private:
    enum { MaxRecentFiles = 6 };

    QAction *aboutQtAction;
    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *saveAllAction;
    QAction *closeAction;
    QAction *closeAllAction;
    QAction *printPdfAction;
    QAction *printableAreaAction;
    QAction *recentFileActions[MaxRecentFiles];
    QAction *separatorAction;
    QAction *exitAction;

    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *deleteAction;

    QAction *settingsAction;

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
    DocumentScene *scene;
    QHash<QString, QPair<QFont, QColor> > *highlightFormats;

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

    void showArea();
    void hideArea();

    void readSettings();
    void writeSettings();
};

#endif // MAIN_WINDOW_H
