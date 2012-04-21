/**
 * block_group.h
 *  ---------------------------------------------------------------------------
 * Contains the declaration of class BlockGroup and it's funtions and identifiers
 *
 */

#ifndef BLOCK_GROUP_H
#define BLOCK_GROUP_H

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

#include "analyzer.h"
#include "text_group.h"

class Block;
class DocBlock;
class DocumentScene;
class FoldButton;

class BlockGroup : public QObject, public QGraphicsRectItem
{
    Q_OBJECT


public:
    BlockGroup(QString text, QString file, DocumentScene *scene);

    ~BlockGroup();

    enum { Type = UserType + 2 };
    enum InsertLine
    {
        None = 0, Horizontal = 1, Vertical = 2,
    };

    // main properties
    int type() const { return Type; }
    Block *mainBlock() const {return root;}
    int getLastLine() const {return lastLine;}
    QString getFilePath() const {return fileName;}
    void setFileName(QString newFileName) {fileName = newFileName;}
    bool isModified() const {return modified;}
    void setModified(bool flag);
    void setContent(QString content);
    void changeMode(QList<QAction *> actionList);
    void changeMode();

    // block management
    Block *getBlockIn(int line) const;
    void setBlockIn(Block *block, int line);
    bool addFoldable(Block *block);
    void removeFoldable(Block *block);

    void selectBlock(Block *block, bool updateNeeded = false);
    void deselect(Block *until = 0, bool updateNeeded = false);
    Block* selectedBlock() const {return selected;}
    Block* blockAt(QPointF scenePos) const;
    int lineAt(QPointF scenePos) const;
    Block *addTextCursorAt(QPointF scenePos);
    TextGroup *getTextGroup();

    DocBlock *addDocBlock(QPointF scenePos);
    QList<DocBlock*> docBlocks() const;
    void highlightLines(QSet<int> lines);
    void highlightON_OFF();
    bool searchBlocks(QString searchStr, bool allowInner, bool exactMatch);
    void clearSearchResults();

    // analysis
    void setAnalyzer(Analyzer *newAnalyzer) {analyzer = newAnalyzer;}
    Analyzer *getAnalyzer() const {return analyzer;}
    Block *reanalyze(Block* block = 0, QPointF cursorPos = QPointF());
    void analyzeAll(QString text);
    bool reanalyzeBlock(Block* block);
    QString toText(bool noDocs = false) const;
    
    // paralelism
    QFutureWatcher<TreeElement*> watcher;
    QFuture<TreeElement*> future;
    QMutex mutex;
    TreeElement *groupRootEl;
    bool runParalelized;
    TreeElement* analazyAllInMaster (QString text);
    void updateAllInMaster (TreeElement* rootEl);
        
    // visualization
    void showInsertLine(InsertLine type, QPointF scenePos);
    QPainterPath shape() const;
    QRectF boundingRect() const;
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

    DocumentScene *docScene;    //! my scene
    bool highlight;

signals:


public slots:
    void keyTyped(QKeyEvent* event);
    void splitLine(Block *block, int cursorPos);
    void eraseChar(Block *block, int key);
    void moveFrom(Block *block, int key, int cursorPos);
    void updateSize();
    TreeElement* analazyAllInThread (QString text);
    void updateAllInThreads ();

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
    void setRoot(Block *newRoot);
    void moveCursorUpDown(Block *start, bool moveUp, int from);
    void moveCursorLeftRight(Block *start, bool moveRight);

    void computeTextSize();

    // fields
    TextGroup *txt;
    QString fileName;           //! name of currently loaded file
    Analyzer *analyzer;         //! my analyzer
    Block *root;                //! main (root) block
    Block *selected;            //! currently selected block
    QList<Block*> lineStarts;   //! line starting blocks
    int lastLine;               //! curent last line
    QSet<Block*> foldableBlocks;//! foldable blocks, only 1 per line allowed
    qreal lastXPos;
    QGraphicsLineItem *horizontalLine, *verticalLine; //! insertion cues
    bool modified;
    QHash<int, QGraphicsRectItem*> highlightingRects;
    bool searched;

    friend class DocumentScene;
};

#endif // BLOCK_GROUP_H
