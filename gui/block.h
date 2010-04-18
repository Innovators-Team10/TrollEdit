#ifndef BLOCK_H
#define BLOCK_H

#include <QGraphicsRectItem>
#include <QtGui>
#include <QList>
#include <QPropertyAnimation>

class TreeElement;
class DocumentScene;
class HideBlockButton;
class TextItem;

class Block : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    Q_PROPERTY(QRectF geometry READ geometry WRITE setGeometry)

public:
    Block(TreeElement *element, Block *parentBlock, QGraphicsScene *parentScene = 0);
    ~Block();

    enum { Type = UserType + 1 };

    // overriden methods to provide AST consistency
    void setParentItem (QGraphicsItem *parent);
    void stackBefore (const QGraphicsItem *sibling);
    QList<Block*> removeBlock();

    // block management methods
    Block *getFirstLeaf() const;
    Block *getLastLeaf() const;
    Block *getAncestorWhereFirst() const;
    Block *getAncestorWhereLast() const;
    Block *getNextSibling() const;
    Block *getNext(bool textOnly = false) const;
    Block *getPrev(bool textOnly = false) const;

    // property getters and setters
    int type() const;
    Block *parentBlock() const;
    QList<Block*> childBlocks() const;
    TextItem *textItem() const;
    TreeElement *getElement() const;
    static int getLastLine(){return lastLine;}

    void setFolded(bool folded);
    bool isFolded() const;
    bool isTextBlock() const;
    int length() const;
    bool hasMoreLines() const;
    int numberOfLines() const;
//    void addSpaces(int count);
    int getSpaces() const;
    int getAbsoluteSpaces() const;
//    void setLine(int newLine);
    QRectF geometry() const;
    void setGeometry(QRectF geometry);
    void setScenePos(QPointF pos);

    // visualization
    QPainterPath shape() const;
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QList<Block*> blocklist_cast(QList<QGraphicsItem*> list) const;
    void highlight(QPair<QFont, QColor> format);

    void updateBlock();
//    void updateAfter(bool updateThis = false);
//    void updatePosAfter();
//    void updateLineStarts();
//    void updateXPosInLine(int lineNo);
    void updateAll(bool animate = true);
    void animate();

    static QMap<int, Block*> lineStarts;// move to private..
    void setSelected(bool flag = true);
    static Block* getSelectedBlock(){return selectedBlock;}

signals:
    void lostFocus(Block *block);

public slots:
    void textFocusChanged(QFocusEvent* event);
    void textChanged();
    void keyPressed(QKeyEvent* event);
    void splitLine(int cursorPos = -1);
    void eraseChar(int key);
    void moveCursorLR(int key);
    void moveCursorUD(int key, int from);

protected:
    QPointF computePos() const;
    QRectF computeRect() const;
    int computeLine() const;
    QPointF getOffset() const;

    Block* findFutureParentAt(QPointF pos) const;
    Block* findNextChildAt(QPointF pos) const;
    QLineF getInsertLineAt(const Block* nextBlock, bool insertedIsLineBreaking) const;

    // event processing
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);

    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

private:
    static QPointF OFFSET, NO_OFFSET;
    static int lastLine;

    bool folded;     // true when block is folded
    bool moveStarted;// true while block is moving
    bool edited;     // edited after last AST analysis
    bool showing;    // true when block border is painted
    bool toAnimate;
    static Block *selectedBlock;

    DocumentScene *docScene;    // my scene
    Block *parent;              // my parent
    TreeElement *element;       // my AST element
    TextItem *myTextItem;       // my text area (AST leafs only)
    int line;                   // my line (global index)
//    int spaces;                 // number of spaces visible before this block

    Block *nextSib, *prevSib, *firstChild;    // used to create links

    // graphic elements
    HideBlockButton *hideButton;

    void createControls();//todo
    void removeLinks();
    void setShowing(bool newState, Block* stopAt=0);

    Block *futureParent;
    Block *futureSibling;   // used for block insertion

    QPropertyAnimation *animation;
    QPointF lastPos;
};

#endif // BLOCK_H
