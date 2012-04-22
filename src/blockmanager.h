/**
 * blockmanager.h
 *  ---------------------------------------------------------------------------
 * Contains the declaration of class BlockManager and it's funtions and identifiers, this class is parent of BlockGroup and TextGroup
 *
 */

#ifndef BLOCKMANAGER_H
#define BLOCKMANAGER_H

#include <QObject>
#include <QGraphicsRectItem>
#include <QSet>
#include <QTime>
#include <QStatusBar>
#include <QtConcurrentRun>
#include <QFuture>
#include <QFutureWatcher>
#include <QThreadPool>
#include <QMessageBox>
#include <QMutex>

class Block;
class BlockGroup;
class DocBlock;
class DocumentScene;
class FoldButton;
class TextGroup;
class MainWindow;

class BlockManager : public QObject{
    Q_OBJECT

public:
    BlockManager(QString text, QString file, DocumentScene *scene);

    ~BlockManager();

    // main properties
    QString getFilePath() const {return fileName;}
    void setFileName(QString newFileName) {fileName = newFileName;}
    bool isModified() const {return modified;}
    void setModified(bool flag);
    void setContent(QString content);
    void changeMode(QList<QAction *> actionList);
    void changeMode();
    TextGroup *getTextGroup();
    QString getText(){return text;}
    QString getFile(){return file;}
    DocumentScene *getDocScene(){return docScene;}
    int getMode(){return mode;}

    bool searchBlocks(QString searchStr, bool allowInner, bool exactMatch);
    void clearSearchResults();

    // visualization
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    bool smoothTextAnimation;

    // helpers
    static QStatusBar *getStatusBar();
    static QList<Block*> blocklist_cast(QList<QGraphicsItem*> list);
    QTime time; //! used for benchmarking

    // constants
    static const QPointF OFFSET_IN_TL, OFFSET_IN_BR, OFFSET_OUT,
        OFFSET_INSERT, NO_OFFSET;
    static const QString BLOCK_MIME;
    int TAB_LENGTH;
    qreal CHAR_HEIGHT, CHAR_WIDTH;
signals:


public slots:
    void keyTyped(QKeyEvent* event);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {Q_UNUSED(event);}
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
    void dropEvent(QGraphicsSceneDragDropEvent *event);

private:
    void moveCursorUpDown(Block *start, bool moveUp, int from);
    void moveCursorLeftRight(Block *start, bool moveRight);

    void computeTextSize();

    // fields
    int mode;
    QString text;
    QString file;
    QString fileName;           //! name of currently loaded file
    qreal lastXPos;
    bool modified;
    bool searched;
    bool selected;
    DocumentScene *docScene;    //! my scene

    friend class DocumentScene;
};

#endif // BLOCKMANAGER_H
