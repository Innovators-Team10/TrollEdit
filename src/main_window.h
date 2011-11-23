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

typedef struct pokus
{
        int test;
} POKUS;

class DocumentScene;
class LanguageManager;
class BlockGroup;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QString programPath, QWidget *parent = 0);

public slots:
    void open(QString fileName);
    void setModified(bool flag);
    void setCurrentFile(BlockGroup *group);

signals:
    void apply(QBrush *brush, QPen *pen);

private slots:
    void newFile();
    void open();
    void openRecentFile();
    void about();
    void help();
    void settings();
    void langChanged(QString);
    void search();

    void printPdf();
    void showPrintableArea();

protected:
    void closeEvent(QCloseEvent *event);

private:
    enum { MaxRecentFiles = 6 };
    QActionGroup *groupActions;     //! used to disable subset of actions when no group is selected

    QAction *aboutQtAction;
    QAction *newAction;
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
    QComboBox *scriptsBox;
    QLineEdit *searchLineEdit;
    QLabel *searchLabel;

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
	QPointF startPoint;
    BlockGroup *selectedGroup;

    void readSettings();
    void writeSettings();
};

#endif // MAIN_WINDOW_H
